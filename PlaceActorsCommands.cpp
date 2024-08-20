// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaceActorsCommands.h"

#define LOCTEXT_NAMESPACE "FPlaceActorsModule"

void FPlaceActorsCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "PlaceActors", "Execute PlaceActors action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
