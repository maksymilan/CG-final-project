#include "Application.h"
#include <iostream>
#include <algorithm>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ModelLoader.h" 
#include "GeometryUtils.h"

// 构造与析构保持不变
Application::Application(const std::string& title, int width, int height)
    : appTitle(title), scrWidth(width), scrHeight(height), 
      deltaTime(0.0f), lastFrame(0.0f), 
      isMousePressed(false), firstMouse(true),
      camera(nullptr), scene(nullptr), mainShader(nullptr)
{
    lastX = width / 2.0f;
    lastY = height / 2.0f;
}

Application::~Application() {
    if (scene) delete scene;
    if (camera) delete camera;
    if (mainShader) delete mainShader;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Application::InitGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(scrWidth, scrHeight, appTitle.c_str(), NULL, NULL);
    if (!window) return false;
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;
    glEnable(GL_DEPTH_TEST);
    return true;
}

bool Application::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.2f;
    ImGui::StyleColorsDark();
    
    // UI 样式微调
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3f, 0.6f, 0.9f, 1.0f); // 选中项颜色

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    return true;
}

void Application::InitScene() {
    camera = new Camera(glm::vec3(0.0f, 3.0f, 8.0f));
    scene = new SceneContext();
    mainShader = new Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    // 1. 地面 (Ground)
    // 注意：我们将地面稍微下移，并设为极薄，作为"平面"
    Mesh* floorMesh = GeometryUtils::CreateCube();
    SceneObject* floorObj = new SceneObject("Ground Plane", floorMesh);
    floorObj->scale = glm::vec3(20.0f, 0.01f, 20.0f); // 20x20 的平面
    floorObj->position = glm::vec3(0.0f, -0.01f, 0.0f);
    floorObj->color = glm::vec3(0.25f, 0.25f, 0.25f);
    scene->AddObject(floorObj);

    // 2. 立方体
    Mesh* cubeMesh = GeometryUtils::CreateCube();
    SceneObject* cubeObj = new SceneObject("Red Cube", cubeMesh);
    cubeObj->position = glm::vec3(-2.0f, 0.5f, 0.0f);
    cubeObj->color = glm::vec3(0.8f, 0.2f, 0.2f);
    scene->AddObject(cubeObj);

    // 3. 球体 (Stub)
    Mesh* sphereMesh = GeometryUtils::CreateSphere(20, 20);
    SceneObject* sphereObj = new SceneObject("Blue Sphere", sphereMesh);
    sphereObj->position = glm::vec3(2.0f, 0.5f, 0.0f);
    sphereObj->color = glm::vec3(0.2f, 0.4f, 0.8f);
    scene->AddObject(sphereObj);

    // 初始不选中任何物体，防止误操作
    scene->selectedObject = nullptr;
}

// 删除选中的物体
void Application::DeleteSelectedObject() {
    if (!scene || !scene->selectedObject) return;

    // 从列表中移除
    auto& objs = scene->objects;
    for (auto it = objs.begin(); it != objs.end(); ) {
        if (*it == scene->selectedObject) {
            // 释放内存
            delete (*it)->mesh; // 如果 Mesh 是共享的，这里需要更智能的指针，目前假设独占
            delete *it;
            it = objs.erase(it);
            scene->selectedObject = nullptr;
            break; // 找到并删除后退出
        } else {
            ++it;
        }
    }
}

void Application::Run() {
    if (!InitGLFW()) return;
    if (!InitImGui()) return;
    InitScene();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput();

        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderScene();
        RenderUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::ProcessInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (ImGui::GetIO().WantCaptureKeyboard) return;

    // 摄像机移动
    if (camera) {
        float speed = deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed *= 3.0f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera->ProcessKeyboard(FORWARD, speed);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera->ProcessKeyboard(BACKWARD, speed);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera->ProcessKeyboard(LEFT, speed);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera->ProcessKeyboard(RIGHT, speed);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera->ProcessKeyboard(DOWN, speed);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera->ProcessKeyboard(UP, speed);
    }

    // 删除快捷键 (Delete 或 Backspace)
    // 增加一个简单的防抖动，或者依赖操作系统的 key repeat
    static bool deletePressed = false;
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
        if (!deletePressed) {
            DeleteSelectedObject();
            deletePressed = true;
        }
    } else {
        deletePressed = false;
    }
}

// ---------------- 升级版射线检测算法 ----------------

