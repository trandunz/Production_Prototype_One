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
	void AttractItems();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	float Dt{};
	
	UPROPERTY(VisibleAnywhere)
	int MeatCount{0};
	UPROPERTY(VisibleAnywhere)
	int AntlerCount{0};
	UPROPERTY(VisibleAnywhere)
	int MaskCount{0};
	UPROPERTY(VisibleAnywhere)
	int CrownCount{0};
	
	UPROPERTY(EditAnywhere)
	int SuctionRadius{200};
	
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

	UPROPERTY(EditAnywhere, Category=Rope)
	class ARope* Rope;

	UPROPERTY(EditAnywhere, Category=Rope)
	TSubclassOf<ARope> RopePrefab;

	UPROPERTY(EditAnywhere, Category=Rope)
	class UCableComponent* CableComponent;
protected:
	float SpawnTimer{};

	APrototype_OneCharacter* Player;
};
