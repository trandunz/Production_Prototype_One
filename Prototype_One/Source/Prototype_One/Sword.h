#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sword.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ASword : public AActor
{
	GENERATED_BODY()
	
public:	

	ASword();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category=Mesh)
	class UStaticMeshComponent* Mesh;
};
