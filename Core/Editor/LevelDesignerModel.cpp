#include "LevelDesignerModel.h"
#include "../World/World.h"
#include "../World/WorldObject.h"
#include "../Components/TileMapComponent.h"
#include "../Components/TransformComponent.h"
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

    bool LevelDesignerModel::CanPlayTest() const
    {
        return CurrentScene.IsValid();
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

    std::shared_ptr<TileMapComponent> LevelDesignerModel::GetSelectedTileMap() const
    {
        const std::shared_ptr<WorldObject> Selected = Selection.GetPrimary();
        if (!Selected)
            return nullptr;

        return Selected->Components().Get<TileMapComponent>();
    }

    void LevelDesignerModel::SelectObjectAtPosition(WorldCoordinate WorldPos, bool bAdditive)
    {
        WorldObject* HitObject = GetObjectAtPosition(WorldPos);

        if (HitObject)
        {
            if (bAdditive)
            {
                Selection.Toggle(HitObject->shared_from_this());
            }
            else
            {
                if (!Selection.Contains(HitObject))
                {
                    Selection.Clear();
                    Selection.Add(HitObject->shared_from_this());
                }
            }
        }
        else
        {
            if (!bAdditive)
            {
                Selection.Clear();
            }
        }
    }

    void LevelDesignerModel::SelectObjectsInRectangle(WorldCoordinate TopLeft, WorldCoordinate BottomRight,
                                                      bool bAdditive)
    {
        const std::vector<WorldObject*> ObjectsInRect = GetObjectsInRectangle(TopLeft, BottomRight);

        if (!bAdditive)
        {
            Selection.Clear();
        }

        for (WorldObject* Object : ObjectsInRect)
        {
            if (Object)
            {
                Selection.Add(Object->shared_from_this());
            }
        }
    }

    void LevelDesignerModel::ClearSelection()
    {
        Selection.Clear();
    }

    void LevelDesignerModel::SetHoveredObject(WorldCoordinate WorldPos)
    {
        HoveredObject = GetObjectAtPosition(WorldPos);
    }

    void LevelDesignerModel::ClearHoveredObject()
    {
        HoveredObject = nullptr;
    }

    void LevelDesignerModel::StartSelectionRectangle(WindowCoordinate Start)
    {
        bIsSelectingRectangle = true;
        SelectRectStart = Start;
        SelectRectCurrent = Start;
    }

    void LevelDesignerModel::UpdateSelectionRectangle(WindowCoordinate Current)
    {
        SelectRectCurrent = Current;
    }

    void LevelDesignerModel::ClearSelectionRectangle()
    {
        bIsSelectingRectangle = false;
    }

    GizmoPart LevelDesignerModel::GetGizmoPartAtPosition(WorldCoordinate WorldPos) const
    {
        const std::vector<std::shared_ptr<WorldObject>> SelectedObjects = Selection.GetAllValid();
        if (SelectedObjects.empty())
            return GizmoPart::None;

        for (const std::shared_ptr<WorldObject>& Object : SelectedObjects)
        {
            TransformComponent* Transform = const_cast<WorldObject*>(Object.get())->Transform();
            if (!Transform)
                continue;

            const sf::Vector2f ObjectPos = Transform->Position;
            const sf::Vector2f Delta = WorldPos.Value - ObjectPos;
            const float DistanceSq = Delta.x * Delta.x + Delta.y * Delta.y;

            constexpr float HandleRadius = 5.0f;
            if (DistanceSq <= HandleRadius * HandleRadius)
            {
                return GizmoPart::CenterHandle;
            }

            constexpr float ArrowLength = 20.0f;
            constexpr float ArrowClickRadius = 8.0f;

            const sf::Vector2f XAxisEnd(ObjectPos.x + ArrowLength, ObjectPos.y);
            const sf::Vector2f XAxisDelta = WorldPos.Value - XAxisEnd;
            const float XAxisDistSq = XAxisDelta.x * XAxisDelta.x + XAxisDelta.y * XAxisDelta.y;
            if (XAxisDistSq <= ArrowClickRadius * ArrowClickRadius)
            {
                return GizmoPart::XAxis;
            }

            const sf::Vector2f YAxisEnd(ObjectPos.x, ObjectPos.y + ArrowLength);
            const sf::Vector2f YAxisDelta = WorldPos.Value - YAxisEnd;
            const float YAxisDistSq = YAxisDelta.x * YAxisDelta.x + YAxisDelta.y * YAxisDelta.y;
            if (YAxisDistSq <= ArrowClickRadius * ArrowClickRadius)
            {
                return GizmoPart::YAxis;
            }
        }

        return GizmoPart::None;
    }

    void LevelDesignerModel::StartDraggingObjects(WorldCoordinate StartPos, DragMode Mode)
    {
        bIsDraggingObjects = true;
        CurrentDragMode = Mode;
        DragStartPos = StartPos;
        DraggedObjectsInitialPositions.clear();

        const std::vector<std::shared_ptr<WorldObject>> SelectedObjects = Selection.GetAllValid();
        for (const std::shared_ptr<WorldObject>& Object : SelectedObjects)
        {
            TransformComponent* Transform = Object->Transform();
            if (Transform)
            {
                DraggedObjectsInitialPositions.emplace_back(Object.get(), Transform->Position);
            }
        }
    }

    void LevelDesignerModel::UpdateDraggedObjects(WorldCoordinate CurrentPos)
    {
        if (!bIsDraggingObjects)
            return;

        const sf::Vector2f TotalDelta = CurrentPos.Value - DragStartPos.Value;
        sf::Vector2f ConstrainedDelta = TotalDelta;

        if (CurrentDragMode == DragMode::XOnly)
        {
            ConstrainedDelta.y = 0.0f;
        }
        else if (CurrentDragMode == DragMode::YOnly)
        {
            ConstrainedDelta.x = 0.0f;
        }

        for (const std::pair<WorldObject*, sf::Vector2f>& Pair : DraggedObjectsInitialPositions)
        {
            WorldObject* Object = Pair.first;
            const sf::Vector2f InitialPos = Pair.second;

            TransformComponent* Transform = Object->Transform();
            if (Transform)
            {
                Transform->Position = InitialPos + ConstrainedDelta;
            }
        }
    }

    void LevelDesignerModel::EndDraggingObjects()
    {
        bIsDraggingObjects = false;
        CurrentDragMode = DragMode::None;
        DraggedObjectsInitialPositions.clear();
    }

    sf::FloatRect LevelDesignerModel::GetObjectBounds(const WorldObject* Object) const
    {
        if (!Object)
            return sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(0, 0));

        const TransformComponent* Transform = const_cast<WorldObject*>(Object)->Transform();
        if (!Transform)
            return sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(0, 0));

        const sf::Vector2f Position = Transform->Position;
        constexpr float BoundsSize = 10.0f;
        constexpr float HalfSize = BoundsSize / 2.0f;

        return sf::FloatRect(sf::Vector2f(Position.x - HalfSize, Position.y - HalfSize),
                             sf::Vector2f(BoundsSize, BoundsSize));
    }

    WorldObject* LevelDesignerModel::GetObjectAtPosition(WorldCoordinate WorldPos) const
    {
        const std::vector<std::shared_ptr<WorldObject>>& AllObjects = WorldRef.Objects().GetAll();

        WorldObject* ClosestObject = nullptr;
        float ClosestDistanceSq = std::numeric_limits<float>::max();

        for (auto It = AllObjects.rbegin(); It != AllObjects.rend(); ++It)
        {
            const std::shared_ptr<WorldObject>& Object = *It;
            if (!Object || Object->GetTag() != ObjectTag::Game)
                continue;

            const sf::FloatRect Bounds = GetObjectBounds(Object.get());
            if (!Bounds.contains(WorldPos.Value))
                continue;

            TransformComponent* Transform = const_cast<WorldObject*>(Object.get())->Transform();
            if (!Transform)
                continue;

            const sf::Vector2f Delta = Transform->Position - WorldPos.Value;
            const float DistanceSq = Delta.x * Delta.x + Delta.y * Delta.y;

            if (DistanceSq < ClosestDistanceSq)
            {
                ClosestDistanceSq = DistanceSq;
                ClosestObject = Object.get();
            }
        }

        return ClosestObject;
    }

    std::vector<WorldObject*> LevelDesignerModel::GetObjectsInRectangle(WorldCoordinate TopLeft,
                                                                        WorldCoordinate BottomRight) const
    {
        const std::vector<std::shared_ptr<WorldObject>>& AllObjects = WorldRef.Objects().GetAll();
        std::vector<WorldObject*> ObjectsInRect;

        const sf::FloatRect SelectionRect(TopLeft.Value, BottomRight.Value - TopLeft.Value);

        for (const std::shared_ptr<WorldObject>& Object : AllObjects)
        {
            if (!Object || Object->GetTag() != ObjectTag::Game)
                continue;

            const sf::FloatRect ObjectBounds = GetObjectBounds(Object.get());
            if (SelectionRect.findIntersection(ObjectBounds).has_value())
            {
                ObjectsInRect.push_back(Object.get());
            }
        }

        return ObjectsInRect;
    }

    void LevelDesignerModel::StartTimePreview()
    {
        SavedPreviewHours = WorldRef.GetEnvironment().GetTime().GetHourOfDay();
        SavedPreviewMinutes = WorldRef.GetEnvironment().GetTime().GetMinutes();
        bIsPreviewingTime = true;
        TimePreviewStart = std::chrono::steady_clock::now();
    }

    void LevelDesignerModel::StopTimePreview()
    {
        bIsPreviewingTime = false;
        WorldRef.GetEnvironment().GetTime().SetTime(SavedPreviewHours, SavedPreviewMinutes);
    }

    void LevelDesignerModel::UpdateTimePreview()
    {
        if (!bIsPreviewingTime)
            return;

        const auto Now = std::chrono::steady_clock::now();
        const float Elapsed = std::chrono::duration<float>(Now - TimePreviewStart).count();

        if (Elapsed >= TimePreviewDuration)
        {
            StopTimePreview();
        }
        else
        {
            const float Progress = Elapsed / TimePreviewDuration;
            const float TotalMinutes = Progress * (24.0f * 60.0f);
            const int PreviewHours = static_cast<int>(TotalMinutes / 60.0f) % 24;
            const int PreviewMinutes = static_cast<int>(TotalMinutes) % 60;
            WorldRef.GetEnvironment().GetTime().SetTime(PreviewHours, PreviewMinutes);
        }
    }

    float LevelDesignerModel::GetTimePreviewProgress() const
    {
        if (!bIsPreviewingTime)
            return 0.0f;

        const auto Now = std::chrono::steady_clock::now();
        const float Elapsed = std::chrono::duration<float>(Now - TimePreviewStart).count();
        return std::min(Elapsed / TimePreviewDuration, 1.0f);
    }
}
