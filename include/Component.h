#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <iostream>
#include "imgui.h"

struct SceneObject;
class Component;

// Registry system
class ComponentRegistry
{
public:
    using Creator = std::function<Component *()>;

    static ComponentRegistry &Instance()
    {
        static ComponentRegistry instance;
        return instance;
    }

    void Register(const std::string &name, Creator creator)
    {
        creators[name] = creator;
    }

    const std::map<std::string, Creator> &GetCreators() const
    {
        return creators;
    }

private:
    std::map<std::string, Creator> creators;
};

template <typename T>
struct ComponentRegistrar
{
    ComponentRegistrar(const std::string &name)
    {
        ComponentRegistry::Instance().Register(name, []() -> Component *
                                               { return new T(); });
    }
};

// Macro to register components
#define REGISTER_COMPONENT(TYPE) \
    static ComponentRegistrar<TYPE> global_registrar_##TYPE(#TYPE);

class Shader; // Forward declaration

class Component
{
public:
    SceneObject *owner = nullptr;
    bool enabled = true;

    virtual ~Component() {}

    // Called when the component is added
    virtual void Start() {}

    // Called every frame
    virtual void Update(float deltaTime) {}

    // Called when collision occurs
    virtual void OnCollision(SceneObject *other) {}

    // Called to render custom UI in the inspector
    virtual void OnInspectorGUI() {}

    // [New] Called to render debug info (like colliders)
    virtual void OnDrawGizmos(Shader &shader) {}

    // Deep copy the component
    virtual Component *Clone() const = 0;

    // Component name for UI
    virtual std::string GetName() const = 0;

    // Component type name for Registry/Serialization
    virtual std::string GetTypeName() const = 0;

    // Serialization
    virtual void Save(std::ostream &out) {}
    virtual void Load(std::istream &in) {}
};

#endif
