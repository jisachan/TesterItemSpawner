// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemBase.h"
#include <Array.h>
#include <TimerManager.h>
#include "ItemSpawner.h"
//#include "MyGameStateBase.h"

#include "ItemSpawnManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ITEMSPAWNTESTER_API UItemSpawnManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemSpawnManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//AMyGameStateBase* GameState = nullptr;
	
	FTimerHandle SpawnTimerHandle;
	//FTimerHandle ScaleTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Spawn Manager");
	TArray<AItemBase*> SpawnedItems;

	UPROPERTY(BlueprintReadOnly, Category = "Spawn Manager")
	int32 NumOfSphereParts = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Spawn Manager")
	int32 NumOfConeParts = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Spawn Manager")
	int32 NumOfCubeParts = 0;

	AItemSpawner* OwnerOfThisComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		TArray<UClass*> ItemsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		TSubclassOf<UObject> ItemClassToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		TSubclassOf<AItemBase> ItemClassToDestroy;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager", meta = (ToolTip = "Number of items to spawn at a time.", ClampMin = "1", ClampMax = "3"))
		int32 NumOfItemsPerSpawn = 1;	
		
	UPROPERTY(BlueprintReadOnly, Category = "Spawn Manager", meta = (ToolTip = "Total number of items spawned."))
		int32 NumOfSpawnedItems;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager", meta = (ToolTip = "Limitating spawn amount. (Need to set up tags for this)."))
		int32 SpawnLimitPerType = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		float SpawnCooldown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		float FirstSpawnDelay = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		float MinSpreadDegree = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		float MaxSpreadDegree = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		float SpawnShootForce = 11515111.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Manager")
		float SpawnOffset_Z = 200.f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void OnStartSpawning();

	UFUNCTION()
		void InitiateSpawn();

	UFUNCTION(BlueprintCallable)
		TArray<AItemBase*> SpawnItem(FTransform _SpawnTransform, int32 _NumOfItemsToSpawn);

	UFUNCTION(BlueprintCallable)
		void LaunchItem(AItemBase* _ItemToLaunch, float _MinSpreadDegree, float _MaxSpreadDegree, float _SpawnShootForce);

	UFUNCTION(BlueprintCallable)
		void GetAllBlueprintSubclasses(TSubclassOf<UObject> _Base);

	UFUNCTION(BlueprintCallable)
		void OnReset();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnDelegate);
	UPROPERTY(BlueprintAssignable, Category = "Spawn Manager")
		FSpawnDelegate OnSpawn;	

	UPROPERTY(BlueprintAssignable, Category = "Spawn Manager")
		FSpawnDelegate OnSpawnFinish;	
	
	UPROPERTY(BlueprintAssignable, Category = "Spawn Manager")
		FSpawnDelegate OnCurrentSpawnedItem;
	
	UPROPERTY(BlueprintAssignable, Category = "Spawn Manager")
		FSpawnDelegate OnCurrentSpawnFinished;

	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
