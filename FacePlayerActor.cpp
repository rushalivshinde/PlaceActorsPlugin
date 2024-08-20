// Fill out your copyright notice in the Description page of Project Settings.


#include "FacePlayerActor.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFacePlayerActor::AFacePlayerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFacePlayerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFacePlayerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (USceneComponent* RootComp = GetRootComponent()) {
		if (APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			RootComp->SetWorldRotation((RootComp->GetComponentLocation() - LocalPawn->GetActorLocation()).GetSafeNormal().Rotation());
		}
	}


}

