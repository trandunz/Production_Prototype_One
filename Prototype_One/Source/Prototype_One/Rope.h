// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableActor.h"
#include "Rope.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_ONE_API ARope : public ACableActor
{
	GENERATED_BODY()

	ARope();
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category=Rope)
	class UPhysicsConstraintComponent* Constraint;
};
