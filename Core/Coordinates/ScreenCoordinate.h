#pragma once
#include <SFML/System/Vector2.hpp>
#include "imgui.h"

namespace Core
{
	struct ScreenCoordinate
	{
		sf::Vector2f Value;

		ScreenCoordinate() : Value(0.0f, 0.0f) {}
		ScreenCoordinate(float X, float Y) : Value(X, Y) {}
		explicit ScreenCoordinate(const sf::Vector2f& V) : Value(V) {}

		float X() const { return Value.x; }
		float Y() const { return Value.y; }

		static ScreenCoordinate FromImGui(const ImVec2& V)
		{
			return ScreenCoordinate(V.x, V.y);
		}

		ImVec2 ToImGui() const
		{
			return ImVec2(Value.x, Value.y);
		}
	};
}
