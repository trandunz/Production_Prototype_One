
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

	UFUNCTION(BlueprintCallable)
	void SellAll();

	/* Sorts the Items in the inventory by type */
	UFUNCTION(BlueprintCallable)
	void SortByType();

	UFUNCTION(BlueprintCallable)
	void Drop(const int32 SlotIndex);

	UFUNCTION(BlueprintPure)
	TArray<FInventorySlot> GetItems() { return Items; }
	
	UFUNCTION(BlueprintPure)
	int32 GetCoins() { return Coins; }

	/* When player buys something, this function takes the amount from coins */
	UFUNCTION(BlueprintCallable)
	void SubtractCoins(int32 AmountToSubtract);

	/* Function to call OnUpdateCoins delegate */
	UFUNCTION(BlueprintCallable)
	void UpdateCoins();

	/* Getter for CurrentWeight */
	UFUNCTION(BlueprintCallable)
	float GetWeight(){ return CurrentWeight; }

	int32 GetRabbitSpawnCount();
	int32 GetMaskedSpawnCount();
	int32 GetKingSpawnCount();
	
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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlotModifiedDelegate, int32, SlotIndex, int32, Amount);
	UPROPERTY(BlueprintAssignable)
	FSlotModifiedDelegate OnSlotModified;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewSlotDelegate, int32, SlotIndex, FInventorySlot, InventorySlot);
	UPROPERTY(BlueprintAssignable)
	FNewSlotDelegate OnNewSlot;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateCoinsDelegate);
	UPROPERTY(BlueprintAssignable)
	FUpdateCoinsDelegate OnUpdateCoins;
};
