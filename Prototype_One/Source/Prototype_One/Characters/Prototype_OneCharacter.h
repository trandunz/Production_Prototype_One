#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Prototype_OneCharacter.generated.h"

class UPlayerHUD;
UCLASS(config=Game)
class APrototype_OneCharacter : public ACharacter
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MeleeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* ScrollZoomAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ToggleDebugAction;
	
public:
	APrototype_OneCharacter();
	
protected:
	void Move(const FInputActionValue& Value);
	void StartSprint();
	void EndSprint();
	void TryRoll();
	void TryMelee();
	void Look(const FInputActionValue& Value);
	void ScrollZoom(const FInputActionValue& Value);
	void TryInteract();
	void ToggleDebugMenu();
	void InteractRaycast();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	void InitInputMappingContext();
	void InitGUI();

	void Ragdoll();

	void UpdateFadeActors();
	void SetShowMeshes();
	void SetHiddenMeshes();

public:
	void TakeDamage(int _amount);
	
	// References
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	FHitResult LastHitResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	UPlayerHUD* PlayerHud;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	AActor* CurrentlyHeldActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	int LargestZoomDistance{800};
	float ZoomRatio = 0.0f;
	float Dt{};
	bool IsTalking{};

	// Stamina/sprint related
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	int JogSpeed{400};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	int SprintSpeed{800};

	// Dodge roll related
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float dodgeMovementMaxTime{0.5f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float dodgeMovementCurrentTime{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool IsDodging{false};
	bool HasStartedDodge{false};
	FVector2D DodgeMovementVector;
	FVector DodgeForwardDirection;
	FVector DodgeRightDirection; 
	
	// Stats 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    class URPGEntityComponent* EntityComponent;

	// Combat related variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float combatMovementMaxTime{0.5f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float combatMovementCurrentTime{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool IsAttacking{false};

	UPROPERTY(VisibleAnywhere)
	int ValuablesCount{1};

	UPROPERTY(EditAnywhere, Category=Camera)
	TArray<class UStaticMeshComponent*> CameraHitMeshes;
	UPROPERTY(EditAnywhere, Category=Camera)
	TArray<class UStaticMeshComponent*> HiddenMeshes;

	// Prefabs
protected:
	//TSubclassOf<AActor> SomePrefab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerHUD> PlayerHudPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MeleeAnimation;
};

