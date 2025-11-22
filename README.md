# Mist Engine

A modern C++20 game engine built with SFML 3.0, designed for creating top-down 2D games with a modular, systems-based architecture.

## Overview

Mist Engine is a work-in-progress game engine that emphasizes clean architecture, modern C++ features, and extensibility. The engine uses a systems-based design where all functionality is implemented as discrete, composable systems that interact through a shared context.

## Features

### Engine Core

- **Systems-Based Architecture**: Modular design where engine functionality is implemented as `CoreSystem` subclasses
- **Scene Management**: Stack-based scene system supporting scene transitions, pushing, and popping
- **Entity-Component System**: WorldObjects with component composition using modern C++ concepts and type-safe templates
- **Lifecycle Management**: Standardized Initialize → Start → Tick → Render → Shutdown pipeline
- **Input Handling**: Event-driven input system built on SFML events

## Design Philosophy

**Systems-Based Design**: All engine functionality is implemented as systems that register with the `SystemsRegistry`. Systems receive lifecycle callbacks and have access to `EngineContext`.

**Entity-Component Model**: Game entities are `WorldObject` instances that can have `Component` instances attached at runtime. Components provide behavior and rendering logic.

**Scene Stack**: Scenes can be pushed onto or popped from a stack, enabling menu overlays, pause screens, and scene transitions.

## Building

### Prerequisites

- C++20 compatible compiler
- SFML 3.0 (statically linked)

## Development Status

This project is in active development and serves as a learning platform for game engine architecture. The core systems are functional, and the foundation for a modular, extensible engine is in place.
