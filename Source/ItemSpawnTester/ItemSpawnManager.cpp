// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawnManager.h"
#include "Engine/World.h"
#include <AssetRegistryModule.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values for this component's properties
UItemSpawnManager::UItemSpawnManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UItemSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	OwnerOfThisComponent = Cast<AItemSpawner>(GetOwner());

	if (!ItemClassToSpawn)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: %s's ItemClassToSpawn is null"), *OwnerOfThisComponent->GetName(), *GetName());
		return;
	}
	if (!ItemClassToDestroy)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: ItemClassToDestroy is null"), *OwnerOfThisComponent->GetName(), *GetName());
		return;
	}
	GetAllBlueprintSubclasses(ItemClassToSpawn);
}


// Called every frame
void UItemSpawnManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UItemSpawnManager::OnStartSpawning()
{
	if (!ItemClassToSpawn)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: %s's ItemClassToSpawn is null"), *OwnerOfThisComponent->GetName(), *GetName());
		return;
	}
	if (!ItemClassToDestroy)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: ItemClassToDestroy is null"), *OwnerOfThisComponent->GetName(), *GetName());
		return;
	}
	OnReset();
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UItemSpawnManager::InitiateSpawn, SpawnCooldown, true, FirstSpawnDelay);
}

void UItemSpawnManager::OnReset()
{
	//OwnerOfThisComponent->GetTheLIIIIDBack();
	TArray<AActor*> ActorsToDestroy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ItemClassToDestroy, ActorsToDestroy);
	for (int32 i = 0; i < ActorsToDestroy.Num(); i++)
	{
		AItemBase* Item = Cast<AItemBase>(ActorsToDestroy[i]);
		if (Item->bCanBeCarried)
		{
			ActorsToDestroy[i]->Destroy();
		}
	}

	NumOfSpawnedItems = 0;
	NumOfCubeParts = 0;
	NumOfSphereParts = 0;
	NumOfConeParts = 0;
}

//void UItemSpawnManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//	UE_LOG(LogTemp, Warning, TEXT("111"));
//	if (NumOfItemsPerSpawn > SpawnLimitPerType * ItemsToSpawn.Num())
//	{
//		NumOfItemsPerSpawn = SpawnLimitPerType * ItemsToSpawn.Num();
//	}
//}

void UItemSpawnManager::InitiateSpawn()
{
	OnSpawn.Broadcast();

	SpawnItem(OwnerOfThisComponent->GetTransform(), NumOfItemsPerSpawn);
	for (int32 i = 0; i < SpawnedItems.Num(); i++)
	{
		LaunchItem(SpawnedItems[i], MinSpreadDegree, MaxSpreadDegree, SpawnShootForce);
	}
}

TArray<AItemBase*> UItemSpawnManager::SpawnItem(FTransform _SpawnTransform, int32 _NumOfItemsPerSpawn)
{
	SpawnedItems.Empty();

	if (NumOfSphereParts == SpawnLimitPerType && NumOfConeParts == SpawnLimitPerType && NumOfCubeParts == SpawnLimitPerType)
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		OnSpawnFinish.Broadcast();
		return SpawnedItems;
	}

	for (int32 i = 0; i < _NumOfItemsPerSpawn; i++)
	{
		int32 RandomInt = FMath::RandRange(0, ItemsToSpawn.Num() - 1);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerOfThisComponent;
		SpawnParams.Instigator = OwnerOfThisComponent->GetInstigator();

		AItemBase* Item = GetWorld()->SpawnActor<AItemBase>(
			ItemsToSpawn[RandomInt],
			_SpawnTransform.GetLocation() + (_SpawnTransform.GetRotation().GetUpVector() * SpawnOffset_Z),
			_SpawnTransform.GetRotation().Rotator(),
			SpawnParams
			);

		if (Item->ActorHasTag("Sphere"))
		{
			NumOfSphereParts++;
			NumOfSpawnedItems++;
		}
		else if (Item->ActorHasTag("Cone"))
		{
			NumOfConeParts++;
			NumOfSpawnedItems++;
		}
		else if (Item->ActorHasTag("Cube"))
		{
			NumOfCubeParts++;
			NumOfSpawnedItems++;
		}

		if (NumOfSphereParts > SpawnLimitPerType)
		{
			Item->Destroy();
			NumOfSphereParts--;
			NumOfSpawnedItems--;
			i--;
			continue;
		}
		else if (NumOfConeParts > SpawnLimitPerType)
		{
			Item->Destroy();
			NumOfConeParts--;
			NumOfSpawnedItems--;
			i--;
			continue;
		}
		else if (NumOfCubeParts > SpawnLimitPerType)
		{
			Item->Destroy();
			NumOfCubeParts--;
			NumOfSpawnedItems--;
			i--;
			continue;
		}

		Item->CurrentScale = Item->MinimizeScale(Item->DefaultScale * Item->MinimizeMultiply);

		SpawnedItems.Add(Item);
		OnCurrentSpawnedItem.Broadcast();

		if (NumOfSphereParts == SpawnLimitPerType && NumOfConeParts == SpawnLimitPerType && NumOfCubeParts == SpawnLimitPerType)
		{
			break;
		}
	}

	OnCurrentSpawnFinished.Broadcast();
	return SpawnedItems;
}

