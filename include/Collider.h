#ifndef COLLIDER_H
#define COLLIDER_H

#include "Component.h"
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

// 基类 Collider，管理所有碰撞体
class Collider : public Component
{
public:
    static std::vector<Collider *> allColliders;

    bool isTrigger = false;

    Collider();
    Collider(const Collider &other);
    virtual ~Collider();

    // 获取世界坐标下的 AABB 包围盒（用于粗略碰撞检测）
    virtual void GetWorldAABB(glm::vec3 &outMin, glm::vec3 &outMax) = 0;

    // 预测在指定位置的 AABB（用于移动检测）
    virtual void GetAABBAtPosition(const glm::vec3 &pos, glm::vec3 &outMin, glm::vec3 &outMax) = 0;

    // 绘制 Gizmos 是 Component 的功能，这里仍保留为纯虚函数或由子类覆盖
    virtual void OnDrawGizmos(Shader &shader) override {}

    // 标识碰撞体类型
    enum class ColliderType
    {
        Box,
        Sphere,
        Mesh,
        Capsule
    };
    virtual ColliderType GetType() const = 0;
};

#endif
