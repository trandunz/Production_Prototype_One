
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
	int32 Amount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemDetails Info;
	
	// Static function used to sort Items array
	static bool CompareAscending(const FInventorySlot& InA, const FInventorySlot& InB)
	{
		return InA.Info.Type < InB.Info.Type;
	}
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

	/* Adds an item with the item details passed in */
	UFUNCTION(BlueprintCallable)
	void Pickup(FItemDetails PickedUpItemInfo);

	/* Sells the slot passed in */
	UFUNCTION(BlueprintCallable)
	void Sell(const int32 SlotIndex);

	/* Sorts the Items in the inventory by type */
	UFUNCTION(BlueprintCallable)
	void SortByType();

	UFUNCTION(BlueprintCallable)
	void Drop(const int32 SlotIndex);

	UFUNCTION(BlueprintPure)
	TArray<FInventorySlot> GetItems() { return Items; }
	
	UFUNCTION(BlueprintPure)
	int32 GetCoins() { return Coins; }

	UFUNCTION(BlueprintCallable)
	void PrintInventory();
private:
	/* Sum all items held in inventory */
	void CalculateWeight();
	void AddNewSlot(FItemDetails ItemInfoToAdd);

public:
	TArray<FInventorySlot> Items;
	float MaximumWeight;
	float CurrentWeight;
	int32 Coins;
};