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
        if (!line.compare(0, 2, "v ")) { //If the first letter of the line is v
            iss >> trash;
            Vec3f vertex;
            for (int i = 0; i < 3; i++){
                iss >> vertex.raw[i];
            }
            verts.push_back(vertex);
        }else if(!line.compare(0, 2, "f ")) { //If the first letter of the line is f
            std::vector<int> face;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) { //We're only interested by the first number
                idx--; //Because indice start at 1 in obj
                face.push_back(idx);
            }
            faces.push_back(face);
        }
    }
}

Model::~Model() {
}

int Model::nverts() {
    return verts.size();
}

Vec3f Model::vert(int i) {
    return verts[i];
}

int Model::nfaces() {
    return faces.size();
}

std::vector<int> Model::face(int j) {
    return faces[j];
}