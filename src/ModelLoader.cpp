#include "ModelLoader.h"
#include <iostream>

// [Part B] STUB: 这是一个桩函数，用于 Part A 开发测试。
// Part B 同学需要删除此处的硬编码，改为真实的文件读取。
Mesh* ModelLoader::LoadMesh(const std::string& path) {
    std::cout << "[Part B Stub] Loading model from: " << path << std::endl;

    // 临时：返回一个简单的三角形，证明接口调用成功
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Vertex v1, v2, v3;
    v1.Position = glm::vec3(-0.5f, -0.5f, 0.0f); v1.Normal = glm::vec3(0,0,1);
    v2.Position = glm::vec3( 0.5f, -0.5f, 0.0f); v2.Normal = glm::vec3(0,0,1);
    v3.Position = glm::vec3( 0.0f,  0.5f, 0.0f); v3.Normal = glm::vec3(0,0,1);

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);

    indices.push_back(0); indices.push_back(1); indices.push_back(2);

    return new Mesh(vertices, indices, textures);
}