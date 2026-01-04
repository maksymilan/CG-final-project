#include "ModelLoader.h"
#include "SceneContext.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstring>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Mesh* ModelLoader::LoadMesh(const std::string& path) {
    std::cout << "Loading OBJ model from: " << path << std::endl;

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texcoords;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open OBJ file: " << path << std::endl;
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        } else if (type == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        } else if (type == "vt") {
            glm::vec2 texcoord;
            iss >> texcoord.x >> texcoord.y;
            temp_texcoords.push_back(texcoord);
        } else if (type == "f") {
            std::string vertexData;
            std::vector<Vertex> faceVertices;
            
            while (iss >> vertexData) {
                std::istringstream vertexIss(vertexData);
                std::string indexStr;
                std::vector<unsigned int> indicesSet;

                while (std::getline(vertexIss, indexStr, '/')) {
                    if (!indexStr.empty()) {
                        indicesSet.push_back(std::stoi(indexStr));
                    } else {
                        indicesSet.push_back(0);
                    }
                }

                if (indicesSet.size() >= 1) {
                    unsigned int posIndex = indicesSet[0] - 1;
                    if (posIndex < temp_vertices.size()) {
                        Vertex vertex;
                        vertex.Position = temp_vertices[posIndex];

                        if (indicesSet.size() >= 3 && indicesSet[2] > 0 && !temp_normals.empty()) {
                            unsigned int normalIndex = indicesSet[2] - 1;
                            if (normalIndex < temp_normals.size()) {
                                vertex.Normal = temp_normals[normalIndex];
                            } else {
                                vertex.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
                            }
                        } else {
                            vertex.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
                        }

                        if (indicesSet.size() >= 2 && indicesSet[1] > 0 && !temp_texcoords.empty()) {
                            unsigned int texIndex = indicesSet[1] - 1;
                            if (texIndex < temp_texcoords.size()) {
                                vertex.TexCoords = temp_texcoords[texIndex];
                            } else {
                                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                            }
                        } else {
                            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                        }

                        faceVertices.push_back(vertex);
                    }
                }
            }
            
            if (faceVertices.size() >= 3) {
                for (unsigned int i = 1; i < faceVertices.size() - 1; i++) {
                    vertices.push_back(faceVertices[0]);
                    vertices.push_back(faceVertices[i]);
                    vertices.push_back(faceVertices[i + 1]);
                }
            }
        }
    }

    file.close();

    for (unsigned int i = 0; i < vertices.size(); i++) {
        indices.push_back(i);
    }

    return new Mesh(vertices, indices, textures);
}

bool ModelLoader::ExportMesh(const Mesh* mesh, const std::string& path) {
    if (!mesh) {
        std::cerr << "Error: Mesh is null" << std::endl;
        return false;
    }

    std::filesystem::path filePath(path);
    std::filesystem::path directory = filePath.parent_path();
    
    if (!directory.empty() && !std::filesystem::exists(directory)) {
        try {
            std::filesystem::create_directories(directory);
            std::cout << "Created directory: " << directory << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error: Failed to create directory: " << directory << ". Reason: " << e.what() << std::endl;
            return false;
        }
    }

    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file for writing: " << path << std::endl;
        return false;
    }

    file << "# Mesh exported from CG Project\n";
    file << "# Total vertices: " << mesh->vertices.size() << "\n";
    file << "# Total indices: " << mesh->indices.size() << "\n\n";

    for (const auto& vertex : mesh->vertices) {
        file << "v " << vertex.Position.x << " " << vertex.Position.y << " " << vertex.Position.z << "\n";
    }

    for (const auto& vertex : mesh->vertices) {
        file << "vt " << vertex.TexCoords.x << " " << vertex.TexCoords.y << "\n";
    }

    for (const auto& vertex : mesh->vertices) {
        file << "vn " << vertex.Normal.x << " " << vertex.Normal.y << " " << vertex.Normal.z << "\n";
    }

    for (unsigned int i = 0; i < mesh->indices.size(); i += 3) {
        unsigned int v1 = mesh->indices[i] + 1;
        unsigned int v2 = mesh->indices[i + 1] + 1;
        unsigned int v3 = mesh->indices[i + 2] + 1;
        file << "f " << v1 << "/" << v1 << "/" << v1 << " " << v2 << "/" << v2 << "/" << v2 << " " << v3 << "/" << v3 << "/" << v3 << "\n";
    }

    file.close();
    std::cout << "Successfully exported mesh to: " << path << std::endl;
    return true;
}

