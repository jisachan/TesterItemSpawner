// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ItemBase.generated.h"

UCLASS()
class ITEMSPAWNTESTER_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		UStaticMeshComponent* MeshComp;
	UPROPERTY()
		FVector DefaultScale;
	UPROPERTY()
		FVector CurrentScale;

	UPROPERTY(EditDefaultsOnly)
		float MinimizeMultiply = .1f;

	UPROPERTY(EditDefaultsOnly)
		float ScaleUpSpeed = .05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bCanBeCarried = true;

	UFUNCTION()
		FVector MinimizeScale(FVector MinimizeScale);

	UFUNCTION()
		UStaticMeshComponent* GetMeshComponent();

	UFUNCTION()
		bool ScaleCompare(FVector V1, FVector V2);

};
