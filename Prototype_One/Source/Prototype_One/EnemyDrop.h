// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "EnemyDrop.generated.h"

UCLASS()
class PROTOTYPE_ONE_API AEnemyDrop : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyDrop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Interact() override;

	UPROPERTY(EditAnywhere)
	class UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnHit(	UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, 
			bool bFromSweep, 
			const FHitResult &SweepResult);
};
