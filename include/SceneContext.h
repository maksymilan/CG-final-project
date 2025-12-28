#ifndef SCENE_CONTEXT_H
#define SCENE_CONTEXT_H

#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"

// 场景中的物体结构
struct SceneObject {
    std::string name;
    Mesh* mesh;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color; // 简单材质颜色

    SceneObject(std::string n, Mesh* m) 
        : name(n), mesh(m), position(0.0f), rotation(0.0f), scale(1.0f), color(1.0f) {}
};

// SceneContext 类：管理场景中所有的物体
// 职责：[Part A] 负责场景图管理、物体增删改查
class SceneContext {
public:
    std::vector<SceneObject*> objects;
    SceneObject* selectedObject = nullptr; // 当前在编辑器中选中的物体

    SceneContext();
    ~SceneContext();

    void AddObject(SceneObject* obj);
    void DrawAll(Shader& shader);
};

#endif