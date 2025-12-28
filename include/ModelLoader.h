#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <vector>
#include "Mesh.h"

// ModelLoader 类：负责从文件系统读取模型数据
// 职责：[Part B] 负责实现具体的 OBJ 解析逻辑
class ModelLoader {
public:
    // 从路径加载 OBJ 文件，返回一个新的 Mesh 指针
    // 注意：调用者负责管理返回指针的内存（delete）
    static Mesh* LoadMesh(const std::string& path);
};

#endif