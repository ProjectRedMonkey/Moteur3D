#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "tgaimage.h"
#include "model.h"

const int width  = 800;
const int height = 800;
const int depth  = 255;

Model *model = NULL;

Vec3f light_dir(0,0,-1);
Vec3f eye(10,0,1);
Vec3f center(0,0,0);

Vec3i m2v(Matrix m) {
    return Vec3i(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Vec3f m2v(Matrix m, int a) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}

Vec3f barycentric(Vec3i p1, Vec3i p2, Vec3i p3, Vec3f P) {
    Vec3f u = cross(Vec3f(p3.x-p1.x, p2.x-p1.x, p1.x-P.x), Vec3f(p3.y-p1.y, p2.y-p1.y,p1.y-P.y));
    if (std::abs(u[2])<1) return Vec3f(-1,1,1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = cross(up,z).normalize();
    Vec3f y = cross(z,x).normalize();
    Matrix res = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        res[0][i] = x[i];
        res[1][i] = y[i];
        res[2][i] = z[i];
        res[i][3] = -center[i];
    }
    return res;
}

/**
 * Draw a faces_texture and fill it
 */
void triangle(Vec3i *pts, float *zbuffer, TGAImage &image, Vec3f texture[3], float intensity, TGAImage textureImage) {
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], (float)pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], (float)pts[i][j]));
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
                image.set(P.x, P.y, TGAColor(color*intensity));
            }
        }
    }
}

int main() {
    model = new Model("obj/african_head.obj");

    Matrix ModelView  = lookat(eye, center, Vec3f(0,1,0));
    Matrix Projection = Matrix::identity(4);
    Matrix ViewPort   = viewport(width/8, height/8, width*3/4, height*3/4);
    Projection[3][2] = -1.f/(eye-center).norm();
    Matrix z = (ViewPort*Projection*ModelView);

    TGAImage image(width, height, TGAImage::RGB);

    float *zbuffer = new float[width*height];
    for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    TGAImage textureImage;
    textureImage.read_tga_file("obj/african_head_diffuse.tga");
    textureImage.flip_vertically();

    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> faces_points = model->faces_point(i);
        std::vector<int> faces_textures = model->faces_texture(i);
        Vec3i screen_coords[3];
        Vec3f screen_coords_light[3];
        Vec3f world_coords[3];
        Vec3f texture[3];
        for (int j=0; j<3; j++) {
            world_coords[j] = model->vert(faces_points[j]);
            screen_coords[j] = m2v(ViewPort*Projection*ModelView*v2m(world_coords[j]));
            screen_coords_light[j] = m2v(ViewPort*Projection*v2m(world_coords[j]), 0);
            texture[j] = model->texture(faces_textures[j]);
        }
        Vec3f n = cross(screen_coords_light[2]-screen_coords_light[0],screen_coords_light[1]-screen_coords_light[0]);
        n.normalize();
        float intensity = n*light_dir;
        triangle(screen_coords, zbuffer, image, texture, intensity, textureImage);

    }

    image.write_tga_file("output.tga");
    delete model;

    float zmin = +std::numeric_limits<float>::max();
    float zmax = -std::numeric_limits<float>::max();
    for (int i=width*height; i--;) {
        if (zbuffer[i]!=-std::numeric_limits<float>::max())
            zmin = std::min(zmin, zbuffer[i]);
        zmax = std::max(zmax, zbuffer[i]);
    }
    std::cerr << zmin << " " << zmax << std::endl;

    TGAImage zimg(width, height, TGAImage::GRAYSCALE);
    for (int i=width*height; i--;) {
        zimg.set(i%width, i/width, TGAColor(255*((zbuffer[i]-zmin)/(zmax-zmin))));
    }
    zimg.write_tga_file("zbuffer.tga");
    return 0;
}
