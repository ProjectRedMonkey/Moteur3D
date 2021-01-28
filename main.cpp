#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "tgaimage.h"
#include "model.h"

const int width  = 800;
const int height = 800;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
const TGAColor blue   = TGAColor(0, 0,   255,   255);
Model *model = NULL;

/**
 *  * Draw a line between two points
 * @param p0 first vertice
 * @param p1 second vertice
 * @param image to use
 * @param color choosen
 */
void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x) {
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    }
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = p0.y;
    for (int x = p0.x; x <= p1.x; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}


Vec3f barycentric(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f P) {
    Vec3f u = cross(Vec3f(p3.x-p1.x, p2.x-p1.x, p1.x-P.x), Vec3f(p3.y-p1.y, p2.y-p1.y,p1.y-P.y));
    if (std::abs(u[2])<1) return Vec3f(-1,1,1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

/**
 * Draw a faces_texture and fill it
 */
void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, Vec3f texture[3], float intensity, TGAImage textureImage) {
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;
            for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];
            if (zbuffer[int(P.x+P.y*width)]<P.z) {
                zbuffer[int(P.x+P.y*width)] = P.z;
                double u = bc_screen[0]*texture[0].x+bc_screen[1]*texture[1].x+bc_screen[2]*texture[2].x;
                double v = bc_screen[0]*texture[0].y+bc_screen[1]*texture[1].y+bc_screen[2]*texture[2].y;
                TGAColor color = textureImage.get(u*textureImage.get_width(), v*textureImage.get_height());
                image.set(P.x, P.y, color*intensity);
            }
        }
    }
}

Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}

int main() {
    model = new Model("obj/african_head.obj");
    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0,0,-1);
    float *zbuffer = new float[width*height];
    for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    TGAImage textureImage;
    textureImage.read_tga_file("obj/african_head_diffuse.tga");
    textureImage.flip_vertically();

    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> faces_points = model->faces_point(i);
        std::vector<int> faces_textures = model->faces_texture(i);
        Vec3f pts[3];
        Vec3f world_coords[3];
        Vec3f texture[3];
        for (int j=0; j<3; j++) {
            pts[j] = world2screen(model->vert(faces_points[j]));
            world_coords[j]  = model->vert(faces_points[j]);
            texture[j] = model->texture(faces_textures[j]);
        }
        Vec3f n = cross((world_coords[2]-world_coords[0]),(world_coords[1]-world_coords[0]));
        n.normalize();
        float intensity = n*light_dir;
        if (intensity>0) {
            triangle(pts, zbuffer, image, texture, intensity, textureImage);
        }
    }
    std::cout << "Bonsoir";
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
