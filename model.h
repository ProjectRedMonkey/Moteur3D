#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
    std::vector<Vec3f> verts;
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    Vec3f vert(int i);
};

#endif //__MODEL_H__