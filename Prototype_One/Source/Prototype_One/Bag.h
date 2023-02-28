#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "Bag.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ABag : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ABag();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Interact() override;

	void UpdateInteractionOutline();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	int CurrentWeight{4};

	UPROPERTY(EditAnywhere)
	int WeightThreshold{5};

	UPROPERTY(EditAnywhere)
	int StoppingThreshold{10};

	UPROPERTY(EditAnywhere)
	bool IsOpen{};

	UPROPERTY(EditAnywhere)
	UStaticMesh* OpenMesh;

	UPROPERTY(EditAnywhere)
	UStaticMesh* ClosedMesh;

	UPROPERTY(EditAnywhere, Category=Interaction)
	float InteractionRange{200.0f};
};
