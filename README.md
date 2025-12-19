# Mist Engine

A modern C++20 game engine built with SFML 3.0, designed for creating top-down 2D games with a modular, systems-based architecture.

## Overview

Mist Engine is a data-driven 2D game engine that emphasizes clean architecture, modern C++ features, and extensibility. The engine uses a systems-based design where all functionality is implemented as discrete, composable systems that interact through a shared context.

## Features

### Core Architecture

- **Systems-Based Design**: Modular engine functionality implemented as `CoreSystem` subclasses with standardized lifecycle hooks
- **Component Composition**: Runtime component attachment using modern C++ concepts and automatic registration via macros
- **Scene Management**: Stack-based scene system supporting transitions, pushing, and popping for menus and gameplay states
- **Data-Driven Entities**: JSON-based object definitions with optional DataAsset templates and inline object support

### Asset Pipeline

- **Hybrid Loading Strategy**: Global assets (UI, fonts) loaded once; level-specific assets loaded/unloaded per scene
- **Async Coroutines**: C++20 coroutine-based parallel asset loading with automatic dependency discovery
- **Manifest System**: Scene manifests define textures, fonts, sounds, and objects with component overrides
- **Automatic Deduplication**: Smart asset queueing prevents duplicate loads across DataAssets and scene manifests

### TileMap System

- **Component-Based Rendering**: TileMap component with built-in serialization and SFML rendering pipeline integration
- **TileSheet Support**: Automatic tile atlas parsing with configurable tile dimensions
- **Editor Integration**: Level designer scene with ImGui-based tile palette and properties panels

### Component System

- **Macro-Based Registration**: Components auto-register via `REGISTER_COMPONENT()` macro
- **Component Manager**: Dedicated component lifecycle management with type-safe accessors
- **Built-in Components**: Transform, Sprite, and TileMap components with serialization support

## Architecture Highlights

**Systems Registry**: Central registry manages all engine systems with template-based type-safe access. Systems receive lifecycle callbacks (Initialize → Start → Tick → Render → Shutdown) and share state via `EngineContext`.

**Component Factory**: Components register themselves automatically and are instantiated via factory pattern from JSON data. Component initialization happens through `Initialize(const nlohmann::json& Data)` for data-driven object composition.

**Asset Resolution**: File extension-based automatic type detection (`.png` → Texture, `.ttf` → Font, `.wav` → Sound). Recursive JSON scanning discovers asset dependencies without per-component boilerplate.

**Inline Objects**: Scene manifests can define objects purely from components without requiring DataAsset templates, enabling rapid prototyping and scene-specific entities.

## Building

### Prerequisites

- **Compiler**: C++20 compatible (MSVC 14.3+, GCC 10+, Clang 11+)
- **Graphics**: SFML 3.0 (statically linked)
- **JSON**: nlohmann/json (header-only, included)
- **UI**: Dear ImGui with ImGui-SFML (for editor)

### Windows (Visual Studio)

1. Install SFML 3.0 and configure paths in project settings
2. Open `RPG.vcxproj` in Visual Studio 2022
3. Build in Debug|x64 configuration

## Development Status

This project is in active development as a learning platform for game engine architecture. The core systems are functional, with ongoing work on input handling, camera controls, and level editing tools.
