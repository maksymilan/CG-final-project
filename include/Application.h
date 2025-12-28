#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "SceneContext.h"
#include "Camera.h"
#include "Shader.h"

class Application {
public:
    Application(const std::string& title, int width, int height);
    ~Application();

    void Run();

private:
    GLFWwindow* window;
    int scrWidth;
    int scrHeight;
    std::string appTitle;

    Camera* camera;
    SceneContext* scene;
    Shader* mainShader;

    // 状态
    float deltaTime;
    float lastFrame;
    bool isMousePressed;
    bool firstMouse;
    float lastX, lastY;

    // 初始化
    bool InitGLFW();
    bool InitImGui();
    void InitScene();

    // 核心逻辑
    void ProcessInput();
    void RenderUI();
    void RenderScene();
    
    // 物体管理
    void DeleteSelectedObject(); // 新增：删除功能

    // 射线拾取系统 (升级版)
    void SelectObjectFromMouse(double xpos, double ypos);
    // 射线-AABB(轴对齐包围盒)相交检测
    bool IntersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                          const glm::vec3& boxMin, const glm::vec3& boxMax, 
                          float& t);

    // 静态回调
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

#endif