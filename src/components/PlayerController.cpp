#include "Component.h"
#include "SceneContext.h"
#include "BoxColliderComponent.h"
#include "MeshColliderComponent.h"
#include "CapsuleColliderComponent.h"
#include "Collider.h"
#include "imgui.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class PlayerController : public Component
{
public:
    float moveSpeed = 5.0f;

    void Update(float deltaTime) override
    {
        if (!owner)
            return;

        // 获取当前上下文的窗口句柄
        GLFWwindow *window = glfwGetCurrentContext();
        if (!window)
            return;

        glm::vec3 movement(0.0f);

        // 检测 WASD 按键
        // 注意：这里使用的是世界坐标系移动
        // W: -Z (向前)
        // S: +Z (向后)
        // A: -X (向左)
        // D: +X (向右)

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            movement.z -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            movement.z += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            movement.x -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            movement.x += 1.0f;

        // 如果有输入，则更新位置
        if (glm::length(movement) > 0.0f)
        {
            movement = glm::normalize(movement);
            glm::vec3 velocity = movement * moveSpeed * deltaTime;

            // Collision Detection
            // Try to get CapsuleCollider first, then BoxCollider, then generic Collider
            Collider *myCollider = owner->GetComponent<CapsuleColliderComponent>();
            if (!myCollider)
                myCollider = owner->GetComponent<BoxColliderComponent>();
            if (!myCollider)
                myCollider = owner->GetComponent<Collider>();

            if (myCollider)
            {
                // Try moving in X
                if (velocity.x != 0.0f)
                {
                    glm::vec3 nextPos = owner->position;
                    nextPos.x += velocity.x;

                    bool collisionX = false;

                    // Check against all colliders
                    glm::vec3 minA, maxA;
                    myCollider->GetAABBAtPosition(nextPos, minA, maxA);

                    // [Fix] Shrink the checking volume vertically to avoid hitting the floor
                    // if the player is intersecting it (common in this top-down setup).
                    // We only check the "upper body" or "torso" area.
                    float checkHeightBottom = nextPos.y + 0.1f;
                    if (minA.y < checkHeightBottom)
                        minA.y = checkHeightBottom;

                    // Iterate over generic Colliders instead of just BoxColliderComponent::allColliders
                    for (auto other : Collider::allColliders)
                    {
                        if (other == myCollider)
                            continue;
                        if (other->owner == owner)
                            continue;
                        if (other->isTrigger)
                            continue; // Ignore triggers for blocking

                        // [Fix] Only interact with colliders in the SAME scene
                        if (other->owner->sceneContext != owner->sceneContext)
                            continue;

                        // [Fix] Explicitly ignore the Ground Plane to prevent getting stuck
                        if (other->owner && other->owner->name == "Ground Plane")
                            continue;

                        glm::vec3 minB, maxB;
                        other->GetWorldAABB(minB, maxB);

                        // Broad-phase AABB Check
                        if ((minA.x <= maxB.x && maxA.x >= minB.x) &&
                            (minA.y <= maxB.y && maxA.y >= minB.y) &&
                            (minA.z <= maxB.z && maxA.z >= minB.z))
                        {
                            // If MeshCollider, perform precise check
                            if (other->GetType() == Collider::ColliderType::Mesh)
                            {
                                MeshColliderComponent *meshCol = static_cast<MeshColliderComponent *>(other);

                                // Check if self is Box for precise Box-Mesh collision
                                if (myCollider->GetType() == Collider::ColliderType::Box)
                                {
                                    BoxColliderComponent *myBox = static_cast<BoxColliderComponent *>(myCollider);

                                    // Temporarily move owner to test position for precise check
                                    glm::vec3 originalPos = owner->position;
                                    owner->position = nextPos;

                                    bool hit = meshCol->CheckCollisionOriginal(myBox);

                                    owner->position = originalPos; // Restore

                                    if (!hit)
                                        continue; // AABB overlapped but precise mesh didn't
                                }
                                // TODO: Implement precise Capsule-Mesh collision
                                // For now, if it's not a box, we trust the AABB check (which is conservative)
                            }

                            // [New] Trigger Collision Events
                            if (owner)
                                owner->OnCollision(other->owner);
                            if (other->owner)
                                other->owner->OnCollision(owner);

                            // Only block if not a trigger
                            if (!other->isTrigger)
                            {
                                collisionX = true;
                                // Debug output
                                if (other->owner)
                                {
                                    std::cout << "Collision X blocked by: " << other->owner->name << std::endl;
                                }
                                break;
                            }
                        }
                    }

                    if (!collisionX)
                    {
                        owner->position.x += velocity.x;
                    }
                }

                // Try moving in Z
                if (velocity.z != 0.0f)
                {
                    glm::vec3 nextPos = owner->position;
                    nextPos.z += velocity.z;

                    bool collisionZ = false;

                    // Check against all colliders
                    glm::vec3 minA, maxA;
                    myCollider->GetAABBAtPosition(nextPos, minA, maxA);

                    // [Fix] Shrink vertically
                    float checkHeightBottom = nextPos.y + 0.1f;
                    if (minA.y < checkHeightBottom)
                        minA.y = checkHeightBottom;

                    for (auto other : Collider::allColliders)
                    {
                        if (other == myCollider)
                            continue;
                        if (other->owner == owner)
                            continue;

                        // [Fix] Only interact with colliders in the SAME scene
                        if (other->owner->sceneContext != owner->sceneContext)
                            continue;

                        // [Fix] Explicitly ignore the Ground Plane to prevent getting stuck
                        if (other->owner && other->owner->name == "Ground Plane")
                            continue;

                        glm::vec3 minB, maxB;
                        other->GetWorldAABB(minB, maxB);

                        if ((minA.x <= maxB.x && maxA.x >= minB.x) &&
                            (minA.y <= maxB.y && maxA.y >= minB.y) &&
                            (minA.z <= maxB.z && maxA.z >= minB.z))
                        {
                            // If MeshCollider, perform precise check
                            if (other->GetType() == Collider::ColliderType::Mesh)
                            {
                                MeshColliderComponent *meshCol = static_cast<MeshColliderComponent *>(other);

                                // Check if self is Box for precise Box-Mesh collision
                                if (myCollider->GetType() == Collider::ColliderType::Box)
                                {
                                    BoxColliderComponent *myBox = static_cast<BoxColliderComponent *>(myCollider);

                                    // Temporarily move owner to test position for precise check
                                    glm::vec3 originalPos = owner->position;
                                    owner->position = nextPos;

                                    bool hit = meshCol->CheckCollisionOriginal(myBox);

                                    owner->position = originalPos; // Restore

                                    if (!hit)
                                        continue;
                                }
                                // Capsule/Generic fallback to AABB check
                            }

                            // [New] Trigger Collision Events
                            if (owner)
                                owner->OnCollision(other->owner);
                            if (other->owner)
                                other->owner->OnCollision(owner);

                            // Only block if not a trigger
                            if (!other->isTrigger)
                            {
                                collisionZ = true;
                                // Debug output
                                if (other->owner)
                                {
                                    std::cout << "Collision Z blocked by: " << other->owner->name << std::endl;
                                }
                                break;
                            }
                        }
                    }

                    if (!collisionZ)
                    {
                        owner->position.z += velocity.z;
                    }
                }
            }
            else
            {
                // No collider attached, just move
                owner->position += velocity;
            }
        }
    }

    void OnInspectorGUI() override
    {
        ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f);
        ImGui::Text("Controls: W/A/S/D to move");
    }

    Component *Clone() const override { return new PlayerController(*this); }

    std::string GetName() const override { return "Player Controller"; }
    std::string GetTypeName() const override { return "PlayerController"; }

    void Save(std::ostream &out) override
    {
        out << moveSpeed << std::endl;
    }

    void Load(std::istream &in) override
    {
        in >> moveSpeed;
    }
};

REGISTER_COMPONENT(PlayerController)
