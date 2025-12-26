#pragma once
#include <memory>
#include <vector>

namespace Core
{
    class WorldObject;

    class ObjectSelection
    {
    public:
        void Add(const std::shared_ptr<WorldObject>& Object);
        void Remove(const std::shared_ptr<WorldObject>& Object);
        void Clear();
        void Toggle(const std::shared_ptr<WorldObject>& Object);

        bool Contains(const WorldObject* Object) const;
        bool IsEmpty() const;
        int GetCount() const;

        std::shared_ptr<WorldObject> GetPrimary() const;
        std::vector<std::shared_ptr<WorldObject>> GetAllValid() const;

    private:
        void CleanupDeadPointers() const;

        mutable std::vector<std::weak_ptr<WorldObject>> Objects;
    };
}
