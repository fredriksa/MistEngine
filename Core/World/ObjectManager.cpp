#include "ObjectManager.h"
#include "WorldObject.h"
#include <algorithm>

namespace Core
{
    ObjectManager::ObjectManager(World* Owner, std::shared_ptr<EngineContext> Context)
        : Owner(Owner), Context(std::move(Context))
    {
    }

    std::shared_ptr<WorldObject> ObjectManager::CreateObject()
    {
        std::shared_ptr<WorldObject> Object = std::make_shared<WorldObject>(Context, Owner);
        Objects.push_back(Object);
        PendingStartObjects.push_back(Object);
        return Object;
    }

    void ObjectManager::Register(const std::shared_ptr<WorldObject>& Object)
    {
        Objects.push_back(Object);
        PendingStartObjects.push_back(Object);
    }

    std::shared_ptr<WorldObject> ObjectManager::GetByName(const std::string& Name) const
    {
        auto It = NamedObjects.find(Name);
        if (It != NamedObjects.end())
        {
            return It->second;
        }
        return nullptr;
    }

    void ObjectManager::RegisterName(const std::string& Name, const std::shared_ptr<WorldObject>& Object)
    {
        NamedObjects[Name] = Object;
    }

    void ObjectManager::UnregisterName(const std::string& Name)
    {
        NamedObjects.erase(Name);
    }

    void ObjectManager::Clear()
    {
        auto IsGameObject = [](const std::shared_ptr<WorldObject>& Obj)
        {
            return Obj && Obj->GetTag() == ObjectTag::Game;
        };

        Objects.erase(
            std::remove_if(Objects.begin(), Objects.end(), IsGameObject),
            Objects.end()
        );

        PendingStartObjects.erase(
            std::remove_if(PendingStartObjects.begin(), PendingStartObjects.end(), IsGameObject),
            PendingStartObjects.end()
        );

        for (auto It = NamedObjects.begin(); It != NamedObjects.end();)
        {
            if (IsGameObject(It->second))
            {
                It = NamedObjects.erase(It);
            }
            else
            {
                ++It;
            }
        }
    }

    void ObjectManager::Remove(const std::shared_ptr<WorldObject>& Object)
    {
        if (!Object)
            return;

        Objects.erase(std::remove(Objects.begin(), Objects.end(), Object), Objects.end());
        PendingStartObjects.erase(std::remove(PendingStartObjects.begin(), PendingStartObjects.end(), Object),
                                  PendingStartObjects.end());

        for (auto It = NamedObjects.begin(); It != NamedObjects.end();)
        {
            if (It->second == Object)
            {
                It = NamedObjects.erase(It);
            }
            else
            {
                ++It;
            }
        }
    }

    bool ObjectManager::MoveUp(const std::shared_ptr<WorldObject>& Object)
    {
        auto It = std::find(Objects.begin(), Objects.end(), Object);
        if (It == Objects.end() || It == Objects.begin())
        {
            return false;
        }

        std::iter_swap(It, It - 1);
        return true;
    }

    bool ObjectManager::MoveDown(const std::shared_ptr<WorldObject>& Object)
    {
        auto It = std::find(Objects.begin(), Objects.end(), Object);
        if (It == Objects.end() || It == Objects.end() - 1)
        {
            return false;
        }

        std::iter_swap(It, It + 1);
        return true;
    }

    void ObjectManager::StartPendingComponents()
    {
        if (PendingStartObjects.empty())
            return;

        for (const std::shared_ptr<WorldObject>& Object : PendingStartObjects)
        {
            Object->Components().Start();
        }

        PendingStartObjects.clear();
    }

    nlohmann::json ObjectManager::ToJson() const
    {
        nlohmann::json SceneJson;
        nlohmann::json ObjectsArray = nlohmann::json::array();

        for (const auto& Obj : Objects)
        {
            if (!Obj)
                continue;

            if (Obj->GetTag() == ObjectTag::Game)
            {
                ObjectsArray.push_back(Obj->ToJson());
            }
        }

        SceneJson["objects"] = ObjectsArray;
        return SceneJson;
    }
}
