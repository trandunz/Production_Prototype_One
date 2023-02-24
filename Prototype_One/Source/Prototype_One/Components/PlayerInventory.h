
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prototype_One/Item.h"
#include "PlayerInventory.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemDetails Info;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_ONE_API UPlayerInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Attempts to pickup an item, returns true if there was enough space in inventory */
	bool Pickup(FItemDetails ItemDetails, int32 Amount);

	/* Sum all items held in inventory */
	void CalculateWeight();
	
public:
	int32 MaximumSlots;
	TArray<FInventorySlot> Items;
	float MaximumWeight;
	float CurrentWeight;
};
