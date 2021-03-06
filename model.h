#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
    std::vector<Vec3f> verts;
    std::vector<std::vector<int>> faces_points;
    std::vector<Vec3f> textures;
    std::vector<std::vector<int>> faces_textures;
    std::vector<std::vector<int>> faces_norm;
    std::vector<Vec3f> norms;
    TGAImage normal_map;
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    Vec3f vert(int i);
    int nfaces();
    std::vector<int> faces_point(int j);
    Vec3f texture(int i);
    std::vector<int> faces_texture(int j);
    Vec3f norm(int face, int nvert);
    Vec3f normal(Vec2f uvf);
};

#endif //__MODEL_H__