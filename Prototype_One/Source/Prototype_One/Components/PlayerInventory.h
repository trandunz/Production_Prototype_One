
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

	/* Drop the items in SlotIndex passed in */
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

	/* Calculate how many enemies to spawn */
	int32 GetRabbitSpawnCount();
	int32 GetMaskedSpawnCount();
	int32 GetKingSpawnCount();

	/* Cache items in bag in separate array */
	void BagDropped();
	/* Restore items to bag */
	void BagReturned();
	/* Delete cached items */
	void BagLost();

	/* Allows items to be sold if true */
	UFUNCTION(BlueprintCallable)
	void SetIsShopping(bool IsShopping){ bIsShopping = IsShopping; }
	
	UFUNCTION(BlueprintCallable)
	void PrintInventory();
private:
	/* Sum all items held in inventory */
	void CalculateWeight();
	/* Add a new slot to the inventory with the itemdetails passed in */
	void AddNewSlot(FItemDetails ItemInfoToAdd);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlotModifiedDelegate, int32, SlotIndex, int32, Amount);
	UPROPERTY(BlueprintAssignable)
	FSlotModifiedDelegate OnSlotModified;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewSlotDelegate, int32, SlotIndex, FInventorySlot, InventorySlot);
	UPROPERTY(BlueprintAssignable)
	FNewSlotDelegate OnNewSlot;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateCoinsDelegate);
	UPROPERTY(BlueprintAssignable)
	FUpdateCoinsDelegate OnUpdateCoins;

private:
	TArray<FInventorySlot> Items;
	TArray<FInventorySlot> DroppedItems;
	float MaximumWeight;
	float CurrentWeight;
	int32 Coins;
	bool bIsShopping;
};
