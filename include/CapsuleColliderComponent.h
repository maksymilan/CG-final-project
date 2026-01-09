#ifndef CAPSULE_COLLIDER_COMPONENT_H
#define CAPSULE_COLLIDER_COMPONENT_H

#include "Collider.h"
#include <glm/glm.hpp>
#include <vector>

class CapsuleColliderComponent : public Collider
{
public:
    glm::vec3 center = glm::vec3(0.0f);
    float radius = 0.5f;
    float height = 2.0f;
    int direction = 1; // 0: X-axis, 1: Y-axis, 2: Z-axis

    CapsuleColliderComponent();
    CapsuleColliderComponent(const CapsuleColliderComponent &other);
    virtual ~CapsuleColliderComponent();

    void Update(float deltaTime) override;
    Component *Clone() const override;
    std::string GetName() const override { return "Capsule Collider"; }

    void OnDrawGizmos(Shader &shader) override;

    // Get World Space AABB
    void GetWorldAABB(glm::vec3 &outMin, glm::vec3 &outMax) override;
    // Get AABB at specific position (ignoring owner's current position)
    void GetAABBAtPosition(const glm::vec3 &pos, glm::vec3 &outMin, glm::vec3 &outMax) override;

    // 实现基类虚函数
    ColliderType GetType() const override { return ColliderType::Capsule; }
    std::string GetTypeName() const override { return "CapsuleColliderComponent"; }

    void Save(std::ostream &out) override;
    void Load(std::istream &in) override;
    void OnInspectorGUI() override;
};

#endif
