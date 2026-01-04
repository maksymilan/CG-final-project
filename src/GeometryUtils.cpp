#include "GeometryUtils.h"
#include <vector>
#include <cmath>

const float PI = 3.1415926535f;

// Helper function to calculate normal
inline glm::vec3 CalculateNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
    return glm::normalize(glm::cross(b - a, c - a));
}

Mesh* GeometryUtils::CreateCube()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Cube vertices with correct normals and texture coordinates
    // Each face has its own set of vertices with correct normals
    Vertex cubeVertices[] = {
        // Front face
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
        
        // Back face
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        
        // Bottom face
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        
        // Top face
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        
        // Left face
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        
        // Right face
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)}
    };

    // Cube face indices (right-hand rule for outward normals)
    unsigned int cubeIndices[] = {
        0, 3, 2, 2, 1, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        8, 9, 10, 10, 11, 8, // Bottom face
        12, 15, 14, 14, 13, 12, // Top face
        16, 17, 18, 18, 19, 16, // Left face
        20, 23, 22, 22, 21, 20  // Right face
    };

    // Add vertices
    for (int i = 0; i < 24; i++) {
        vertices.push_back(cubeVertices[i]);
    }

    // Add indices
    for (int i = 0; i < 36; i++) {
        indices.push_back(cubeIndices[i]);
    }

    return new Mesh(vertices, indices, textures);
}

Mesh* GeometryUtils::CreateSphere(int latitudeSegments, int longitudeSegments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Generate vertices
    for (int lat = 0; lat <= latitudeSegments; lat++) {
        float theta = lat * PI / latitudeSegments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeSegments; lon++) {
            float phi = lon * 2 * PI / longitudeSegments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // Calculate vertex position
            glm::vec3 position;
            position.x = cosPhi * sinTheta;
            position.y = cosTheta;
            position.z = sinPhi * sinTheta;

            // Normal is the same as vertex position (since center is at origin)
            // Ensure normal is pointing outward by normalizing
            glm::vec3 normal = glm::normalize(position);

            // Texture coordinates
            glm::vec2 texCoord;
            texCoord.x = (float)lon / (float)longitudeSegments;
            texCoord.y = (float)lat / (float)latitudeSegments;

            vertices.push_back({position, normal, texCoord});
        }
    }

    // Generate indices
    for (int lat = 0; lat < latitudeSegments; lat++) {
        for (int lon = 0; lon < longitudeSegments; lon++) {
            unsigned int first = lat * (longitudeSegments + 1) + lon;
            unsigned int second = first + longitudeSegments + 1;

            // First triangle (ensure correct winding order for outward normals)
            indices.push_back(first);
            indices.push_back(first + 1);
            indices.push_back(second);

            // Second triangle (ensure correct winding order for outward normals)
            indices.push_back(first + 1);
            indices.push_back(second + 1);
            indices.push_back(second);
        }
    }

    return new Mesh(vertices, indices, textures);
}

