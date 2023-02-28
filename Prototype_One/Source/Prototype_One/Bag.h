#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bag.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ABag : public AActor
{
	GENERATED_BODY()
	
public:	
	ABag();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	int CurrentWeight{4};

	UPROPERTY(EditAnywhere)
	int WeightThreshold{5};

	UPROPERTY(EditAnywhere)
	int StoppingThreshold{10};

	UPROPERTY(EditAnywhere)
	bool IsOpen{};

	
};
