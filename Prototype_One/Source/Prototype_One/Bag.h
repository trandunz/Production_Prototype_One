#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "Bag.generated.h"

UENUM()
enum class MOVEMENTSTATE
{
	FLYING,
	DRAGGING,
	FROZEN,
	FROZEN_GROUND
};


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
	void AttractItems(float DeltaTime);
	void SpawnEnemies(float DeltaTime);
	void SpawnSmallItems(float DeltaTime);
	void HandleBehaviorBasedOnWeight(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	int GetWeight();
	void FlapWings();
	MOVEMENTSTATE GetMovementState();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PickupRange = 40.0f;
	
	UPROPERTY(EditAnywhere)
	int SuctionRadius{200};
	
	UPROPERTY(EditAnywhere,	BlueprintReadOnly)
	int PlayerWeightThreshold{80};

	UPROPERTY(EditAnywhere,	BlueprintReadOnly)
	int StoppingThreshold{100};

	UPROPERTY(EditAnywhere,	BlueprintReadOnly)
	bool IsOpen{};

	UPROPERTY(VisibleAnywhere)
	float SmallItemSpawnTimer{};
	UPROPERTY(EditAnywhere)
	float SmallItemSpawnInterval{5};

	UPROPERTY(EditAnywhere)
	float DistanceFromNPC{1000};

	UPROPERTY(EditAnywhere)
	UStaticMesh* OpenMesh;

	UPROPERTY(EditAnywhere)
	UStaticMesh* ClosedMesh;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WingRightMesh;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WingLeftMesh;

	UPROPERTY(EditAnywhere)
	bool IsBiengPulled{true};

	UPROPERTY(EditAnywhere, Category=Interaction)
	float InteractionRange{200.0f};
	UPROPERTY(EditAnywhere)
	bool IsWithinInteractionRange{false};

	UPROPERTY(EditAnywhere)
	float BagSpeed = 2.0f;

	UPROPERTY(EditAnywhere)
	float SpawnRadius = 1500.0f;

	UPROPERTY(EditAnywhere, Category=EnemySpawning)
	TSubclassOf<AActor> RabbitPrefab;

	UPROPERTY(EditAnywhere, Category=EnemySpawning)
	TSubclassOf<AActor> MaskedPrefab;

	UPROPERTY(EditAnywhere, Category=EnemySpawning)
	TSubclassOf<AActor> KingPrefab;

	UPROPERTY(EditAnywhere, Category= ItemPrefabs)
	TSubclassOf<AActor> CarrotPrefab;

	UPROPERTY(EditAnywhere, Category= ItemPrefabs)
	TSubclassOf<AActor> PebblePrefab;

	UPROPERTY(EditAnywhere, Category= ItemPrefabs)
	TSubclassOf<AActor> StickPrefab;
	
	UPROPERTY(EditAnywhere, Category=Rope)
	class ARope* Rope;



	UPROPERTY(EditAnywhere, Category=Rope)
	TSubclassOf<ARope> RopePrefab;

	UPROPERTY(EditAnywhere, Category=Rope)
	class UCableComponent* CableComponent;
protected:
	float EnemySpawnTimer{};
	float EnemySpawnTimerLength = 10.0f;

	APrototype_OneCharacter* Player;
};
