#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bush.generated.h"

class AItem;
UCLASS()
class PROTOTYPE_ONE_API ABush : public AActor
{
	GENERATED_BODY()
	
public:	
	ABush();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void TakeDamage(int _amount);

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Carrot1;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Carrot2;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Carrot3;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Carrot4;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AItem> CarrotPrefab;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AItem> StickPrefab;

	UPROPERTY(VisibleAnywhere)
	int CurrentHealth{100};

	UPROPERTY(VisibleAnywhere)
	float RespawnTimer{};

	UPROPERTY(EditAnywhere)
	float RespawnTime{10.0f};
};
