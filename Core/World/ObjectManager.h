#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include "../ThirdParty/json.hpp"

namespace Core
{
    class WorldObject;
    class World;
    struct EngineContext;

    class ObjectManager
    {
    public:
        ObjectManager(World* Owner, std::shared_ptr<EngineContext> Context);

        std::shared_ptr<WorldObject> CreateObject();
        void Register(std::shared_ptr<WorldObject> Object);

        std::shared_ptr<WorldObject> GetByName(const std::string& Name) const;
        const std::vector<std::shared_ptr<WorldObject>>& GetAll() const { return Objects; }

        void Clear();
        void Remove(std::shared_ptr<WorldObject> Object);

        bool MoveUp(std::shared_ptr<WorldObject> Object);
        bool MoveDown(std::shared_ptr<WorldObject> Object);

        void RegisterName(const std::string& Name, std::shared_ptr<WorldObject> Object);
        void UnregisterName(const std::string& Name);

        void StartPendingComponents();

        nlohmann::json ToJson() const;

    private:
        World* Owner;
        std::shared_ptr<EngineContext> Context;
        std::vector<std::shared_ptr<WorldObject>> Objects;
        std::unordered_map<std::string, std::shared_ptr<WorldObject>> NamedObjects;
        std::vector<std::shared_ptr<WorldObject>> PendingStartObjects;
    };
}
