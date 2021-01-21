#include <iostream>
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

/**
 * Draw a triangle and fill it
 */
void triangle(Vec2i A, Vec2i B, Vec2i C, TGAImage &image, TGAColor color){
        //Sorting points in ascending row order
        if(A.y > B.y){
            std::swap(A, B);
        }
        if(B.y > C.y){
            std::swap(B, C);
        }
        if(A.y > B.y){
            std::swap(A, B);
        }

        //Calculation of vectors
        Vec2i AB = {B.x - A.x, B.y - A.y};
        Vec2i AC = {C.x - A.x, C.y - A.y};
        Vec2i BC = {C.x - B.x, C.y - B.y};

        //Construction of the 2 extreme points for each line
        Vec2i pAB, pAC;

        //Travel of lines A to B only if A and B on different lines
        if(AB.y > 0){
            for(int l = 0 ; l <= AB.y - 1 ; l++){
                //Segments
                pAB.x = A.x + AB.x * l / AB.y;
                pAB.y = A.y + l;
                pAC.x = A.x + AC.x * l / AC.y;
                pAC.y = pAB.y;

                line(pAB, pAC, image, color);
            }
        }else{
            line(A, B, image, color);
        }

        Vec2i pBC;

        //Travel of lines B to C only if B and C on different lines
        if(BC.y > 0){
            for(int l = AB.y ; l <= AC.y ; l++){
                pBC.x = B.x + BC.x * (l - AB.y) / BC.y;
                pBC.y = A.y + l;
                pAC.x = A.x + AC.x * l / AC.y;
                pAC.y = pBC.y;

                line(pBC, pAC, image, color);
            }
        }else{
            line(B, C, image, color);
        }
}

int main() {
    model = new Model("obj/african_head.obj");
    TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir(0,0,-1);
    //Browse the faces
    for (int i=0; i<model->nfaces(); i++) {
        //For each face, display the triangle
        std::vector<int> face = model->face(i);
        Vec3f point1 = model->vert(face[0]);
        Vec3f point2 = model->vert(face[1]);
        Vec3f point3 = model->vert(face[2]);
        int x0 = (point1.x+1.)*width/2.;
        int y0 = (point1.y+1.)*height/2.;
        int x1 = (point2.x+1.)*width/2.;
        int y1 = (point2.y+1.)*height/2.;
        int x2 = (point3.x+1.)*width/2.;
        int y2 = (point3.y+1.)*height/2.;

        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            world_coords[j]  = model->vert(face[j]);
        }
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
        n.normalize();
        float intensity = n*light_dir;
        if (intensity>0) {
            triangle(Vec2i(x0, y0), Vec2i(x1, y1), Vec2i(x2, y2), image,
                     TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }

    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
