#ifndef GEOMETRY_GENERATOR_H
#define GEOMETRY_GENERATOR_H

#include "Mesh.h"

// Forward declaration to avoid circular includes
class GeometryUtils;

// GeometryGenerator class: Implements the required interface
// Uses existing GeometryUtils internally to reuse implementation
class GeometryGenerator {
public:
    static Mesh* CreateCube();
    static Mesh* CreateSphere(int segments);
    static Mesh* CreateCylinder(float radius, float height, int segments);
    static Mesh* CreateCone(float radius, float height, int segments);
    static Mesh* CreatePlane(float width, float depth);
};

#endif