Mesh* GeometryUtils::CreateCylinder(float radius, float height, int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    float halfHeight = height / 2.0f;
    int vertexCount = 0;

    // --- Generate side vertices (with correct side normals) --- 
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * PI;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);

        // Bottom vertex (side) - uses side normal
        glm::vec3 bottomPos(radius * cosAngle, -halfHeight, radius * sinAngle);
        glm::vec3 sideNormal = glm::normalize(glm::vec3(cosAngle, 0.0f, sinAngle)); // Outward normal for side
        vertices.push_back({bottomPos, sideNormal, glm::vec2((float)i / segments, 0.0f)});

        // Top vertex (side) - uses side normal
        glm::vec3 topPos(radius * cosAngle, halfHeight, radius * sinAngle);
        vertices.push_back({topPos, sideNormal, glm::vec2((float)i / segments, 1.0f)});
    }

    // --- Generate bottom face vertices (with correct bottom normals) --- 
    // Bottom center
    int bottomCenterIdx = vertices.size();
    vertices.push_back({glm::vec3(0.0f, -halfHeight, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0.5f)});
    
    // Bottom edge vertices (with bottom normal)
    int bottomEdgeStartIdx = vertices.size();
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * PI;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        
        glm::vec3 pos(radius * cosAngle, -halfHeight, radius * sinAngle);
        vertices.push_back({pos, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f + 0.5f * cosAngle, 0.5f + 0.5f * sinAngle)});
    }

    // --- Generate top face vertices (with correct top normals) --- 
    // Top center
    int topCenterIdx = vertices.size();
    vertices.push_back({glm::vec3(0.0f, halfHeight, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 0.5f)});
    
    // Top edge vertices (with top normal)
    int topEdgeStartIdx = vertices.size();
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * PI;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        
        glm::vec3 pos(radius * cosAngle, halfHeight, radius * sinAngle);
        vertices.push_back({pos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f + 0.5f * cosAngle, 0.5f + 0.5f * sinAngle)});
    }

    // --- Generate side indices --- 
    for (int i = 0; i < segments; i++) {
        int current = i * 2;
        int next = (i + 1) * 2;

        // First triangle (bottom to top to next top, counter-clockwise)
        indices.push_back(current);
        indices.push_back(current + 1);
        indices.push_back(next + 1);

        // Second triangle (bottom to next top to next bottom, counter-clockwise)
        indices.push_back(current);
        indices.push_back(next + 1);
        indices.push_back(next);
    }

    // --- Generate bottom face indices --- 
    for (int i = 0; i < segments; i++) {
        indices.push_back(bottomCenterIdx);
        indices.push_back(bottomEdgeStartIdx + i + 1);
        indices.push_back(bottomEdgeStartIdx + i);
    }

    // --- Generate top face indices --- 
    for (int i = 0; i < segments; i++) {
        indices.push_back(topCenterIdx);
        indices.push_back(topEdgeStartIdx + i);
        indices.push_back(topEdgeStartIdx + i + 1);
    }

    return new Mesh(vertices, indices, textures);
}

Mesh* GeometryUtils::CreateCone(float radius, float height, int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Generate vertices
    // Bottom center
    vertices.push_back({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0.5f)});
    
    // Bottom vertices
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * PI;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        // Bottom vertices have downward normal (-Y)
        vertices.push_back({glm::vec3(x, 0.0f, z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle))});
    }
    
    // Apex
    vertices.push_back({glm::vec3(0.0f, height, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 1.0f)});
    
    int apexIdx = vertices.size() - 1;
    
    // Generate bottom face indices with correct winding order (counter-clockwise for -Y normal)
    for (int i = 1; i < segments + 1; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    
    // Generate side face indices with correct winding order for outward normals
    for (int i = 1; i < segments + 1; i++) {
        indices.push_back(apexIdx);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    // Calculate side face normals (outward pointing)
    for (int i = 1; i < segments + 2; i++) {
        // Calculate triangle vertices
        glm::vec3 v0 = vertices[apexIdx].Position;
        // 使用与索引顺序一致的顶点顺序：apexIdx -> i+1 -> i
        glm::vec3 v1 = vertices[(i % segments) + 1].Position;
        glm::vec3 v2 = vertices[i].Position;
        
        // Calculate normal using cross product with correct winding order
        // Use (v1 - v0) × (v2 - v0) to get outward normal
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        
        // Update the vertex normal
        vertices[i].Normal = normal;
    }

    return new Mesh(vertices, indices, textures);
}

Mesh* GeometryUtils::CreatePlane(float width, float depth)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;

    // Plane vertices with correct normals and texture coordinates
    Vertex planeVertices[] = {
        // Bottom-left corner
        {glm::vec3(-halfWidth, 0.0f, halfDepth), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        // Bottom-right corner
        {glm::vec3(halfWidth, 0.0f, halfDepth), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        // Top-right corner
        {glm::vec3(halfWidth, 0.0f, -halfDepth), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        // Top-left corner
        {glm::vec3(-halfWidth, 0.0f, -halfDepth), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)}
    };

    // Plane indices (two triangles forming a quad)
    unsigned int planeIndices[] = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };

    // Add vertices
    for (int i = 0; i < 4; i++) {
        vertices.push_back(planeVertices[i]);
    }

    // Add indices
    for (int i = 0; i < 6; i++) {
        indices.push_back(planeIndices[i]);
    }

    return new Mesh(vertices, indices, textures);
}