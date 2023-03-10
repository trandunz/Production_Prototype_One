// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

/* Enum for all items type */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Pebble,
	Stick,
	Carrot,
	Meat,
	Antler,
	Mask,
	Crown,
	Potion,
	DEFAULT
};


/* Struct for holding details for items (type, rarity, value)*/
USTRUCT(BlueprintType)
struct FItemDetails
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType Type = EItemType::DEFAULT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaximumStackSize = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Weight = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* IconTexture = nullptr;
};

UCLASS()
class PROTOTYPE_ONE_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FItemDetails ItemDetails;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	bool IsPickupable{};
};
