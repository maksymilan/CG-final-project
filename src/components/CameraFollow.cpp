#include "Component.h"
#include "SceneContext.h"
#include "Camera.h"
#include "imgui.h"
#include <glm/glm.hpp>
#include <iostream>

class CameraFollow : public Component
{
public:
    glm::vec3 offset = glm::vec3(0.0f, 5.0f, 10.0f);
    float smoothFactor = 0.1f; // 0.0 to 1.0 (Percentage)
    bool lookAtTarget = true;

    void Update(float deltaTime) override
    {
        if (!owner || !owner->sceneContext || !owner->sceneContext->mainCamera)
            return;

        // Safety clamp for smoothFactor to avoid NaN in pow if loaded from old Save file
        if (smoothFactor < 0.0f)
            smoothFactor = 0.0f;
        if (smoothFactor > 1.0f)
            smoothFactor = 1.0f;

        Camera *cam = owner->sceneContext->mainCamera;

        // Target position
        glm::vec3 targetPos = owner->position + offset;

        // Smooth follow with percentage
        float t = 1.0f;
        if (smoothFactor < 0.999f)
        {
            t = 1.0f - std::pow(1.0f - smoothFactor, deltaTime * 60.0f);
        }

        // Clamp for safety
        if (t > 1.0f)
            t = 1.0f;
        if (t < 0.0f)
            t = 0.0f;

        cam->Position += (targetPos - cam->Position) * t;

        if (lookAtTarget)
        {
            cam->LookAt(owner->position);
        }
    }

    void OnInspectorGUI() override
    {
        ImGui::Text("Camera Follow");
        ImGui::DragFloat3("Offset", &offset.x, 0.1f);
        ImGui::SliderFloat("Smooth Factor", &smoothFactor, 0.01f, 1.0f, "%.2f");
        ImGui::Checkbox("Look At Target", &lookAtTarget);
    }

    Component *Clone() const override
    {
        CameraFollow *newC = new CameraFollow();
        newC->offset = offset;
        newC->smoothFactor = smoothFactor;
        newC->lookAtTarget = lookAtTarget;
        return newC;
    }

    std::string GetName() const override { return "Camera Follow"; }
    std::string GetTypeName() const override { return "CameraFollow"; }

    void Save(std::ostream &out) override
    {
        out << offset.x << " " << offset.y << " " << offset.z << std::endl;
        out << smoothFactor << std::endl;
        out << lookAtTarget << std::endl;
    }

    void Load(std::istream &in) override
    {
        in >> offset.x >> offset.y >> offset.z;
        in >> smoothFactor;
        in >> lookAtTarget;
    }
};

REGISTER_COMPONENT(CameraFollow)