// Ray-AABB Intersection (Slab Method)
// 检测射线是否击中局部空间的标准立方体 (min=-0.5, max=0.5)
bool Application::IntersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                                   const glm::vec3& boxMin, const glm::vec3& boxMax, 
                                   float& t) {
    glm::vec3 tMin = (boxMin - rayOrigin) / rayDir;
    glm::vec3 tMax = (boxMax - rayOrigin) / rayDir;
    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);
    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
    
    if (tNear > tFar || tFar < 0.0f) return false;
    
    t = tNear;
    return true;
}

void Application::SelectObjectFromMouse(double xpos, double ypos) {
    if (!camera || !scene) return;

    // 1. 生成世界空间射线
    float x = (2.0f * xpos) / scrWidth - 1.0f;
    float y = 1.0f - (2.0f * ypos) / scrHeight;
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec3 rayDirWorld = glm::normalize(glm::vec3(glm::inverse(camera->GetViewMatrix()) * rayEye));
    glm::vec3 rayOriginWorld = camera->Position;

    SceneObject* bestObj = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    // 2. 遍历所有物体，转换到局部空间进行检测
    for (auto obj : scene->objects) {
        // 构建模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obj->position);
        model = glm::rotate(model, glm::radians(obj->rotation.y), glm::vec3(0,1,0));
        model = glm::rotate(model, glm::radians(obj->rotation.x), glm::vec3(1,0,0));
        model = glm::rotate(model, glm::radians(obj->rotation.z), glm::vec3(0,0,1));
        model = glm::scale(model, obj->scale);

        // 将射线变换到物体的局部空间 (Local Space)
        // 射线原点变换： P_local = InverseModel * P_world
        // 射线方向变换： D_local = InverseModel * D_world (注意这里方向向量w=0)
        glm::mat4 invModel = glm::inverse(model);
        glm::vec3 rayOriginLocal = glm::vec3(invModel * glm::vec4(rayOriginWorld, 1.0f));
        glm::vec3 rayDirLocal = glm::vec3(invModel * glm::vec4(rayDirWorld, 0.0f)); // 方向不归一化，以保持t的线性关系? 
        // 修正：AABB算法通常需要方向。为了得到正确的距离 t，我们需要小心缩放。
        // 但这里为了简化，我们只关心"是否击中"和"相对距离比较"。
        
        float t = 0.0f;
        // 我们的 Mesh 默认都是中心在 (0,0,0)，边长1的立方体 (-0.5 ~ 0.5)
        if (IntersectRayAABB(rayOriginLocal, rayDirLocal, glm::vec3(-0.5f), glm::vec3(0.5f), t)) {
            // 注意：此时的 t 是在局部空间下的距离，或者包含了缩放因子的距离。
            // 为了正确比较不同物体的距离，我们需要将交点转回世界坐标算距离
            glm::vec3 hitPointLocal = rayOriginLocal + rayDirLocal * t;
            glm::vec3 hitPointWorld = glm::vec3(model * glm::vec4(hitPointLocal, 1.0f));
            float worldDist = glm::distance(camera->Position, hitPointWorld);

            if (worldDist < minDistance) {
                minDistance = worldDist;
                bestObj = obj;
            }
        }
    }

    // 切换选中状态
    if (bestObj) {
        scene->selectedObject = bestObj;
    }
}

// --------------------------------------------------------

void Application::RenderScene() {
    if (!mainShader || !scene || !camera) return;

    mainShader->use();
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    mainShader->setMat4("projection", projection);
    mainShader->setMat4("view", view);
    mainShader->setVec3("viewPos", camera->Position);
    mainShader->setVec3("lightPos", glm::vec3(2.0f, 4.0f, 2.0f));
    mainShader->setVec3("lightColor", glm::vec3(1.0f));

    for (auto obj : scene->objects) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obj->position);
        model = glm::rotate(model, glm::radians(obj->rotation.x), glm::vec3(1,0,0));
        model = glm::rotate(model, glm::radians(obj->rotation.y), glm::vec3(0,1,0));
        model = glm::rotate(model, glm::radians(obj->rotation.z), glm::vec3(0,0,1));
        model = glm::scale(model, obj->scale);

        mainShader->setMat4("model", model);
        mainShader->setVec3("objectColor", obj->color);

        if (obj->mesh) obj->mesh->Draw(*mainShader);

        // 选中高亮逻辑
        if (obj == scene->selectedObject) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(2.5f); // 稍微加粗
            
            // 微微放大线框，防止 Z-fighting (闪烁)
            glm::mat4 highlightModel = glm::scale(model, glm::vec3(1.005f)); 
            mainShader->setMat4("model", highlightModel);
            
            // 地面和普通物体用不同的高亮颜色
            if (obj->name.find("Ground") != std::string::npos) {
                 mainShader->setVec3("objectColor", glm::vec3(0.0f, 1.0f, 0.5f)); // 绿色高亮地面
            } else {
                 mainShader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.0f)); // 黄色高亮物体
            }

            if (obj->mesh) obj->mesh->Draw(*mainShader);
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glLineWidth(1.0f);
        }
    }
}

