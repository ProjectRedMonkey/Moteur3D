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

    int main() {
        model = new Model("obj/african_head.obj");
        TGAImage image(width, height, TGAImage::RGB);

        //Browse all vertices of the model and print it
        for (int i = 0; i < model->nverts(); i++) {
            Vec3f point = model->vert(i);
            int x0 = (point.x+1.)*width/2.;
            int y0 = (point.y+1.)*height/2.;
            image.set(x0, y0, white);
        }

        image.write_tga_file("output.tga");
        delete model;
        return 0;
    }
