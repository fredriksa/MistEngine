#include "ObjectSelection.h"
#include "../World/WorldObject.h"
#include <algorithm>

namespace Core
{
    void ObjectSelection::Add(const std::shared_ptr<WorldObject>& Object)
    {
        if (!Object || Contains(Object.get()))
            return;

        Objects.push_back(Object);
    }

    void ObjectSelection::Remove(const std::shared_ptr<WorldObject>& Object)
    {
        if (!Object)
            return;

        Objects.erase(
            std::remove_if(Objects.begin(), Objects.end(),
                           [&Object](const std::weak_ptr<WorldObject>& Weak)
                           {
                               const std::shared_ptr<WorldObject> Ptr = Weak.lock();
                               return !Ptr || Ptr.get() == Object.get();
                           }),
            Objects.end()
        );
    }

    void ObjectSelection::Clear()
    {
        Objects.clear();
    }

    void ObjectSelection::Toggle(const std::shared_ptr<WorldObject>& Object)
    {
        if (Contains(Object.get()))
            Remove(Object);
        else
            Add(Object);
    }

    bool ObjectSelection::Contains(const WorldObject* Object) const
    {
        if (!Object)
            return false;

        for (const std::weak_ptr<WorldObject>& Weak : Objects)
        {
            if (const std::shared_ptr<WorldObject> Ptr = Weak.lock())
            {
                if (Ptr.get() == Object)
                    return true;
            }
        }
        return false;
    }

    bool ObjectSelection::IsEmpty() const
    {
        CleanupDeadPointers();
        return Objects.empty();
    }

    int ObjectSelection::GetCount() const
    {
        CleanupDeadPointers();
        return static_cast<int>(Objects.size());
    }

    std::shared_ptr<WorldObject> ObjectSelection::GetPrimary() const
    {
        CleanupDeadPointers();

        if (Objects.empty())
            return nullptr;

        return Objects[0].lock();
    }

    std::vector<std::shared_ptr<WorldObject>> ObjectSelection::GetAllValid() const
    {
        CleanupDeadPointers();

        std::vector<std::shared_ptr<WorldObject>> Valid;
        for (const std::weak_ptr<WorldObject>& Weak : Objects)
        {
            if (std::shared_ptr<WorldObject> Ptr = Weak.lock())
            {
                Valid.push_back(Ptr);
            }
        }
        return Valid;
    }

    void ObjectSelection::CleanupDeadPointers() const
    {
        Objects.erase(
            std::remove_if(Objects.begin(), Objects.end(),
                           [](const std::weak_ptr<WorldObject>& Weak)
                           {
                               return Weak.expired();
                           }),
            Objects.end()
        );
    }
}
