#ifndef COMMON_H
#define COMMON_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

// 定义顶点结构
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// 定义纹理结构
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

#endif