#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <vector>
#include "Mesh.h"

class SceneContext;

class ModelLoader {
public:
    static Mesh* LoadMesh(const std::string& path);
    static bool ExportMesh(const Mesh* mesh, const std::string& path);
    static bool ExportScene(const SceneContext* scene, const std::string& path);
};

#endif