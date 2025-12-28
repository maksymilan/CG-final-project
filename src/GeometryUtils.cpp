#include "GeometryUtils.h"
#include <vector>

// 辅助函数：添加面的两个三角形
void AddFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
             glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 normal) {
    unsigned int startIdx = vertices.size();
    vertices.push_back({a, normal, glm::vec2(0.0f, 0.0f)});
    vertices.push_back({b, normal, glm::vec2(1.0f, 0.0f)});
    vertices.push_back({c, normal, glm::vec2(1.0f, 1.0f)});
    vertices.push_back({d, normal, glm::vec2(0.0f, 1.0f)});
    
    indices.push_back(startIdx);
    indices.push_back(startIdx + 1);
    indices.push_back(startIdx + 2);
    indices.push_back(startIdx + 2);
    indices.push_back(startIdx + 3);
    indices.push_back(startIdx);
}

Mesh* GeometryUtils::CreateCube() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // 8个角点
    glm::vec3 p0(-0.5f, -0.5f,  0.5f); glm::vec3 p1( 0.5f, -0.5f,  0.5f);
    glm::vec3 p2( 0.5f,  0.5f,  0.5f); glm::vec3 p3(-0.5f,  0.5f,  0.5f);
    glm::vec3 p4(-0.5f, -0.5f, -0.5f); glm::vec3 p5( 0.5f, -0.5f, -0.5f);
    glm::vec3 p6( 0.5f,  0.5f, -0.5f); glm::vec3 p7(-0.5f,  0.5f, -0.5f);

    // 前面
    AddFace(vertices, indices, p0, p1, p2, p3, glm::vec3(0, 0, 1));
    // 后面
    AddFace(vertices, indices, p5, p4, p7, p6, glm::vec3(0, 0, -1));
    // 左面
    AddFace(vertices, indices, p4, p0, p3, p7, glm::vec3(-1, 0, 0));
    // 右面
    AddFace(vertices, indices, p1, p5, p6, p2, glm::vec3(1, 0, 0));
    // 上面
    AddFace(vertices, indices, p3, p2, p6, p7, glm::vec3(0, 1, 0));
    // 下面
    AddFace(vertices, indices, p4, p5, p1, p0, glm::vec3(0, -1, 0));

    return new Mesh(vertices, indices, textures);
}

Mesh* GeometryUtils::CreateSphere(int latitudeSegments, int longitudeSegments) {
    // [Part C Stub] 暂时用一个不同颜色的金字塔代替球体，证明是不同的形状
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // 金字塔顶点
    glm::vec3 top(0.0f, 0.5f, 0.0f);
    glm::vec3 base0(-0.5f, -0.5f, 0.5f);
    glm::vec3 base1( 0.5f, -0.5f, 0.5f);
    glm::vec3 base2( 0.5f, -0.5f, -0.5f);
    glm::vec3 base3(-0.5f, -0.5f, -0.5f);

    // 侧面法线暂略，统一向上
    glm::vec3 normal(0.0f, 1.0f, 0.0f);

    // 底面
    vertices.push_back({base0, glm::vec3(0,-1,0), {0,0}});
    vertices.push_back({base1, glm::vec3(0,-1,0), {1,0}});
    vertices.push_back({base2, glm::vec3(0,-1,0), {1,1}});
    vertices.push_back({base3, glm::vec3(0,-1,0), {0,1}});
    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(2); indices.push_back(3); indices.push_back(0);

    // 侧面 (简单处理，不共用顶点以保证法线垂直)
    unsigned int start = 4;
    Vertex vTop = {top, normal, {0.5, 1}};
    
    // 面1
    vertices.push_back(vTop); vertices.push_back({base0, normal, {0,0}}); vertices.push_back({base1, normal, {1,0}});
    indices.push_back(start); indices.push_back(start+1); indices.push_back(start+2); start+=3;
    
    // 面2
    vertices.push_back(vTop); vertices.push_back({base1, normal, {0,0}}); vertices.push_back({base2, normal, {1,0}});
    indices.push_back(start); indices.push_back(start+1); indices.push_back(start+2); start+=3;

    // 面3
    vertices.push_back(vTop); vertices.push_back({base2, normal, {0,0}}); vertices.push_back({base3, normal, {1,0}});
    indices.push_back(start); indices.push_back(start+1); indices.push_back(start+2); start+=3;

    // 面4
    vertices.push_back(vTop); vertices.push_back({base3, normal, {0,0}}); vertices.push_back({base0, normal, {1,0}});
    indices.push_back(start); indices.push_back(start+1); indices.push_back(start+2);

    return new Mesh(vertices, indices, textures);
}