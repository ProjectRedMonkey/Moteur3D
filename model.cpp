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
        char trash; //Used to delete the first char
        if (!line.compare(0, 2, "v ")) { //Vertices
            iss >> trash;
            Vec3f vertex;
            for (int i = 0; i < 3; i++){
                iss >> vertex[i];
            }
            verts.push_back(vertex);
        }else if(!line.compare(0, 2, "f ")) { //Faces
            std::vector<int> face, texture;
            int itrash, idx, idx2;
            iss >> trash;
            while (iss >> idx >> trash >> idx2 >> trash >> itrash) { //We're only interested by the first number
                idx--; //Because indice start at 1 in obj
                idx2--;
                face.push_back(idx);
                texture.push_back(idx2);
            }
            faces_points.push_back(face);
            faces_textures.push_back(texture);
        }else if(!line.compare(0, 3, "vt ")) {//Textures
            iss >> trash;
            iss >> trash;
            Vec3f vertex(1,1,1);
            for (int i = 0; i < 3; i++){
                iss >> vertex[i];
            }
            textures.push_back(vertex);
        }
    }
}

Model::~Model() = default;

int Model::nverts() {
    return verts.size();
}

Vec3f Model::vert(int i) {
    return verts[i];
}

int Model::nfaces() {
    return faces_points.size();
}

std::vector<int> Model::faces_point(int j) {
    return faces_points[j];
}

Vec3f Model::texture(int i) {
    return textures[i];
}

std::vector<int> Model::faces_texture(int j) {
    return faces_textures[j];
}