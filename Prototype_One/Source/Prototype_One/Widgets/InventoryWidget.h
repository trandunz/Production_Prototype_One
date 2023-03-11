// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_ONE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* RabbitCount;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* MaskedCount;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* KingCount;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
