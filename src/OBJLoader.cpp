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
        // Validate directory
        if (!std::filesystem::exists(folderPath)) {
            std::cerr << "Error: Directory does not exist: " << folderPath << std::endl;
            return meshes;
        }

        if (!std::filesystem::is_directory(folderPath)) {
            std::cerr << "Error: Path is not a directory: " << folderPath << std::endl;
            return meshes;
        }

        std::cout << "Loading animation sequence from: " << folderPath << std::endl;

        // Get all .obj files in the folder
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".obj") {
                objFiles.push_back(entry.path().string());
            }
        }

        // Sort the files by name
        std::sort(objFiles.begin(), objFiles.end());

        // Check if any OBJ files found
        if (objFiles.empty()) {
            std::cerr << "Error: No .obj files found in directory: " << folderPath << std::endl;
            return meshes;
        }

        std::cout << "Found " << objFiles.size() << " .obj files" << std::endl;

        // Load each file
        int successCount = 0;
        for (size_t i = 0; i < objFiles.size(); i++) {
            const auto& filePath = objFiles[i];
            std::cout << "Loading frame " << i + 1 << "/" << objFiles.size() << ": " << filePath << std::endl;
            
            Mesh* mesh = ModelLoader::LoadMesh(filePath);
            if (mesh) {
                meshes.push_back(mesh);
                successCount++;
            } else {
                std::cerr << "Failed to load mesh: " << filePath << std::endl;
            }
        }

        std::cout << "Animation sequence loaded: " << successCount << "/" << objFiles.size() << " frames loaded successfully" << std::endl;

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error loading sequence: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading sequence: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error loading sequence" << std::endl;
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