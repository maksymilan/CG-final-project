#ifndef RENDERER_H
#define RENDERER_H

#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

namespace PartC {
    class GeometryGenerator {
    public:
        // [接口] 生成球体
        // 目前实现：返回立方体代替
        static Mesh* CreateSphere(float radius, int segments);
    };

    class Renderer {
    public:
        // [接口] 统一渲染入口
        static void RenderMesh(Mesh* mesh, Shader& shader, const glm::mat4& modelMatrix);
        
        // [接口] 设置光照参数
        static void SetupLights(Shader& shader, const glm::vec3& camPos);
    };
}

#endif