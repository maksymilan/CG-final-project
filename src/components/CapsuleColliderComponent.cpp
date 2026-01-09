#include "CapsuleColliderComponent.h"
#include "SceneContext.h"
#include "imgui.h"
#include "GeometryUtils.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

CapsuleColliderComponent::CapsuleColliderComponent() : Collider()
{
}

CapsuleColliderComponent::CapsuleColliderComponent(const CapsuleColliderComponent &other) : Collider(other)
{
    center = other.center;
    radius = other.radius;
    height = other.height;
    direction = other.direction;
}

CapsuleColliderComponent::~CapsuleColliderComponent()
{
}

void CapsuleColliderComponent::Update(float deltaTime)
{
}

Component *CapsuleColliderComponent::Clone() const { return new CapsuleColliderComponent(*this); }

void CapsuleColliderComponent::OnDrawGizmos(Shader &shader)
{
    if (!owner)
        return;

    static Mesh *debugSphere = nullptr;
    static Mesh *debugCylinder = nullptr;
    if (!debugSphere)
        debugSphere = GeometryUtils::CreateSphere(16, 16);
    if (!debugCylinder)
        debugCylinder = GeometryUtils::CreateCylinder(1.0f, 1.0f, 16); // Unit cylinder

    // Safe checks for geometry
    float h = std::max(0.0f, height);
    float r = std::max(0.0f, radius);
    if (h < 2 * r)
        h = 2 * r; // Clamp
    float cylinderHeight = h - 2 * r;

    // Base transform
    glm::mat4 baseModel = glm::mat4(1.0f);
    baseModel = glm::translate(baseModel, owner->position);
    baseModel = glm::rotate(baseModel, glm::radians(owner->rotation.x), glm::vec3(1, 0, 0));
    baseModel = glm::rotate(baseModel, glm::radians(owner->rotation.y), glm::vec3(0, 1, 0));
    baseModel = glm::rotate(baseModel, glm::radians(owner->rotation.z), glm::vec3(0, 0, 1));
    baseModel = glm::scale(baseModel, owner->scale);
    baseModel = glm::translate(baseModel, center);

    // Rotation based on direction
    // Cylinder default is Y-aligned usually? GeometryGenerator implementation dependent.
    // Assuming Cylinder is Y-aligned.
    glm::mat4 dirRot = glm::mat4(1.0f);
    if (direction == 0) // X-axis
        dirRot = glm::rotate(dirRot, glm::radians(90.0f), glm::vec3(0, 0, -1));
    else if (direction == 2) // Z-axis
        dirRot = glm::rotate(dirRot, glm::radians(90.0f), glm::vec3(1, 0, 0));

    // Draw Cylinder Body
    if (cylinderHeight > 0.0001f)
    {
        glm::mat4 cylModel = baseModel * dirRot;
        cylModel = glm::scale(cylModel, glm::vec3(r, cylinderHeight, r));

        shader.setMat4("model", cylModel);
        shader.setVec3("albedo", glm::vec3(0.0f, 1.0f, 0.0f));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (debugCylinder)
            debugCylinder->Draw(shader);
    }

    // Draw Top/Bottom Spheres
    float offset = cylinderHeight * 0.5f;
    glm::vec3 offsetVec(0, offset, 0);

    // Upper Sphere
    glm::mat4 sphereModel1 = baseModel * dirRot;
    sphereModel1 = glm::translate(sphereModel1, offsetVec);
    sphereModel1 = glm::scale(sphereModel1, glm::vec3(r));
    shader.setMat4("model", sphereModel1);
    if (debugSphere)
        debugSphere->Draw(shader);

    // Lower Sphere
    glm::mat4 sphereModel2 = baseModel * dirRot;
    sphereModel2 = glm::translate(sphereModel2, -offsetVec);
    sphereModel2 = glm::scale(sphereModel2, glm::vec3(r));
    shader.setMat4("model", sphereModel2);
    if (debugSphere)
        debugSphere->Draw(shader);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CapsuleColliderComponent::GetWorldAABB(glm::vec3 &outMin, glm::vec3 &outMax)
{
    if (!owner)
        return;
    GetAABBAtPosition(owner->position, outMin, outMax);
}

void CapsuleColliderComponent::GetAABBAtPosition(const glm::vec3 &pos, glm::vec3 &outMin, glm::vec3 &outMax)
{
    if (!owner)
        return;

    float r = radius;
    float h = height;
    if (h < 2 * r)
        h = 2 * r;
    float segHalf = (h / 2.0f) - r;

    glm::vec3 p0_local(0), p1_local(0);
    // Assuming Direction Y
    glm::vec3 axis(0, 1, 0);
    if (direction == 0)
        axis = glm::vec3(1, 0, 0);
    if (direction == 2)
        axis = glm::vec3(0, 0, 1);

    p0_local = center + axis * segHalf;
    p1_local = center - axis * segHalf;

    // Transform
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(owner->rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(owner->rotation.z), glm::vec3(0, 0, 1));
    // Apply Scale
    // For radius, we need to take scale into account.
    // worldRadius ~ max scale on perpendicular axes * r
    // For segment points, we just multiply by scale matrix.
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), owner->scale);

    // World Points
    glm::vec3 w0 = glm::vec3(model * scaleMat * glm::vec4(p0_local, 1.0f));
    glm::vec3 w1 = glm::vec3(model * scaleMat * glm::vec4(p1_local, 1.0f));

    // World Radius approximation
    // We take the max scale for safety
    float maxScale = std::max({owner->scale.x, owner->scale.y, owner->scale.z});
    float worldRadius = r * maxScale;

    glm::vec3 minP = glm::min(w0, w1);
    glm::vec3 maxP = glm::max(w0, w1);

    outMin = minP - glm::vec3(worldRadius);
    outMax = maxP + glm::vec3(worldRadius);
}

void CapsuleColliderComponent::Save(std::ostream &out)
{
    out << center.x << " " << center.y << " " << center.z << std::endl;
    out << radius << " " << height << " " << direction << std::endl;
    out << isTrigger << std::endl;
}

void CapsuleColliderComponent::Load(std::istream &in)
{
    in >> center.x >> center.y >> center.z;
    in >> radius >> height >> direction;
    in >> isTrigger;
}

void CapsuleColliderComponent::OnInspectorGUI()
{
    ImGui::DragFloat3("Center", (float *)&center, 0.1f);
    ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("Height", &height, 0.1f, 0.0f, 100.0f);
    const char *dirs[] = {"X-Axis", "Y-Axis", "Z-Axis"};
    ImGui::Combo("Direction", &direction, dirs, 3);
    ImGui::Checkbox("Is Trigger", &isTrigger);
}

REGISTER_COMPONENT(CapsuleColliderComponent)
