#include "OBJLoader.h"
#include "ModelLoader.h"
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <algorithm>

// Uses existing ModelLoader implementation internally
Mesh* OBJLoader::Load(const std::string& path) {
    return ModelLoader::LoadMesh(path);
}

// Implements the required LoadSequence method
std::vector<Mesh*> OBJLoader::LoadSequence(const std::string& folderPath) {
    std::vector<Mesh*> meshes;
    std::vector<std::string> objFiles;

    try {
        // Get all .obj files in the folder
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".obj") {
                objFiles.push_back(entry.path().string());
            }
        }

        // Sort the files by name
        std::sort(objFiles.begin(), objFiles.end());

        // Load each file
        for (const auto& filePath : objFiles) {
            Mesh* mesh = ModelLoader::LoadMesh(filePath);
            if (mesh) {
                meshes.push_back(mesh);
            } else {
                std::cerr << "Failed to load mesh: " << filePath << std::endl;
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error loading sequence: " << e.what() << std::endl;
    }

    return meshes;
}

// Uses existing ModelLoader implementation internally
bool OBJLoader::ExportMesh(const Mesh* mesh, const std::string& path) {
    return ModelLoader::ExportMesh(mesh, path);
}

// Uses existing ModelLoader implementation internally
bool OBJLoader::ExportScene(const SceneContext* scene, const std::string& path) {
    return ModelLoader::ExportScene(scene, path);
}