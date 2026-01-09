#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H

#include "Collider.h"
#include <glm/glm.hpp>
#include <vector>

class BoxColliderComponent : public Collider
{
public:
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 size = glm::vec3(1.0f);
    // bool isTrigger moved to Collider base class
    // static allColliders moved to Collider base class

    BoxColliderComponent();
    BoxColliderComponent(const BoxColliderComponent &other);
    virtual ~BoxColliderComponent();

    void Update(float deltaTime) override;
    Component *Clone() const override;
    std::string GetName() const override { return "Box Collider"; }

    void OnDrawGizmos(Shader &shader) override;

    // Get World Space AABB
    void GetWorldAABB(glm::vec3 &outMin, glm::vec3 &outMax) override;
    // Get AABB at specific position (ignoring owner's current position)
    void GetAABBAtPosition(const glm::vec3 &pos, glm::vec3 &outMin, glm::vec3 &outMax) override;

    // 具体类型的碰撞检测（保留）
    bool CheckCollision(BoxColliderComponent *other);

    // 实现基类虚函数
    ColliderType GetType() const override { return ColliderType::Box; }
    std::string GetTypeName() const override { return "BoxColliderComponent"; }

    void Save(std::ostream &out) override;
    void Load(std::istream &in) override;
    void OnInspectorGUI() override;
};

#endif
