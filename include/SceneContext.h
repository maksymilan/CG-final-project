#ifndef SCENE_CONTEXT_H
#define SCENE_CONTEXT_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"

// 几何体类型枚举
enum class GeometryType {
    None,
    Cube,
    Sphere,
    Cylinder,
    Cone,
    Plane,
    Prism,
    Frustum
};

struct SceneObject {
    std::string name;
    Mesh* mesh;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color;
    
    // 纹理相关
    std::string texturePath; 
    unsigned int textureId = 0;
    
    // 几何体信息，用于网格精度调整
    GeometryType geometryType = GeometryType::None;
    float param1 = 0.0f; // 半径、宽度等
    float param2 = 0.0f; // 高度、深度等
    float param3 = 0.0f; // 额外参数（如棱台的上底半径）
    int segments = 0;    // 精度参数

    // 动画相关
    bool isAnimated = false;
    std::vector<Mesh*> animationFrames;
    int currentFrame = 0;
    float animationSpeed = 1.0f;
    bool isPlaying = false;
    float animationTime = 0.0f;
    bool loopAnimation = true;
    float startDelay = 0.0f;
    float delayTimer = 0.0f;

    SceneObject(std::string n, Mesh* m) 
        : name(n), mesh(m), position(0.0f), rotation(0.0f), scale(1.0f), color(1.0f), texturePath("") {}
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