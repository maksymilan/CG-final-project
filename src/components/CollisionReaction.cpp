#include "Component.h"
#include "SceneContext.h"
#include "imgui.h"
#include <iostream>
#include <glm/glm.hpp>

class CollisionReaction : public Component
{
public:
    glm::vec3 collisionColor = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 originalColor = glm::vec3(1.0f);
    float resetTime = 0.0f;

    void Start() override
    {
        if (owner)
        {
            originalColor = owner->color;
        }
    }

    void Update(float deltaTime) override
    {
        if (resetTime > 0.0f)
        {
            resetTime -= deltaTime;
            if (resetTime <= 0.0f && owner)
            {
                owner->color = originalColor;
            }
        }
    }

    void OnCollision(SceneObject *other) override
    {
        if (owner)
        {
            owner->color = collisionColor;
            resetTime = 0.1f; // Reset color after 0.1s of no collision
            // std::cout << "Collision detected with: " << (other ? other->name : "Unknown") << std::endl;
        }
    }

    void OnInspectorGUI() override
    {
        ImGui::ColorEdit3("Collision Color", (float *)&collisionColor);
    }

    Component *Clone() const override { return new CollisionReaction(*this); }

    std::string GetName() const override { return "Collision Reaction"; }
    std::string GetTypeName() const override { return "CollisionReaction"; }

    void Save(std::ostream &out) override
    {
        out << collisionColor.x << " " << collisionColor.y << " " << collisionColor.z << std::endl;
    }

    void Load(std::istream &in) override
    {
        in >> collisionColor.x >> collisionColor.y >> collisionColor.z;
    }
};

REGISTER_COMPONENT(CollisionReaction)