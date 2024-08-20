// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaceActors.h"
#include "PlaceActorsStyle.h"
#include "PlaceActorsCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationBounds.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/Source/ThirdParty/Python3/Win64/include/Python.h"


static const FName PlaceActorsTabName("PlaceActors");

#define LOCTEXT_NAMESPACE "FPlaceActorsModule"

void FPlaceActorsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPlaceActorsStyle::Initialize();
	FPlaceActorsStyle::ReloadTextures();

	FPlaceActorsCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPlaceActorsCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FPlaceActorsModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPlaceActorsModule::RegisterMenus));
}

void FPlaceActorsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPlaceActorsStyle::Shutdown();

	FPlaceActorsCommands::Unregister();
}

void FPlaceActorsModule::PluginButtonClicked()
{
	FText DialogText = FText::FromString("Actors have been rotated by 90 degrees.");
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	MoveToNearestNavMesh("AI_Char");

}

void FPlaceActorsModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FPlaceActorsCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FPlaceActorsCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}


void FPlaceActorsModule::MoveToNearestNavMesh(const FString& TagToCheck)
{
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
		return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys)
		return;
	APlayerStart* PlayerStart = nullptr;
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		PlayerStart = *It;
		break; // Assume there is only one Player Start actor
	}

	if (!PlayerStart)
		return;

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), Actors);


	for (AActor* Actor : Actors)
	{
		ACharacter* Character = Cast<ACharacter>(Actor);
		if (Character && Character->Tags.Contains(FName(*TagToCheck)))
		{
			FVector StartLocation = Character->GetActorLocation();
			FNavLocation ProjectedLocation;

			// Project the start location to the navigation mesh
			bool bSuccess = NavSys->ProjectPointToNavigation(StartLocation, ProjectedLocation, FVector(2000, 2000, 2000));
			if (bSuccess)
			{
				FVector DirectionToPlayerStart = PlayerStart->GetActorLocation() - StartLocation;
				DirectionToPlayerStart.Z = 0.0f; // Ensure rotation only happens in the horizontal plane

				// Calculate the rotation needed to face the player start
				FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToPlayerStart).Rotator();

				FVector ForwardVector = Character->GetActorForwardVector();
				float ForwardDistance = 20.0f;
				float OffsetAboveGround = 89.0f;
				// Add an offset to the projected location to prevent sinking
				 // Adjust based on capsule height
				FVector Offset = ForwardVector * 60.0f + FVector(0, 0, OffsetAboveGround);
				ProjectedLocation.Location += Offset;

				FHitResult HitResult;
				FVector TraceStart = ProjectedLocation.Location;
				FVector TraceEnd = ProjectedLocation.Location;
				if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic))
				{
					// Adjust the destination point to avoid collision
					ProjectedLocation.Location = HitResult.ImpactPoint;
				}
				// Set the character's location to the adjusted projected location
				Character->SetActorLocationAndRotation(ProjectedLocation.Location, NewRotation);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPlaceActorsModule, PlaceActors)