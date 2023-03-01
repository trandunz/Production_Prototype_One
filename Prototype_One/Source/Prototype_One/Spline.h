// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spline.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ASpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpline();

	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class USplineComponent* Spline;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* Mesh;
};
