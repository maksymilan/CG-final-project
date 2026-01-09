#ifndef MESH_COLLIDER_COMPONENT_H
#define MESH_COLLIDER_COMPONENT_H

#include "Collider.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <vector>

// MeshColliderComponent: 使用物体网格数据进行精确碰撞检测
class MeshColliderComponent : public Collider
{
public:
    // 是否是凸包（简化碰撞检测，暂未实现凸包生成算法，仅作为标志位）
    bool convex = false;

    // 如果为 null，这使用 owner->mesh
    Mesh *sharedMesh = nullptr;

    MeshColliderComponent();
    virtual ~MeshColliderComponent();

    void Start() override;
    void OnDrawGizmos(Shader &shader) override;

    // 获取世界空间 AABB
    void GetWorldAABB(glm::vec3 &outMin, glm::vec3 &outMax) override;
    void GetAABBAtPosition(const glm::vec3 &pos, glm::vec3 &outMin, glm::vec3 &outMax) override;

    // 核心功能：检测与盒体的碰撞
    bool CheckCollisionOriginal(class BoxColliderComponent *box);

    // 实现基类接口
    ColliderType GetType() const override { return ColliderType::Mesh; }
    std::string GetTypeName() const override { return "MeshColliderComponent"; }
    std::string GetName() const override { return "Mesh Collider"; }
    Component *Clone() const override;

    void Save(std::ostream &out) override;
    void Load(std::istream &in) override;
    void OnInspectorGUI() override;

private:
    // 缓存变换后的顶点，避免每帧重复计算（可选优化）
    // 或者只存储原始顶点的最小/最大值用于快速计算 AABB
    glm::vec3 minVertex;
    glm::vec3 maxVertex;
    bool boundsDirty = true;

    void RecalculateBounds();

    // 简单的三角形-Box相交测试
    bool IntersectTriangleBox(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &boxCenter, const glm::vec3 &boxHalfSize);
};

#endif
