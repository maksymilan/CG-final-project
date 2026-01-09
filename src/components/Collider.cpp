#include "Collider.h"

// Initialize static member
std::vector<Collider *> Collider::allColliders;

Collider::Collider()
{
    allColliders.push_back(this);
}

Collider::Collider(const Collider &other) : Component(other)
{
    isTrigger = other.isTrigger;
    allColliders.push_back(this);
}

Collider::~Collider()
{
    auto it = std::find(allColliders.begin(), allColliders.end(), this);
    if (it != allColliders.end())
    {
        allColliders.erase(it);
    }
}