bool ModelLoader::ExportScene(const SceneContext* scene, const std::string& path) {
    if (!scene || scene->objects.empty()) {
        std::cerr << "Error: Scene is null or empty" << std::endl;
        return false;
    }

    std::filesystem::path filePath(path);
    std::filesystem::path directory = filePath.parent_path();
    
    if (!directory.empty() && !std::filesystem::exists(directory)) {
        try {
            std::filesystem::create_directories(directory);
            std::cout << "Created directory: " << directory << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error: Failed to create directory: " << directory << ". Reason: " << e.what() << std::endl;
            return false;
        }
    }

    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file for writing: " << path << std::endl;
        return false;
    }

    file << "# Scene exported from CG Project\n";
    file << "# Total objects: " << scene->objects.size() << "\n\n";

    unsigned int vertexOffset = 0;
    unsigned int texCoordOffset = 0;
    unsigned int normalOffset = 0;

    for (const auto& obj : scene->objects) {
        if (!obj->mesh) continue;

        file << "# Object: " << obj->name << "\n";        
        // Add object command to separate objects in OBJ file (Blender prefers 'o' over 'g')
        file << "o " << obj->name << "\n";
        // Also add group command for compatibility
        file << "g " << obj->name << "\n";

        for (const auto& vertex : obj->mesh->vertices) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj->position);
            model = glm::rotate(model, glm::radians(obj->rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(obj->rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(obj->rotation.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, obj->scale);
            
            glm::vec4 transformedPos = model * glm::vec4(vertex.Position, 1.0f);
            file << "v " << transformedPos.x << " " << transformedPos.y << " " << transformedPos.z << "\n";
        }

        for (const auto& vertex : obj->mesh->vertices) {
            file << "vt " << vertex.TexCoords.x << " " << vertex.TexCoords.y << "\n";
        }

        for (const auto& vertex : obj->mesh->vertices) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj->position);
            model = glm::rotate(model, glm::radians(obj->rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(obj->rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(obj->rotation.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, obj->scale);
            
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
            glm::vec3 transformedNormal = normalMatrix * vertex.Normal;
            file << "vn " << transformedNormal.x << " " << transformedNormal.y << " " << transformedNormal.z << "\n";
        }

        for (unsigned int i = 0; i < obj->mesh->indices.size(); i += 3) {
            unsigned int v1 = obj->mesh->indices[i] + 1 + vertexOffset;
            unsigned int v2 = obj->mesh->indices[i + 1] + 1 + vertexOffset;
            unsigned int v3 = obj->mesh->indices[i + 2] + 1 + vertexOffset;
            
            unsigned int vt1 = obj->mesh->indices[i] + 1 + texCoordOffset;
            unsigned int vt2 = obj->mesh->indices[i + 1] + 1 + texCoordOffset;
            unsigned int vt3 = obj->mesh->indices[i + 2] + 1 + texCoordOffset;
            
            unsigned int vn1 = obj->mesh->indices[i] + 1 + normalOffset;
            unsigned int vn2 = obj->mesh->indices[i + 1] + 1 + normalOffset;
            unsigned int vn3 = obj->mesh->indices[i + 2] + 1 + normalOffset;
            
            file << "f " 
                 << v1 << "/" << vt1 << "/" << vn1 << " " 
                 << v2 << "/" << vt2 << "/" << vn2 << " " 
                 << v3 << "/" << vt3 << "/" << vn3 << "\n";
        }

        file << "\n";

        vertexOffset += obj->mesh->vertices.size();
        texCoordOffset += obj->mesh->vertices.size();
        normalOffset += obj->mesh->vertices.size();
    }

    file.close();
    std::cout << "Successfully exported scene to: " << path << std::endl;
    return true;
}