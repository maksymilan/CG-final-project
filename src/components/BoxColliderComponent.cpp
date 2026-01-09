#include "BoxColliderComponent.h"
#include "SceneContext.h"
#include "imgui.h"
#include "GeometryUtils.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

// Initialize static member (moved to Collider base class)
// std::vector<BoxColliderComponent *> BoxColliderComponent::allColliders;

BoxColliderComponent::BoxColliderComponent() : Collider()
{
    // allColliders handling is done in base Collider constructor
}

BoxColliderComponent::BoxColliderComponent(const BoxColliderComponent &other) : Collider(other)
{
    center = other.center;
    size = other.size;
    // allColliders handling is done in base Collider copy constructor
}

BoxColliderComponent::~BoxColliderComponent()
{
    // allColliders handling is done in base Collider destructor
}

void BoxColliderComponent::OnDrawGizmos(Shader &shader)
{
    if (!owner)
        return;

    static Mesh *debugMesh = nullptr;
    if (!debugMesh)
        debugMesh = GeometryUtils::CreateCube();

    // Calculate transformation matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, owner->position);
    model = glm::rotate(model, glm::radians(owner->rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, owner->scale);

    // Add collider offset and size
    model = glm::translate(model, center);
    model = glm::scale(model, size);

    shader.setMat4("model", model);
    shader.setVec3("albedo", glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setInt("useTexture", 0);
    shader.setFloat("metallic", 0.0f);
    shader.setFloat("roughness", 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    debugMesh->Draw(shader);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BoxColliderComponent::GetWorldAABB(glm::vec3 &outMin, glm::vec3 &outMax)
{
    if (!owner)
        return;
    GetAABBAtPosition(owner->position, outMin, outMax);
}

void BoxColliderComponent::GetAABBAtPosition(const glm::vec3 &pos, glm::vec3 &outMin, glm::vec3 &outMax)
{
    if (!owner)
        return;

    // Calculate transformation matrix using custom position but owner's rotation/scale
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(owner->rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, owner->scale);

    // Local corners
    glm::vec3 halfSize = size * 0.5f;
    glm::vec3 corners[8];
    corners[0] = center + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z);
    corners[1] = center + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z);
    corners[2] = center + glm::vec3(halfSize.x, halfSize.y, -halfSize.z);
    corners[3] = center + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z);
    corners[4] = center + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z);
    corners[5] = center + glm::vec3(halfSize.x, -halfSize.y, halfSize.z);
    corners[6] = center + glm::vec3(halfSize.x, halfSize.y, halfSize.z);
    corners[7] = center + glm::vec3(-halfSize.x, halfSize.y, halfSize.z);

    // Transform corners and find min/max
    outMin = glm::vec3(std::numeric_limits<float>::max());
    outMax = glm::vec3(std::numeric_limits<float>::lowest());

    for (int i = 0; i < 8; i++)
    {
        glm::vec3 worldPos = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        outMin = glm::min(outMin, worldPos);
        outMax = glm::max(outMax, worldPos);
    }
}

bool BoxColliderComponent::CheckCollision(BoxColliderComponent *other)
{
    glm::vec3 minA, maxA;
    GetWorldAABB(minA, maxA);

    glm::vec3 minB, maxB;
    other->GetWorldAABB(minB, maxB);

    return (minA.x <= maxB.x && maxA.x >= minB.x) &&
           (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}

void BoxColliderComponent::Update(float deltaTime)
{
    // Deprecated self-check logic, collision is usually handled by a system or controller
    // But we keep it for backward compatibility if needed, iterating base allColliders
    // However, CheckCollision(BoxColliderComponent*) only accepts BoxCollider.
    // So we'll skip this logic or update it to use casting.
    // For now, let's just leave it empty or minimal as PlayerController handles the main logic.
}

void BoxColliderComponent::OnInspectorGUI()
{
    ImGui::DragFloat3("Center", (float *)&center, 0.1f);
    ImGui::DragFloat3("Size", (float *)&size, 0.1f);
    ImGui::Checkbox("Is Trigger", &isTrigger);
}

Component *BoxColliderComponent::Clone() const { return new BoxColliderComponent(*this); }

void BoxColliderComponent::Save(std::ostream &out)
{
    out << center.x << " " << center.y << " " << center.z << std::endl;
    out << size.x << " " << size.y << " " << size.z << std::endl;
    out << isTrigger << std::endl;
}

void BoxColliderComponent::Load(std::istream &in)
{
    in >> center.x >> center.y >> center.z;
    in >> size.x >> size.y >> size.z;
    in >> isTrigger;
}

REGISTER_COMPONENT(BoxColliderComponent)
