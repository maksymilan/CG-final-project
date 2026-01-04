#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include "Mesh.h"

// GeometryUtils 类：负责程序化生成基础几何体
// 职责：[Part C] 负责实现球体、立方体等的数学生成算法
class GeometryUtils {
public:
    static Mesh* CreateCube();
    static Mesh* CreateSphere(int latitudeSegments, int longitudeSegments);
    static Mesh* CreateCylinder(float radius, float height, int segments);
    static Mesh* CreateCone(float radius, float height, int segments);
    static Mesh* CreatePlane(float width, float depth);
};

#endif