#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tree.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ATree : public AActor
{
	GENERATED_BODY()
	
public:	
	ATree();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Stump;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Trunk;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Leaves;
};
