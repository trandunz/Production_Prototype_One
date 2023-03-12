// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"
#include "Rock.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ARock : public AActor
{
	GENERATED_BODY()
	
public:	
	ARock();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void TakeDamage(int _amount);

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AItem> RockPrefab;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	int CurrentHealth{200};

	UPROPERTY(VisibleAnywhere)
	float RespawnTimer{};

	UPROPERTY(EditAnywhere)
	float RespawnTime{10.0f};

	UPROPERTY(EditAnywhere, Category = "Explosion")
	UNiagaraSystem* Explosion;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComp;

	UFUNCTION(BlueprintImplementableEvent)
	void RockHitEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void RockDestroyedEvent();
};
