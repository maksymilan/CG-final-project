#ifndef SCENE_CONTEXT_H
#define SCENE_CONTEXT_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"

struct SceneObject {
    std::string name;
    Mesh* mesh;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color; // 用作 Diffuse Color
    
    // --- [新增/修改] 材质属性 ---
    float shininess;       // 反光度 (例如 32.0f)
    float specularStrength; // 镜面反射强度 (0.0f - 1.0f)
    bool useTexture;       // 是否启用纹理的开关
    
    std::string texturePath;
    unsigned int textureId = 0;

    SceneObject(std::string n, Mesh* m)
        : name(n), mesh(m), position(0.0f), rotation(0.0f), scale(1.0f), color(1.0f),
          texturePath(""), shininess(32.0f), specularStrength(0.5f), useTexture(false) {}
};

class SceneContext {
public:
    std::vector<SceneObject*> objects;
    SceneObject* selectedObject = nullptr;

    SceneContext();
    ~SceneContext();

    void AddObject(SceneObject* obj);
    void DrawAll(Shader& shader);
};

#endif