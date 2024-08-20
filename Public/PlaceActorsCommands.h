// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PlaceActorsStyle.h"

class FPlaceActorsCommands : public TCommands<FPlaceActorsCommands>
{
public:

	FPlaceActorsCommands()
		: TCommands<FPlaceActorsCommands>(TEXT("PlaceActors"), NSLOCTEXT("Contexts", "PlaceActors", "PlaceActors Plugin"), NAME_None, FPlaceActorsStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
