#ifndef SCENE_CONTEXT_H
#define SCENE_CONTEXT_H

#include <vector>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Component.h"

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

// Forward declaration
class Camera;

struct SceneObject
{
    std::string name;
    Mesh *mesh;
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

    // PBR Material properties
    float roughness = 0.5f;
    float metallic = 0.0f;

    // [新增] 模型路径，用于保存/加载
    std::string meshPath;

    // Components
    std::vector<Component *> components;

    // Context reference
    class SceneContext *sceneContext = nullptr;

    SceneObject(std::string n, Mesh *m)
        : name(n), mesh(m), position(0.0f), rotation(0.0f), scale(1.0f), color(1.0f), texturePath(""), meshPath("") {}

    SceneObject *Clone()
    {
        SceneObject *newObj = new SceneObject(name, mesh);
        newObj->position = position;
        newObj->rotation = rotation;
        newObj->scale = scale;
        newObj->color = color;
        newObj->roughness = roughness;
        newObj->metallic = metallic;
        newObj->texturePath = texturePath;
        newObj->textureId = textureId;
        newObj->meshPath = meshPath;
        
        // Clone HEAD properties
        newObj->geometryType = geometryType;
        newObj->param1 = param1;
        newObj->param2 = param2;
        newObj->param3 = param3;
        newObj->segments = segments;
        newObj->isAnimated = isAnimated;
        newObj->animationFrames = animationFrames;
        newObj->currentFrame = currentFrame;
        newObj->animationSpeed = animationSpeed;
        newObj->isPlaying = isPlaying;
        newObj->loopAnimation = loopAnimation;
        newObj->startDelay = startDelay;

        for (auto c : components)
        {
            Component *newC = c->Clone();
            newObj->AddComponent(newC);
        }
        return newObj;
    }

    ~SceneObject()
    {
        for (auto c : components)
            delete c;
        components.clear();
    }

    void Update(float deltaTime)
    {
        for (auto c : components)
        {
            if (c->enabled)
                c->Update(deltaTime);
        }
    }

    // [New] Render gizmos
    void DrawGizmos(Shader &shader)
    {
        for (auto c : components)
        {
            if (c->enabled)
                c->OnDrawGizmos(shader);
        }
    }

    void OnCollision(SceneObject *other)
    {
        for (auto c : components)
        {
            if (c->enabled)
                c->OnCollision(other);
        }
    }

    void AddComponent(Component *c)
    {
        c->owner = this;
        components.push_back(c);
        c->Start();
    }

    template <typename T>
    T *AddComponent()
    {
        T *c = new T();
        c->owner = this;
        components.push_back(c);
        c->Start();
        return c;
    }

    template <typename T>
    T *GetComponent()
    {
        for (auto c : components)
        {
            if (dynamic_cast<T *>(c))
                return static_cast<T *>(c);
        }
        return nullptr;
    }

    void RemoveComponent(Component *c)
    {
        auto it = std::find(components.begin(), components.end(), c);
        if (it != components.end())
        {
            delete *it;
            components.erase(it);
        }
    }
};

class SceneContext
{
public:
    std::vector<SceneObject *> objects;
    SceneObject *selectedObject = nullptr;
    Camera *mainCamera = nullptr;

    SceneContext();
    ~SceneContext();

    SceneContext *Clone()
    {
        SceneContext *newScene = new SceneContext();
        for (auto obj : objects)
        {
            SceneObject *newObj = obj->Clone();
            newScene->AddObject(newObj);
        }
        return newScene;
    }

    void AddObject(SceneObject *obj);
    void Update(float deltaTime);
    void DrawAll(Shader &shader);
    void DrawGizmos(Shader &shader);

    void SaveScene(const std::string &filename);
    void LoadScene(const std::string &filename);
};

#endif
