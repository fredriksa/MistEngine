#include "LevelDesignerModel.h"
#include "../World/World.h"
#include "../World/WorldObject.h"
#include "../Components/TileMapComponent.h"
#include "../Scene/SceneLoader.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/AssetRegistrySystem.h"
#include "../TileMap/TileSheet.h"
#include <filesystem>
#include <fstream>

namespace Core
{
    void SceneInfo::SetPath(const std::string& InPath)
    {
        Path = InPath;

        const size_t LastSlash = InPath.find_last_of("/\\");
        const size_t LastDot = InPath.find_last_of('.');

        if (LastSlash != std::string::npos && LastDot != std::string::npos && LastDot > LastSlash)
        {
            Name = InPath.substr(LastSlash + 1, LastDot - LastSlash - 1);
        }
        else if (LastDot != std::string::npos)
        {
            Name = InPath.substr(0, LastDot);
        }
        else
        {
            Name = InPath;
        }
    }

    const std::string& SceneInfo::GetName() const
    {
        return Name;
    }

    const std::string& SceneInfo::GetPath() const
    {
        return Path;
    }

    bool SceneInfo::IsValid() const
    {
        return !Path.empty();
    }

    void SceneInfo::Clear()
    {
        Name.clear();
        Path.clear();
    }

    LevelDesignerModel::LevelDesignerModel(World& InWorld, std::shared_ptr<EngineContext> InContext)
        : WorldRef(InWorld), Context(std::move(InContext))
    {
    }

    std::shared_ptr<WorldObject> LevelDesignerModel::CreateObject()
    {
        return WorldRef.Objects().CreateObject();
    }

    void LevelDesignerModel::RemoveObject(const std::shared_ptr<WorldObject>& Object)
    {
        WorldRef.Objects().Remove(Object);
    }

    bool LevelDesignerModel::MoveObjectUp(const std::shared_ptr<WorldObject>& Object)
    {
        return WorldRef.Objects().MoveUp(Object);
    }

    bool LevelDesignerModel::MoveObjectDown(const std::shared_ptr<WorldObject>& Object)
    {
        return WorldRef.Objects().MoveDown(Object);
    }

    const std::vector<std::shared_ptr<WorldObject>>& LevelDesignerModel::GetAllObjects() const
    {
        return WorldRef.Objects().GetAll();
    }

    void LevelDesignerModel::SaveScene()
    {
        if (!CurrentScene.IsValid())
        {
            return;
        }

        const nlohmann::json SceneJson = WorldRef.ToJson();
        std::ofstream File(CurrentScene.GetPath());
        if (File.is_open())
        {
            File << SceneJson.dump(4);
            File.close();
        }
    }

    void LevelDesignerModel::SaveSceneAs(const std::string& SceneName)
    {
        const std::string ScenePath = "Game/Assets/Scenes/" + SceneName + ".json";
        CurrentScene.SetPath(ScenePath);
        SaveScene();
    }

    Task<> LevelDesignerModel::LoadScene(const std::string& SceneName)
    {
        WorldRef.Objects().Clear();

        const std::string ScenePath = "Game/Assets/Scenes/" + SceneName + ".json";
        CurrentScene.SetPath(ScenePath);

        SceneLoader Loader(Context);
        co_await Loader.LoadScene(SceneName, WorldRef);
    }

    void LevelDesignerModel::NewScene()
    {
        WorldRef.Objects().Clear();
        CurrentScene.Clear();
    }

    std::vector<std::string> LevelDesignerModel::GetAvailableScenes() const
    {
        std::vector<std::string> Scenes;
        const std::string ScenesDirectory = "Game/Assets/Scenes";

        if (!std::filesystem::exists(ScenesDirectory))
        {
            return Scenes;
        }

        for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator(ScenesDirectory))
        {
            if (Entry.is_regular_file() && Entry.path().extension() == ".json")
            {
                Scenes.push_back(Entry.path().stem().string());
            }
        }

        return Scenes;
    }

    void LevelDesignerModel::SelectTile(int TileSheetIndex, TileRectCoord Rect)
    {
        CurrentSelection.TileSheetIndex = static_cast<uint>(TileSheetIndex);
        CurrentSelection.SelectionRect = Rect;
    }

    void LevelDesignerModel::SetTileSheetIndex(int Index)
    {
        CurrentTileSheetIndex = Index;
        CurrentSelection.TileSheetIndex.reset();
        CurrentSelection.SelectionRect = TileRectCoord(TileCoordinate(0, 0), TileCoordinate(0, 0));
    }

    int LevelDesignerModel::GetTileSheetColumns(int TileSheetIndex) const
    {
        const std::vector<std::shared_ptr<WorldObject>>& AllObjects = WorldRef.Objects().GetAll();
        if (AllObjects.empty())
            return 0;

        const std::shared_ptr<EngineContext> ContextPtr = AllObjects[0]->GetContextPtr();
        if (!ContextPtr || !ContextPtr->SystemsRegistry)
            return 0;

        const std::shared_ptr<AssetRegistrySystem> AssetRegistry =
            ContextPtr->SystemsRegistry->GetCoreSystem<AssetRegistrySystem>();
        if (!AssetRegistry)
            return 0;

        const std::vector<std::shared_ptr<const TileSheet>> TileSheets = AssetRegistry->GetAllTileSheets();
        if (TileSheetIndex < 0 || TileSheetIndex >= static_cast<int>(TileSheets.size()))
            return 0;

        const std::shared_ptr<const TileSheet> Sheet = TileSheets[TileSheetIndex];
        return Sheet ? Sheet->GetNumColumns() : 0;
    }

    void LevelDesignerModel::SetSelectedObject(const std::shared_ptr<WorldObject>& Object)
    {
        SelectedObject = Object;
    }

    std::shared_ptr<WorldObject> LevelDesignerModel::GetSelectedObject() const
    {
        return SelectedObject.lock();
    }

    std::shared_ptr<TileMapComponent> LevelDesignerModel::GetSelectedTileMap() const
    {
        const std::shared_ptr<WorldObject> Selected = SelectedObject.lock();
        if (!Selected)
            return nullptr;

        return Selected->Components().Get<TileMapComponent>();
    }
}
