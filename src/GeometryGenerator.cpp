#include "GeometryGenerator.h"
#include "GeometryUtils.h"

// Uses existing GeometryUtils implementation internally
Mesh* GeometryGenerator::CreateCube() {
    return GeometryUtils::CreateCube();
}

// Converts single segment parameter to match existing dual-parameter method
Mesh* GeometryGenerator::CreateSphere(int segments) {
    return GeometryUtils::CreateSphere(segments, segments);
}

Mesh* GeometryGenerator::CreateCylinder(float radius, float height, int segments) {
    return GeometryUtils::CreateCylinder(radius, height, segments);
}

Mesh* GeometryGenerator::CreateCone(float radius, float height, int segments) {
    return GeometryUtils::CreateCone(radius, height, segments);
}

Mesh* GeometryGenerator::CreatePlane(float width, float depth) {
    return GeometryUtils::CreatePlane(width, depth);
}