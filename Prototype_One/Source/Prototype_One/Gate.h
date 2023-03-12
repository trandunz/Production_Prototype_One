#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "Gate.generated.h"

UCLASS()
class PROTOTYPE_ONE_API AGate : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	AGate();

protected:
	virtual void BeginPlay() override;

	void UpdateInteractionOutline();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Interact() override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ArchMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* GateMesh;
};
