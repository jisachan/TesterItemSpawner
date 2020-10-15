// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"

// Sets default values
AItemBase::AItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	MeshComp = FindComponentByClass<UStaticMeshComponent>();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("%s does not have a StaticMashComponent!"), *GetName());
	}

	DefaultScale = FVector(1.f, 1.f, 1.f);
	CurrentScale = DefaultScale;

}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!CurrentScale.Equals(DefaultScale, .1f) && ScaleCompare(CurrentScale, DefaultScale))
	{
		CurrentScale = MeshComp->GetComponentScale();

		MeshComp->SetRelativeScale3D(FMath::Lerp(CurrentScale, DefaultScale, ScaleUpSpeed));
	}

}

FVector AItemBase::MinimizeScale(FVector MinimizeScale)
{
	MeshComp->SetRelativeScale3D(MinimizeScale);
	return MeshComp->GetComponentScale();
}

UStaticMeshComponent* AItemBase::GetMeshComponent()
{
	return MeshComp;
}

bool AItemBase::ScaleCompare(FVector V1, FVector V2)
{
	return V1.X < V2.X && V1.Y < V2.Y && V1.Z < V2.Z;
}

