#include "SceneContext.h"
#include <glm/gtc/matrix_transform.hpp>

SceneContext::SceneContext() {}

SceneContext::~SceneContext() {
    for (auto obj : objects) {
        delete obj->mesh; // 释放 Mesh 内存
        delete obj;
    }
    objects.clear();
}

void SceneContext::AddObject(SceneObject* obj) {
    objects.push_back(obj);
}

void SceneContext::DrawAll(Shader& shader) {
    for (auto obj : objects) {
        // 计算 Model 矩阵 [Part A 核心逻辑]
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obj->position);
        model = glm::rotate(model, glm::radians(obj->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(obj->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(obj->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, obj->scale);

        // 设置 Shader Uniforms
        shader.setMat4("model", model);
        shader.setVec3("objectColor", obj->color);

        // 绘制
        if (obj->mesh) {
            obj->mesh->Draw(shader);
        }
    }
}