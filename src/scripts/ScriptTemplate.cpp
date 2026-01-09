#include "Component.h"
#include "SceneContext.h"
#include "imgui.h"
#include <iostream>
#include <glm/glm.hpp>

// Template for creating new scripts
// 1. Rename 'ScriptTemplate' to your class name.
// 2. Implement the methods.
// 3. Register the component at the bottom.

class ScriptTemplate : public Component
{
public:
    // Define public variables here (will be editable in Inspector if you add UI)
    float exampleValue = 1.0f;

    // Called when the component is added to an object
    void Start() override
    {
        // Initialization code
    }

    // Called every frame
    void Update(float deltaTime) override
    {
        if (!owner)
            return;

        // Game logic here
        // Example: owner->rotation.y += exampleValue * deltaTime;
    }

    // Called when collision occurs (if Collider is attached)
    void OnCollision(SceneObject *other) override
    {
        // Collision response
    }

    // Define inspector UI
    void OnInspectorGUI() override
    {
        ImGui::Text("Script Template");
        ImGui::DragFloat("Example Value", &exampleValue, 0.1f);
    }

    // Deep copy implementation
    Component *Clone() const override
    {
        ScriptTemplate *newC = new ScriptTemplate();
        // Copy variables
        newC->exampleValue = exampleValue;
        return newC;
    }

    // Runtime type identification
    std::string GetName() const override { return "Script Template"; }
    std::string GetTypeName() const override { return "ScriptTemplate"; }

    // Serialization
    void Save(std::ostream &out) override
    {
        out << exampleValue << std::endl;
    }

    void Load(std::istream &in) override
    {
        in >> exampleValue;
    }
};

// Register component for reflection/loading
REGISTER_COMPONENT(ScriptTemplate)
