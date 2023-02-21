#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Prototype_One/Interfaces/InteractInterface.h"
#include "DialogueNPC.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ADialogueNPC : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	ADialogueNPC();

protected:
	virtual void BeginPlay() override;
	
	void UpdateInteractionOutline();
public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void Interact() override;
public:
	UPROPERTY(EditAnywhere, Category=Interaction)
	float InteractionRange{200.0f};
};