void Application::RenderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ---------------- 编辑器主面板 ----------------
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene Editor");

    ImGui::Text("SCENE HIERARCHY");
    ImGui::Separator();
    
    ImGui::BeginChild("HierarchyList", ImVec2(0, 200), true);
    for (auto obj : scene->objects) {
        bool isSelected = (scene->selectedObject == obj);
        if (ImGui::Selectable(obj->name.c_str(), isSelected)) {
            scene->selectedObject = obj;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndChild();

    // ---------------- 属性面板 ----------------
    if (scene->selectedObject) {
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Text("INSPECTOR: %s", scene->selectedObject->name.c_str());
        ImGui::Separator();

        ImGui::Text("Transform");
        ImGui::DragFloat3("Position", (float*)&scene->selectedObject->position, 0.05f);
        ImGui::DragFloat3("Rotation", (float*)&scene->selectedObject->rotation, 1.0f);
        
        // 缩放：对于地面，我们可能希望分开控制 X/Z 和 Y
        if (scene->selectedObject->name.find("Ground") != std::string::npos) {
            ImGui::Text("Scale (Plane Mode)");
            ImGui::DragFloat("Size X", &scene->selectedObject->scale.x, 0.1f, 0.1f, 100.0f);
            ImGui::DragFloat("Size Z", &scene->selectedObject->scale.z, 0.1f, 0.1f, 100.0f);
            ImGui::DragFloat("Thickness", &scene->selectedObject->scale.y, 0.01f, 0.01f, 5.0f);
        } else {
            ImGui::DragFloat3("Scale", (float*)&scene->selectedObject->scale, 0.05f, 0.01f, 100.0f);
        }

        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Text("Material");
        ImGui::ColorEdit3("Color", (float*)&scene->selectedObject->color);

        ImGui::Dummy(ImVec2(0, 15));
        // 删除按钮
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("DELETE OBJECT", ImVec2(-1, 30))) {
            DeleteSelectedObject();
        }
        ImGui::PopStyleColor(2);
    } 
    else {
        ImGui::Dummy(ImVec2(0, 20));
        ImGui::TextDisabled("Select an object to edit attributes");
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// ---------------- 回调函数更新 ----------------

void Application::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (!app) return;

    bool isAltDown = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
    bool isRightMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (!isAltDown && !isRightMouse) {
        app->firstMouse = true;
        return;
    }

    if (app->firstMouse) {
        app->lastX = xpos;
        app->lastY = ypos;
        app->firstMouse = false;
    }

    float xoffset = xpos - app->lastX;
    float yoffset = app->lastY - ypos; 
    app->lastX = xpos; app->lastY = ypos;

    if (isAltDown && app->scene && app->scene->selectedObject) {
        app->scene->selectedObject->rotation.y += xoffset * 0.5f;
        app->scene->selectedObject->rotation.x -= yoffset * 0.5f;
    } else if (isRightMouse && app->camera) {
        app->camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void Application::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (!app || ImGui::GetIO().WantCaptureMouse) return;

    if (app->scene && app->scene->selectedObject) {
        // [修复] 线性比例缩放: 使用乘法而不是加法
        // yoffset 通常是 1 或 -1
        float scaleFactor = 1.0f + (float)yoffset * 0.1f; // 每次缩放 10%
        
        // 限制缩放系数防止过小
        if (scaleFactor < 0.1f) scaleFactor = 0.1f;

        app->scene->selectedObject->scale *= scaleFactor;
    } 
    else if (app->camera) {
        app->camera->ProcessMouseScroll(yoffset);
    }
}

void Application::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (!app || ImGui::GetIO().WantCaptureMouse) return;

    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            app->scene->selectedObject = nullptr; // 取消选中
            app->isMousePressed = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (button == GLFW_MOUSE_BUTTON_LEFT) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            app->SelectObjectFromMouse(xpos, ypos);
        }
    } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT) {
        app->isMousePressed = false;
        app->firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (app) { app->scrWidth = width; app->scrHeight = height; }
}