#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "model.h"

Model::Model(const char *filename) : verts(){
    std::ifstream in; //File reader
    in.open (filename, std::ifstream::in);
    std::string line;
    while (!in.eof()) { //Read line per line
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) { //If the first letter of the line is v
            iss >> trash;
            Vec3f vertex;
            for (int i = 0; i < 3; i++){
                iss >> vertex.raw[i];
            }
            verts.push_back(vertex);
        }
    }
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts.size();
}

Vec3f Model::vert(int i) {
    return verts[i];
}