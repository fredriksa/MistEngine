#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Systems/CoreSystem.hpp"

namespace Core
{
    class SystemsRegistry
    {
    public:
        SystemsRegistry() = default;

        template <typename T>
            requires IsCoreSystem<T>
        void Register(const std::shared_ptr<EngineContext>& InContext);

        const std::vector<std::shared_ptr<CoreSystem>>& GetCoreSystems() const { return CoreSystems; }

        template <typename T>
            requires IsCoreSystem<T>
        std::shared_ptr<T> GetCoreSystem()
        {
            auto it = SystemToCoreSystem.find(T::Type);
            if (it != SystemToCoreSystem.end())
            {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr;
        }

    private:
        std::unordered_map<ECoreSystemType, std::shared_ptr<CoreSystem>> SystemToCoreSystem;
        std::unordered_map<std::string, std::shared_ptr<CoreSystem>> NameToCoreSystem;
        std::vector<std::shared_ptr<CoreSystem>> CoreSystems;
    };

    template <typename T> requires IsCoreSystem<T>
    void SystemsRegistry::Register(const std::shared_ptr<EngineContext>& Context)
    {
        std::shared_ptr<CoreSystem> System = std::make_shared<T>(Context);
        auto [Iter, Inserted] = NameToCoreSystem.emplace(System->GetName(), System);
        SystemToCoreSystem.emplace(System->GetType(), System);
        CoreSystems.push_back(Iter->second);
    }
}
