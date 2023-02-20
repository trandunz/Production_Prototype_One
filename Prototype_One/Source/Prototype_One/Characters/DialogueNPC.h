#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DialogueNPC.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ADialogueNPC : public ACharacter
{
	GENERATED_BODY()

public:
	ADialogueNPC();

protected:
	virtual void BeginPlay() override;
	
	void UpdateInteractionOutline();
	void UpdateInteractionText();
public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditAnywhere, Category=Interaction)
	float InteractionRange{200.0f};
};
