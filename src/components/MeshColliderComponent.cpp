#include "MeshColliderComponent.h"
#include "SceneContext.h"
#include "BoxColliderComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <algorithm>

REGISTER_COMPONENT(MeshColliderComponent)

MeshColliderComponent::MeshColliderComponent()
{
}

MeshColliderComponent::~MeshColliderComponent()
{
}

void MeshColliderComponent::Start()
{
    if (!sharedMesh && owner && owner->mesh)
    {
        sharedMesh = owner->mesh;
        RecalculateBounds();
    }
}

void MeshColliderComponent::RecalculateBounds()
{
    if (!sharedMesh || sharedMesh->vertices.empty())
    {
        minVertex = glm::vec3(-0.5f);
        maxVertex = glm::vec3(0.5f);
        return;
    }

    minVertex = glm::vec3(std::numeric_limits<float>::max());
    maxVertex = glm::vec3(std::numeric_limits<float>::lowest());

    for (const auto &v : sharedMesh->vertices)
    {
        minVertex = glm::min(minVertex, v.Position);
        maxVertex = glm::max(maxVertex, v.Position);
    }
    boundsDirty = false;
}

void MeshColliderComponent::GetWorldAABB(glm::vec3 &outMin, glm::vec3 &outMax)
{
    if (boundsDirty)
        RecalculateBounds();

    if (!owner)
    {
        outMin = minVertex;
        outMax = maxVertex;
        return;
    }

    // 获取对象变换矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, owner->position);
    model = glm::rotate(model, glm::radians(owner->rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, owner->scale);

    // 变换 AABB 的 8 个角点来计算新的 AABB
    glm::vec3 corners[8];
    corners[0] = glm::vec3(minVertex.x, minVertex.y, minVertex.z);
    corners[1] = glm::vec3(minVertex.x, minVertex.y, maxVertex.z);
    corners[2] = glm::vec3(minVertex.x, maxVertex.y, minVertex.z);
    corners[3] = glm::vec3(minVertex.x, maxVertex.y, maxVertex.z);
    corners[4] = glm::vec3(maxVertex.x, minVertex.y, minVertex.z);
    corners[5] = glm::vec3(maxVertex.x, minVertex.y, maxVertex.z);
    corners[6] = glm::vec3(maxVertex.x, maxVertex.y, minVertex.z);
    corners[7] = glm::vec3(maxVertex.x, maxVertex.y, maxVertex.z);

    outMin = glm::vec3(std::numeric_limits<float>::max());
    outMax = glm::vec3(std::numeric_limits<float>::lowest());

    for (int i = 0; i < 8; i++)
    {
        glm::vec3 transPos = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        outMin = glm::min(outMin, transPos);
        outMax = glm::max(outMax, transPos);
    }
}

void MeshColliderComponent::GetAABBAtPosition(const glm::vec3 &pos, glm::vec3 &outMin, glm::vec3 &outMax)
{
    // 简化的实现，仅基于当前 Scale 和 Rotation，改变位置
    // 在实际物理引擎中，通常会分离 Transform 和 Collider
    glm::vec3 oldPos = owner->position;
    owner->position = pos;
    GetWorldAABB(outMin, outMax);
    owner->position = oldPos;
}

// 分离轴定理 (SAT) 辅助函数
bool CheckAxis(const glm::vec3 &axis, const std::vector<glm::vec3> &verts, const glm::vec3 &boxMin, const glm::vec3 &boxMax)
{
    // 投影三角形
    float minTri = std::numeric_limits<float>::max();
    float maxTri = -std::numeric_limits<float>::max();
    for (const auto &v : verts)
    {
        float p = glm::dot(v, axis);
        minTri = std::min(minTri, p);
        maxTri = std::max(maxTri, p);
    }

    // 投影 AABB
    // AABB 的中心和半长轴
    glm::vec3 boxCenter = (boxMin + boxMax) * 0.5f;
    glm::vec3 boxHalf = (boxMax - boxMin) * 0.5f;

    // AABB 在任意轴上的投影半径 r = Sum(|dot(axis, boxAxis_i)| * extent_i)
    // 这里 boxAxis 是世界坐标轴 (1,0,0) 等
    float r = boxHalf.x * std::abs(glm::dot(glm::vec3(1, 0, 0), axis)) +
              boxHalf.y * std::abs(glm::dot(glm::vec3(0, 1, 0), axis)) +
              boxHalf.z * std::abs(glm::dot(glm::vec3(0, 0, 1), axis));

    float openCenter = glm::dot(boxCenter, axis);
    float minBox = openCenter - r;
    float maxBox = openCenter + r;

    return !(minTri > maxBox || maxTri < minBox);
}

// 检测三角形与 AABB 是否相交
bool MeshColliderComponent::IntersectTriangleBox(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &boxCenter, const glm::vec3 &boxHalfSize)
{
    // 将 Box 视为世界坐标下的 AABB，直接使用 SAT
    glm::vec3 boxMin = boxCenter - boxHalfSize;
    glm::vec3 boxMax = boxCenter + boxHalfSize;
    std::vector<glm::vec3> tri = {v0, v1, v2};

    // 1. Box 的面法线 (3个: 1,0,0 / 0,1,0 / 0,0,1)
    // 即检查三角形 AABB 与 Box AABB 是否相交
    glm::vec3 triMin = glm::min(v0, glm::min(v1, v2));
    glm::vec3 triMax = glm::max(v0, glm::max(v1, v2));
    if (triMax.x < boxMin.x || triMin.x > boxMax.x)
        return false;
    if (triMax.y < boxMin.y || triMin.y > boxMax.y)
        return false;
    if (triMax.z < boxMin.z || triMin.z > boxMax.z)
        return false;

    // 2. 三角形面法线 (1个)
    glm::vec3 triNorm = glm::cross(v1 - v0, v2 - v0);
    if (!CheckAxis(triNorm, tri, boxMin, boxMax))
        return false;

    // 3. 边叉积 (9个 = 3 tri edges * 3 box edges)
    glm::vec3 triEdges[3] = {v1 - v0, v2 - v1, v0 - v2};
    glm::vec3 boxAxes[3] = {glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)};

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            glm::vec3 axis = glm::cross(triEdges[i], boxAxes[j]);
            // 如果叉积接近0（平行），忽略该轴
            if (glm::length(axis) < 0.001f)
                continue;
            if (!CheckAxis(axis, tri, boxMin, boxMax))
                return false;
        }
    }

    return true;
}

