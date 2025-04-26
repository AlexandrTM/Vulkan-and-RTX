#pragma once

#include "Model.h"
#include "DungeonComponents.h"
#include "GameContext.h"
#include "Character.h"

namespace Dungeon
{
	void createDungeonFloor(
		DungeonFloor& dungeonFloor, std::vector<Model>& models,
		Texture& floorTexture, Texture& wallTexture
	);
	void enterDungeon(
		DungeonFloor& dungeonFloor, GameContext& gameContext, Character& character
	);

}