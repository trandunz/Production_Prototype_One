#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "Bag.generated.h"

class APrototype_OneCharacter;
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
	int WeightThreshold{5};

	UPROPERTY(EditAnywhere)
	int StoppingThreshold{10};

	UPROPERTY(EditAnywhere)
	bool IsOpen{};

	UPROPERTY(EditAnywhere)
	bool IsDropped{};

	UPROPERTY(EditAnywhere)
	UStaticMesh* OpenMesh;

	UPROPERTY(EditAnywhere)
	UStaticMesh* ClosedMesh;

	UPROPERTY(EditAnywhere)
	bool IsBiengPulled{};

	UPROPERTY(EditAnywhere, Category=Interaction)
	float InteractionRange{200.0f};

	UPROPERTY(EditAnywhere, Category=EnemySpawning)
	TSubclassOf<AActor> RabbitPrefab;

	UPROPERTY(EditAnywhere, Category=EnemySpawning)
	TSubclassOf<AActor> MaskedPrefab;

	UPROPERTY(EditAnywhere, Category=EnemySpawning)
	TSubclassOf<AActor> KingPrefab;
protected:
	float SpawnTimer{};

	APrototype_OneCharacter* Player;
};