bool MeshColliderComponent::CheckCollisionOriginal(BoxColliderComponent *box)
{
    if (!sharedMesh || !owner || !box || !box->owner)
        return false;

    // 1. 宽阶段 (Broad Phase)：AABB 检测
    glm::vec3 myMin, myMax;
    GetWorldAABB(myMin, myMax);

    glm::vec3 otherMin, otherMax;
    box->GetWorldAABB(otherMin, otherMax); // 注意：这里需要确保 Box 的 WorldAABB 也被正确计算

    bool aabbOverlap = (myMax.x >= otherMin.x && myMin.x <= otherMax.x) &&
                       (myMax.y >= otherMin.y && myMin.y <= otherMax.y) &&
                       (myMax.z >= otherMin.z && myMin.z <= otherMax.z);

    if (!aabbOverlap)
        return false;

    // 如果是凸包，可以使用 GJK 算法等高效算法（此处省略，按照 Unity 非 Convex 处理，即 Triangle Soup）

    // 2. 窄阶段 (Narrow Phase)：三角形遍历
    // 准备变换矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, owner->position);
    model = glm::rotate(model, glm::radians(owner->rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, owner->scale);

    // Box 信息 (World Space)
    glm::vec3 boxWorldCenter = (otherMin + otherMax) * 0.5f;
    glm::vec3 boxHalfSize = (otherMax - otherMin) * 0.5f;

    // 遍历所有三角形
    // 优化提示：实际项目中应该使用 Octree 或 BVH 来只检测附近的三角形
    const auto &indices = sharedMesh->indices;
    const auto &vertices = sharedMesh->vertices;

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        // 获取三角形顶点 (Local Space)
        glm::vec3 v0_local = vertices[indices[i]].Position;
        glm::vec3 v1_local = vertices[indices[i + 1]].Position;
        glm::vec3 v2_local = vertices[indices[i + 2]].Position;

        // 转换到 World Space
        glm::vec3 v0 = glm::vec3(model * glm::vec4(v0_local, 1.0f));
        glm::vec3 v1 = glm::vec3(model * glm::vec4(v1_local, 1.0f));
        glm::vec3 v2 = glm::vec3(model * glm::vec4(v2_local, 1.0f));

        if (IntersectTriangleBox(v0, v1, v2, boxWorldCenter, boxHalfSize))
        {
            return true;
        }
    }

    return false;
}

void MeshColliderComponent::OnDrawGizmos(Shader &shader)
{
    // 如果想要 Debug 绘制，可以画出 AABB 线框
    // 或者高亮发生碰撞的三角形
}

Component *MeshColliderComponent::Clone() const
{
    MeshColliderComponent *newC = new MeshColliderComponent();
    newC->sharedMesh = sharedMesh;
    newC->convex = convex;
    newC->isTrigger = isTrigger;
    newC->enabled = enabled;
    return newC;
}

void MeshColliderComponent::Save(std::ostream &out)
{
    // 序列化逻辑
}

void MeshColliderComponent::Load(std::istream &in)
{
    // 反序列化逻辑
}

void MeshColliderComponent::OnInspectorGUI()
{
    ImGui::Text("Mesh Collider");
    ImGui::Checkbox("Convex", &convex);
    ImGui::Checkbox("Is Trigger", &isTrigger);
    // 这里可以添加 Mesh 选择器
}
