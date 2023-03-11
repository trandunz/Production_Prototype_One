#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Prototype_One/Sword.h"
#include "Prototype_One/Widgets/PauseMenuWidget.h"
#include "Components/SphereComponent.h"
#include "Prototype_OneCharacter.generated.h"

class UPlayerInventory;
class UPlayerHUD;
class HealthPotion;
UCLASS(config=Game)
class APrototype_OneCharacter : public ACharacter
{
	GENERATED_BODY()
	
private:
	enum class FadeState
	{
		FadeIn,
		FadeOut,
		StopFade,
	};
	FadeState fadeState = FadeState::StopFade;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* OpenBagAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PauseAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* UseHealthPotionAction;
	
	HealthPotion* healthPotion = nullptr;
	
public:
	APrototype_OneCharacter();
	
protected:
	void Move(const FInputActionValue& Value);
	void StartSprint();
	void EndSprint();
	void TryDash(); 
	void TryMelee();
	void StartAim();
	void EndAim();
	void Look(const FInputActionValue& Value);
	void ScrollZoom(const FInputActionValue& Value);
	void TryInteract();
	void ToggleDebugMenu();
	void InteractRaycast();
	void PauseGame();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	void InitInputMappingContext();
	void InitGUI();

	void Ragdoll();
	void UnRagdoll();

	void UpdateFadeActors();
	void SetShowMeshes();
	void SetHiddenMeshes();

	void LookAtCursor();
	void TryOpenBag();
public:
	UFUNCTION(BlueprintCallable)
	void TakeDamage(int _amount);
	void RecoverHealth(int _amount);
	void RecoverMana(int _amount);
	void UseMana(int _amount);
	void PlayerRespawn();
	void RegenHealth();

	UFUNCTION(BlueprintCallable)
	void Heal();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	UPauseMenuWidget* PauseMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	AActor* CurrentlyHeldActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AttackStencilCollider;
	
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DesiredSpeed{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool CanSprint{true};

	// Dash related
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashMovementMaxTime{0.5f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashMovementCurrentTime{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool IsDashing{false};
	bool HasStartedDash{false};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashDistance{5000.0f};
	FVector2D DashMovementVector;
	FVector DashForwardDirection;
	FVector DashRightDirection;

	// Aiming related - player looking towards mouse position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool IsAiming{false};
	
	// Combat related variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float combatMovementMaxTime{2.0f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float combatMovementCurrentTime{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool IsAttacking{false};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AttackMovementAmount{2000.0f};

	bool IsShopping{};
	float CameraShopLerpRatio{};

	// Player Death/Respawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Respawn, meta = (AllowPrivateAccess = "true"))
	bool IsRespawning{false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Respawn, meta = (AllowPrivateAccess = "true"))
	bool HasRespawnedOnce{false};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Respawn, meta = (AllowPrivateAccess = "true"))
	float TimeRespawnPauseMovement{4.0f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Respawn, meta = (AllowPrivateAccess = "true"))
	float PauseMovementTimer{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Respawn, meta = (AllowPrivateAccess = "true"))
	float TimeBeforeRespawn{2.0f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Respawn, meta = (AllowPrivateAccess = "true"))
	float RespawnTimer{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Respawn, meta = (AllowPrivateAccess = "true"))
	FVector StartLocation{};
	
	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UPlayerInventory* PlayerInventory;
	
	// Stats 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
    class URPGEntityComponent* EntityComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float DistanceToMole{1000.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float MaxTimeUntilHealthRegen{0.05};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float HealthRegenTimer{};
	
	
	// Related to disappearing objects
	UPROPERTY(VisibleAnywhere)
	int ValuablesCount{1};

	UPROPERTY(EditAnywhere, Category=Camera)
	TArray<class UStaticMeshComponent*> CameraHitMeshes;
	UPROPERTY(EditAnywhere, Category=Camera)
	TArray<class UStaticMeshComponent*> HiddenMeshes;

	// Delegates
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackDelegate);
	UPROPERTY(BlueprintAssignable)
	FAttackDelegate OnAttack;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashDelegate);
	UPROPERTY(BlueprintAssignable)
	FDashDelegate OnDash;

	// Blueprint implementable events for audio triggering
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerHitEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void OnDashEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void OnBagOpenOrClose();
	UFUNCTION(BlueprintImplementableEvent)
	void OnDrinkPotionEvent();
	
	// Prefabs
protected:
	//TSubclassOf<AActor> SomePrefab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerHUD> PlayerHudPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPauseMenuWidget> PauseMenuPrefab;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<ASword> SwordPrefab;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	//UAnimMontage* DashAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MeleeAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashAnimation;

	UPROPERTY(EditAnywhere)
	class USceneComponent* ShopCameraLerpPoint{};
private:
	bool bHasResetStaminaDelay;
};

