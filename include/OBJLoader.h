#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include "Mesh.h"

// Forward declaration to avoid circular includes
class ModelLoader;
class SceneContext;

// OBJLoader class: Implements the required interface
// Uses existing ModelLoader internally to reuse implementation
class OBJLoader {
public:
    static Mesh* Load(const std::string& path);
    static std::vector<Mesh*> LoadSequence(const std::string& folderPath);
    static bool ExportMesh(const Mesh* mesh, const std::string& path);
    static bool ExportScene(const SceneContext* scene, const std::string& path);
};

#endif