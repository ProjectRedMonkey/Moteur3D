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
 * Draw a line between two point
 * @param x0 abscissa of first point
 * @param y0 ordinate of the first point
 * @param x1 abscissa of second point
 * @param y1 ordinate of the second point
 * @param image to print the line
 * @param color choosen
 */
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
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
 * Draw a triangle
 */
void triangle(int x0, int y0, int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color){
    line(x0, y0, x1, y1, image, color);
    line(x0, y0, x2, y2, image, color);
    line(x1, y1, x2, y2, image, color);
}

int main() {
    model = new Model("obj/african_head.obj");
    TGAImage image(width, height, TGAImage::RGB);

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

        triangle(x0, y0, x1, y1, x2, y2, image, white);
    }

    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
