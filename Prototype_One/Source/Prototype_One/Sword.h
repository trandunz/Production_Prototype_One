#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "Sword.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ASword : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	

	ASword();

protected:
	virtual void BeginPlay() override;
	void UpdateInteractionOutline();
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Interact() override;
	void Unequip();

protected:
	
	
public:
	UPROPERTY(EditAnywhere)
	float Damage{50.0f};
	
	UPROPERTY(EditAnywhere, Category=Interaction)
	float InteractionRange{200.0f};
	
	UPROPERTY(EditAnywhere, Category=Mesh)
	class UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnHit(	UPrimitiveComponent* OverlappedComponent, 
				AActor* OtherActor, 
				UPrimitiveComponent* OtherComp, 
				int32 OtherBodyIndex, 
				bool bFromSweep, 
				const FHitResult &SweepResult);

	bool IsEquiped{};
};
