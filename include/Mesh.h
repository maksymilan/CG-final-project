#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"

// 顶点结构定义
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// 纹理结构定义
struct Texture {
    unsigned int id;
    std::string type; // diffuse, specular
    std::string path;
};

// Mesh 类：负责存储几何数据和渲染
// 职责：[Part C] 负责维护此类的内部实现（VAO/VBO管理）
class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    
    // 渲染网格
    void Draw(Shader &shader);

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

#endif