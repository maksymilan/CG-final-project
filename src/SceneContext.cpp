#include "SceneContext.h"
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "ModelLoader.h"
#include "GeometryUtils.h"

SceneContext::SceneContext() {}

SceneContext::~SceneContext()
{
    for (auto obj : objects)
    {
        // delete obj->mesh; // Mesh is shared resource, do not delete here
        delete obj;
    }
    objects.clear();
}

void SceneContext::AddObject(SceneObject *obj)
{
    obj->sceneContext = this;
    objects.push_back(obj);
}

void SceneContext::Update(float deltaTime)
{
    for (auto obj : objects)
    {
        obj->Update(deltaTime);
    }
}

void SceneContext::DrawGizmos(Shader &shader)
{
    for (auto obj : objects)
    {
        obj->DrawGizmos(shader);
    }
}

void SceneContext::DrawAll(Shader &shader)
{
    for (auto obj : objects)
    {
        // 计算 Model 矩阵 [Part A 核心逻辑]
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obj->position);
        model = glm::rotate(model, glm::radians(obj->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(obj->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(obj->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, obj->scale);

        // 设置 Shader Uniforms
        shader.setMat4("model", model);
        shader.setVec3("albedo", obj->color);
        shader.setFloat("roughness", obj->roughness);
        shader.setFloat("metallic", obj->metallic);

        // 绘制
        if (obj->mesh)
        {
            obj->mesh->Draw(shader);
        }
    }
}

void SceneContext::SaveScene(const std::string &filename)
{
    std::ofstream out(filename);
    if (!out.is_open())
    {
        std::cerr << "Failed to open file for saving: " << filename << std::endl;
        return;
    }

    out << "SCENE_v1" << std::endl;
    out << objects.size() << std::endl;

    for (auto obj : objects)
    {
        out << "OBJECT" << std::endl;
        out << std::quoted(obj->name) << std::endl;
        // Use a placeholder if meshPath is empty
        out << std::quoted(obj->meshPath.empty() ? "internal:cube" : obj->meshPath) << std::endl;

        out << obj->position.x << " " << obj->position.y << " " << obj->position.z << std::endl;
        out << obj->rotation.x << " " << obj->rotation.y << " " << obj->rotation.z << std::endl;
        out << obj->scale.x << " " << obj->scale.y << " " << obj->scale.z << std::endl;
        out << obj->color.r << " " << obj->color.g << " " << obj->color.b << std::endl;
        out << obj->roughness << " " << obj->metallic << std::endl;
        out << std::quoted(obj->texturePath.empty() ? "NONE" : obj->texturePath) << std::endl;

        out << obj->components.size() << std::endl;
        for (auto c : obj->components)
        {
            out << c->GetTypeName() << std::endl;
            c->Save(out);
        }
    }
    out.close();
    std::cout << "Scene saved to " << filename << std::endl;
}

void SceneContext::LoadScene(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        std::cerr << "Failed to open file for loading: " << filename << std::endl;
        return;
    }

    std::string header;
    in >> header;
    if (header != "SCENE_v1")
    {
        std::cerr << "Invalid scene file format: " << filename << std::endl;
        return;
    }

    // Clear current scene
    for (auto obj : objects)
        delete obj;
    objects.clear();
    selectedObject = nullptr;

    int objCount;
    in >> objCount;

    for (int i = 0; i < objCount; i++)
    {
        std::string tag;
        in >> tag; // OBJECT
        if (tag != "OBJECT")
            break;

        std::string name, meshPath, texPath;
        in >> std::quoted(name);
        in >> std::quoted(meshPath);

        Mesh *mesh = nullptr;
        if (meshPath == "internal:cube")
        {
            mesh = GeometryUtils::CreateCube();
        }
        else
        {
            mesh = ModelLoader::LoadMesh(meshPath);
        }

        if (!mesh)
        {
            // Fallback
            mesh = GeometryUtils::CreateCube();
        }

        SceneObject *obj = new SceneObject(name, mesh);
        obj->meshPath = meshPath;

        in >> obj->position.x >> obj->position.y >> obj->position.z;
        in >> obj->rotation.x >> obj->rotation.y >> obj->rotation.z;
        in >> obj->scale.x >> obj->scale.y >> obj->scale.z;
        in >> obj->color.r >> obj->color.g >> obj->color.b;
        in >> obj->roughness >> obj->metallic;
        in >> std::quoted(texPath);

        if (texPath != "NONE")
        {
            obj->texturePath = texPath;
            if (obj->mesh)
            {
                obj->mesh->textures.clear();
                Texture diffuseMap(texPath.c_str(), "diffuse");
                Texture specularMap(texPath.c_str(), "specular");
                if (diffuseMap.id != 0)
                {
                    obj->mesh->textures.push_back(diffuseMap);
                    obj->mesh->textures.push_back(specularMap);
                }
            }
        }

        int compCount;
        in >> compCount;
        for (int j = 0; j < compCount; j++)
        {
            std::string typeName;
            in >> typeName;

            auto &creators = ComponentRegistry::Instance().GetCreators();
            if (creators.find(typeName) != creators.end())
            {
                Component *c = creators.at(typeName)();
                c->Load(in);
                obj->AddComponent(c);
            }
            else
            {
                std::cerr << "Unknown component type: " << typeName << std::endl;
            }
        }
        AddObject(obj);
    }
    in.close();
    std::cout << "Scene loaded from " << filename << std::endl;
}