void UItemSpawnManager::LaunchItem(AItemBase* _ItemToLaunch, float _MinSpreadDegree, float _MaxSpreadDegree, float _SpawnShootForce)
{
	FVector RandomVector;
	float Tolerance = _MinSpreadDegree / 100.f;
	do
	{
		RandomVector = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(OwnerOfThisComponent->GetActorUpVector(), _MaxSpreadDegree);
	} while (RandomVector.Equals(OwnerOfThisComponent->GetActorUpVector(), Tolerance));

	FVector Force = RandomVector * _SpawnShootForce;
	_ItemToLaunch->GetMeshComponent()->AddForce(Force);
	RandomVector = FVector::ZeroVector;

	//	OwnerOfThisComponent->OnEachItemLaunch(_ItemToLaunch);
}

void UItemSpawnManager::GetAllBlueprintSubclasses(TSubclassOf<UObject> _Base)
{
	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	// The asset registry is populated asynchronously at startup, so there's no guarantee it has finished.
	// This simple approach just runs a synchronous scan on the entire content directory.
	// Better solutions would be to specify only the path to where the relevant blueprints are,
	// or to register a callback with the asset registry to be notified of when it's finished populating.
	TArray< FString > ContentPaths;
	ContentPaths.Add(TEXT("/Game"));
	AssetRegistry.ScanPathsSynchronous(ContentPaths);

	FName BaseClassName = _Base->GetFName();

	// Use the asset registry to get the set of all class names deriving from Base
	TSet< FName > DerivedNames;
	{
		TArray< FName > BaseNames;
		BaseNames.Add(BaseClassName);

		TSet< FName > Excluded;
		AssetRegistry.GetDerivedClassNames(BaseNames, Excluded, DerivedNames);
	}

	FARFilter Filter;
	Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	Filter.bRecursiveClasses = true;
	//if (!Path.IsEmpty())
	//{
	//	Filter.PackagePaths.Add(*Path);
	//}
	Filter.bRecursivePaths = true;

	TArray< FAssetData > AssetList;
	AssetRegistry.GetAssets(Filter, AssetList);

	// Iterate over retrieved blueprint assets
	for (auto const& Asset : AssetList)
	{
		// Get the the class this blueprint generates (this is stored as a full path)
		if (auto GeneratedClassPathPtr = Asset.TagsAndValues.Find(TEXT("GeneratedClass")))
		{
			// Convert path to just the name part
			const FString ClassObjectPath = FPackageName::ExportTextPathToObjectPath(*GeneratedClassPathPtr);
			const FString ClassName = FPackageName::ObjectPathToObjectName(ClassObjectPath);

			// Check if this class is in the derived set
			if (!DerivedNames.Contains(*ClassName))
			{
				continue;
			}

			// Check if this is "ItemBase". we don't want to spawn the base class
			if (ClassName.Contains("Base"))
			{
				continue;
			}

			// Store using the path to the generated class
			UClass* Item = UKismetSystemLibrary::LoadClassAsset_Blocking(TAssetSubclassOf<UObject>(FStringAssetReference(ClassObjectPath)));
			if (Item)
			{
				ItemsToSpawn.Add(Item);
			}
		}
	}
}