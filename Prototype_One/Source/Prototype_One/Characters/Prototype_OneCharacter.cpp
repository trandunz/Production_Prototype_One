#include "Prototype_OneCharacter.h"

#include "AITypes.h"
#include "CableComponent.h"
#include "DialogueNPC.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PrototypeEnemy.h"
#include "Prototype_One/Widgets/PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prototype_One/Bag.h"
#include "Prototype_One/Gamemodes/Prototype_OneGameMode.h"
#include "Prototype_One/Sword.h"
#include "Prototype_One/Components/FadeComponent.h"
#include "Prototype_One/Components/PlayerInventory.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"
#include "Prototype_One/HealthPotion.h"
#include "Prototype_One/Tree.h"

APrototype_OneCharacter::APrototype_OneCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f;	
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->SetRelativeRotation({0,-50,0});

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	EntityComponent = CreateDefaultSubobject<URPGEntityComponent>(TEXT("Entity Component"));
	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("Inventory"));

	AttackStencilCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Stenciling"));
	AttackStencilCollider->SetupAttachment(RootComponent); // not working?
	AttackStencilCollider->SetCollisionProfileName("Trigger");

	healthPotion = new HealthPotion();

	ShopCameraLerpPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ShopCameraLerpPoint"));
	
}

void APrototype_OneCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitGUI();
	CameraBoom->TargetArmLength = FMath::Lerp(LargestZoomDistance, 300,ZoomRatio );
	if (PlayerHud)
	{
		PlayerHud->UpdateHealth(EntityComponent->Properties.CurrentHealth, EntityComponent->Properties.MaxHealth);
		PlayerHud->UpdateStamina(EntityComponent->Properties.CurrentStamina, EntityComponent->Properties.MaxStamina);
	}

	EndSprint();

	InitInputMappingContext();

	// Spawning sword in hand
	if (SwordPrefab)
	{
		if (auto* newSword = GetWorld()->SpawnActor(SwordPrefab))
		{
			Cast<ASword>(newSword)->Interact();
		}
	}

	// Respawn
	RespawnTimer = TimeBeforeRespawn;
	PauseMovementTimer = TimeRespawnPauseMovement;
	StartLocation = GetActorLocation();

	// Combat
	AttackStencilCollider->OnComponentBeginOverlap.AddDynamic(this, &APrototype_OneCharacter::OnBeginOverlap);
	AttackStencilCollider->OnComponentEndOverlap.AddDynamic(this, &APrototype_OneCharacter::OnOverlapEnd);

	// Health regen
	HealthRegenTimer = MaxTimeUntilHealthRegen;
	PreviousArmLength = CameraBoom->TargetArmLength;

	if (Trail) {
		// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Trail, GetMesh(), FName("Base-HumanTail8"), FVector(0.f), FRotator(0.f), EAttachLocation::SnapToTarget, true);
		NiagaraComp->SetPaused(false);
		NiagaraComp->SetVisibility(false);
	}
	if (HealPuff) {
		// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
		NiagaraHealComp = UNiagaraFunctionLibrary::SpawnSystemAttached(HealPuff, GetMesh(), FName("Base-HumanTail8"), FVector(0.f), FRotator(0.f), EAttachLocation::SnapToTarget, true);
		NiagaraHealComp->SetPaused(false);
		NiagaraHealComp->SetVisibility(false);
	}
}

void APrototype_OneCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (PlayerHud)
	{
		PlayerHud->RemoveFromParent();
		PlayerHud = nullptr;
	}
}

void APrototype_OneCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Dt = DeltaSeconds;
	InteractRaycast();

	// Timer for dodging
	if (DashMovementCurrentTime > 0)
		DashMovementCurrentTime -= DeltaSeconds;
	if (DashMovementCurrentTime <= 0)
	{
		IsDashing = false;
		if (NiagaraComp)
			NiagaraComp->SetVisibility(false);
	}
		
	// Dodging
	if (IsDashing == true)
	{
		GetCharacterMovement()->Velocity.Set(DashMovementVector.Y * DashDistance, DashMovementVector.X * DashDistance, GetCharacterMovement()->Velocity.Z);
	}
	
	// Timer for combat
	if (combatMovementCurrentTime > 0)
		combatMovementCurrentTime -= DeltaSeconds;
	if (combatMovementCurrentTime <= 0)
		IsAttacking = false;

	// Sprint related
	if (EntityComponent->Properties.CurrentStamina <= 0)
		EndSprint();

	PlayerRespawn();
	
	UpdateFadeActors();
	SetShowMeshes();
	SetHiddenMeshes();
	RegenHealth();
	

	if (PlayerHud)
	{
		PlayerHud->UpdateSneakStatus(2);
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototypeEnemy::StaticClass(), actors);
		bool seen{};
		bool anyCanSeePlayer{};
		for(auto enemyActor : actors)
		{
			if (auto* enemy = Cast<APrototypeEnemy>(enemyActor))
			{
				if (auto* enemyController = Cast<AEnemyController>(enemy->Controller))
				{
					
					if (enemyController->CanSeePlayer)
					{
						anyCanSeePlayer = true;
					}
					if (enemyController->BlackboardComponent->GetValueAsBool(FName("CanSeePlayer")))
					{
						seen = true;
					}
					
					
				}
			}
		}
		
		if (anyCanSeePlayer && !seen)
		{
			PlayerHud->UpdateSneakStatus(1);
		}
		else if (seen)
		{
			PlayerHud->UpdateSneakStatus(0);
		}
		else if (!seen && !anyCanSeePlayer)
		{
			PlayerHud->UpdateSneakStatus(2);
		}
	}

	if (IsShopping)
	{
		if (CameraShopLerpRatio < 1)
		{
			CameraShopLerpRatio += DeltaSeconds;
			if (FollowCamera && ShopCameraLerpPoint)
			{
				FollowCamera->SetRelativeLocation(FMath::Lerp(FollowCamera->GetRelativeLocation(), ShopCameraLerpPoint->GetRelativeLocation(), CameraShopLerpRatio));
				FollowCamera->SetRelativeRotation(FMath::Lerp(FollowCamera->GetRelativeRotation(), ShopCameraLerpPoint->GetRelativeRotation(), CameraShopLerpRatio));
				CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, 200.0f, CameraShopLerpRatio);
			}
				
		}
	}
	else
	{
		if (CameraShopLerpRatio > 0)
		{
			CameraShopLerpRatio -= DeltaSeconds;
			if (FollowCamera)
			{
				FollowCamera->SetRelativeLocation(FMath::Lerp({},FollowCamera->GetRelativeLocation(), CameraShopLerpRatio));
				FollowCamera->SetRelativeRotation(FMath::Lerp({},FollowCamera->GetRelativeRotation(), CameraShopLerpRatio));
				
				CameraBoom->TargetArmLength = FMath::Lerp(PreviousArmLength, CameraBoom->TargetArmLength, CameraShopLerpRatio);
			}
				
		}
	}
}

void APrototype_OneCharacter::InitInputMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APrototype_OneCharacter::InitGUI()
{
	if (!PlayerHud && PlayerHudPrefab)
	{
		PlayerHud = CreateWidget<UPlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0), PlayerHudPrefab);
		PlayerHud->AddToViewport();
	}
	if (!PauseMenu && PauseMenuPrefab)
	{
		PauseMenu = CreateWidget<UPauseMenuWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), PauseMenuPrefab);
		PauseMenu->AddToViewport();
	}

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameAndUI{});
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
}

void APrototype_OneCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::TryDash);
		EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::TryMelee);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::TryInteract);
		EnhancedInputComponent->BindAction(ScrollZoomAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::ScrollZoom);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APrototype_OneCharacter::EndSprint);
		EnhancedInputComponent->BindAction(ToggleDebugAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::ToggleDebugMenu);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::StartAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APrototype_OneCharacter::EndAim);
		EnhancedInputComponent->BindAction(OpenBagAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::TryOpenBag);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::PauseGame);
		EnhancedInputComponent->BindAction(UseHealthPotionAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::Heal);
	}
}

void APrototype_OneCharacter::Move(const FInputActionValue& Value)
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
	for(auto bagActor : actors)
	{
		if (auto* bag = Cast<ABag>(bagActor))
		{
			if (bag->IsBiengPulled == true && bag->GetWeight() >= bag->PlayerWeightThreshold)
			{
				float LerpAlpha = ((float)bag->GetWeight() / 2.0f) / (float)bag->StoppingThreshold;
				if (LerpAlpha > 0.3f)
				{
					LerpAlpha = 0.3f;
				}
				GetCharacterMovement()->MaxWalkSpeed = DesiredSpeed * FMath::Lerp(1.0f,0.0f,LerpAlpha);
			}
			else
			{
				GetCharacterMovement()->MaxWalkSpeed = DesiredSpeed;
			}
		}
	}

	
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (DashMovementCurrentTime <= 0 && combatMovementCurrentTime <= 0)
	{
		if (Controller != nullptr)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
			AddMovementInput(ForwardDirection , MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
			
			//UE_LOG(LogTemp, Log, TEXT("Movement Vector: %s"), *MovementVector.ToString());
		}
	}
	else // Dashing code
	{
		if (IsDashing == true)
		{
			if (HasStartedDash == true)
			{
				DashMovementVector = Value.Get<FVector2D>();
				DashMovementVector.Normalize();
				
				if (Controller != nullptr)
				{
					const FRotator Rotation = Controller->GetControlRotation();
					const FRotator YawRotation(0, Rotation.Yaw, 0);
					DashForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
					DashRightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
					
					AddMovementInput(DashForwardDirection , DashMovementVector.Y);
					AddMovementInput(DashRightDirection, DashMovementVector.X);
					
					HasStartedDash = false;
					
					//UE_LOG(LogTemp, Log, TEXT("Dash Movement Vector: %s"), *DashMovementVector.ToString()); // Log output to check current dash vector
				}
			}
		}
	}
}

void APrototype_OneCharacter::StartSprint()
{
	if (GetCharacterMovement()->GetLastUpdateVelocity().Length() != 0 && CanSprint == true)
	{
		if (EntityComponent->Properties.CurrentStamina > EntityComponent->Properties.MinimumStaminaToSprint)
		{
			DesiredSpeed = SprintSpeed;
			EntityComponent->Properties.IsStaminaDraining = true;
			bHasResetStaminaDelay = false;
		}
	}
}

void APrototype_OneCharacter::EndSprint()
{
	DesiredSpeed = JogSpeed;
	EntityComponent->Properties.IsStaminaDraining = false;
	if (!bHasResetStaminaDelay)
	{
		bHasResetStaminaDelay = true;
		EntityComponent->Properties.StaminaDelayTimer = EntityComponent->Properties.StaminaRegenDelay;
	}
}

void APrototype_OneCharacter::TryDash()
{
	if (EntityComponent->Properties.CurrentHealth > 0)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
		for(auto bagActor : actors)
		{
			if (auto* bag = Cast<ABag>(bagActor))
			{
				if (bag->GetMovementState() == MOVEMENTSTATE::FLYING ||
					bag->IsBiengPulled == false)
				{
					if (GetCharacterMovement()->GetLastUpdateVelocity().Length() != 0)
					{
						if (DashMovementCurrentTime <= 0 && EntityComponent->Properties.CurrentStamina > EntityComponent->Properties.StaminaDamageDodge)
						{
							if (NiagaraComp)
								NiagaraComp->SetVisibility(true);
							IsDashing = true;
							HasStartedDash = true;
							EntityComponent->Properties.CurrentStamina -= EntityComponent->Properties.StaminaDamageDodge;
							EntityComponent->Properties.StaminaDelayTimer = EntityComponent->Properties.StaminaRegenDelay;
							if (PlayerHud)
							{
								PlayerHud->UpdateStamina(EntityComponent->Properties.CurrentStamina, EntityComponent->Properties.MaxStamina);
							}
							if (DashAnimation)
								GetMesh()->GetAnimInstance()->Montage_Play(DashAnimation, 1.5f);

							DashMovementCurrentTime = DashMovementMaxTime;

							// For Audio
							OnDashEvent();						
						}
					}
				}
			}
		}
	}
}

void APrototype_OneCharacter::TryMelee()
{
	if (!IsAttacking && EntityComponent->Properties.CurrentStamina > EntityComponent->Properties.StaminaDamageAttack && EntityComponent->Properties.CurrentHealth > 0)
	{
		IsAttacking = true;
		EntityComponent->Properties.CurrentStamina -= EntityComponent->Properties.StaminaDamageAttack;
		EntityComponent->Properties.StaminaDelayTimer = EntityComponent->Properties.StaminaRegenDelay;
		if (PlayerHud)
		{
			PlayerHud->UpdateStamina(EntityComponent->Properties.CurrentStamina, EntityComponent->Properties.MaxStamina);
		}
		if (MeleeAnimation)
			GetMesh()->GetAnimInstance()->Montage_Play(MeleeAnimation, 1.5f);
		combatMovementCurrentTime = combatMovementMaxTime;

		const FVector ForwardVector = GetActorForwardVector();

		if (CurrentlyHeldActor)
		{
			if (auto* sword = Cast<ASword>(CurrentlyHeldActor))
			{
				sword->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
			}
		}
		
		
		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp)
		{
			CharacterComp->Velocity = ForwardVector * AttackMovementAmount;
		}
		
		// broadcast OnAttack for audio		
		OnAttackEvent();
	}
}

void APrototype_OneCharacter::StartAim()
{
	LookAtCursor();
}

void APrototype_OneCharacter::EndAim()
{
	CanSprint = true;
}

void APrototype_OneCharacter::Look(const FInputActionValue& Value)
{
	//FVector2D LookAxisVector = Value.Get<FVector2D>();
//
	//if (Controller != nullptr)
	//{
	//	AddControllerYawInput(LookAxisVector.X);
	//	AddControllerPitchInput(LookAxisVector.Y);
	//}
}

void APrototype_OneCharacter::ScrollZoom(const FInputActionValue& Value)
{
	auto MovementVector = Value.Get<float>();

	UE_LOG(LogTemp, Warning, TEXT("Scrolled! (%s)" ), *FString::FromInt(MovementVector) );
	if (Controller != nullptr && !IsShopping)
	{
		ZoomRatio += MovementVector * Dt * 10;
		ZoomRatio = FMath::Clamp(ZoomRatio, 0, 1);
		UE_LOG(LogTemp, Warning, TEXT("ZoomRatio (%f)" ), ZoomRatio );
		CameraBoom->TargetArmLength = FMath::Lerp(LargestZoomDistance, 300,ZoomRatio );
		CameraBoom->SetRelativeRotation({FMath::Lerp(-50.0f, 0.0f,ZoomRatio / 2.0f),0.0f,0});
		PreviousArmLength = CameraBoom->TargetArmLength;
	}
}

void APrototype_OneCharacter::TryInteract()
{
	//UE_LOG(LogTemp, Warning, TEXT("Try Interact") );
	TArray<AActor*> nearbyActors;
	TArray<AActor*> interactables;
	float nearestActor{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(),nearbyActors);
	
	for(auto* actor : nearbyActors)
	{
		if (actor)
		{
			if (Cast<IInteractInterface>(actor))
			{
				if (CurrentlyHeldActor != actor)
				{
					interactables.Add(actor);
				}
			}
		}
	}
	if (auto* nearestNPC = UGameplayStatics::FindNearestActor(GetActorLocation(), interactables, nearestActor))
	{
		if (nearestActor <= 200)
		{
			if (auto* interactable = Cast<IInteractInterface>(nearestNPC))
			{
				if (Cast<ASword>(nearestNPC))
				{
					if (!CurrentlyHeldActor)
					{
						interactable->Interact();
						CurrentlyHeldActor = nearestNPC;
						return;
					}
					else
					{
						CurrentlyHeldActor->SetActorLocation(GetActorLocation() + GetActorForwardVector() * 10);
						if (auto* sword = Cast<ASword>(CurrentlyHeldActor))
						{
							sword->Unequip();
						}
						CurrentlyHeldActor = nullptr;
						CurrentlyHeldActor = nearestNPC;

						
						interactable->Interact();
						
						return;
					}
				}
				else
				{
					
					interactable->Interact();
					return;
				}
			}
		}
		else
		{
			if (auto* bag = Cast<ABag>(nearestNPC))
			{
				if (bag->IsBiengPulled == true)
				{
					if (bag->CableComponent)
						bag->CableComponent->bAttachEnd = false;
					bag->IsBiengPulled = false;
				}
			}
		}
	}
}

void APrototype_OneCharacter::ToggleDebugMenu()
{
	if (PlayerHud)
	{
		PlayerHud->ToggleDebugMenu();
	}
}

void APrototype_OneCharacter::InteractRaycast()
{
	float mouseX;
	float mouseY;
	auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	controller->GetMousePosition(mouseX, mouseY);
	FVector worldPos{};
	FVector worldDir{};
	UGameplayStatics::DeprojectScreenToWorld(controller, {mouseX, mouseY}, worldPos, worldDir);

	if (PlayerHud)
	{
		PlayerHud->UpdateInteractionText();
		if (controller->GetHitResultUnderCursorByChannel(TraceTypeQuery1, true, LastHitResult))
		{
			
		}
	}
}

void APrototype_OneCharacter::PauseGame()
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		if (PlayerHud)
		{
			PlayerHud->PauseMenu->SetVisibility(ESlateVisibility::Hidden);
			if(auto* PlayerController = Cast<APrototypePlayerController>(Controller))
			{
				PlayerController->SetInputMode(FInputModeGameAndUI{});
			}
		}
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		if (PlayerHud)
		{
			PlayerHud->PauseMenu->SetVisibility(ESlateVisibility::Visible);
			if(auto* PlayerController = Cast<APrototypePlayerController>(Controller))
			{
				PlayerController->SetInputMode(FInputModeUIOnly{});
			}
		}
	}
}

void APrototype_OneCharacter::Ragdoll()
{
	SetReplicateMovement(false);

	//DetachFromControllerPendingDestroy();
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SetActorEnableCollision(true);
	
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}
}

void APrototype_OneCharacter::UnRagdoll()
{
	SetReplicateMovement(true);

	//DetachFromControllerPendingDestroy();
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Pawn"));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SetActorEnableCollision(false);
	
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->SetComponentTickEnabled(true);
	}
}

void APrototype_OneCharacter::UpdateFadeActors()
{
	CameraHitMeshes.Reset();
	TArray<FHitResult> OutHits;
	FCollisionShape capsule = FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetScaledCapsuleRadius() - 1.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 1.0f);
	if (GetWorld()->SweepMultiByChannel(OutHits, FollowCamera->GetComponentLocation(), GetActorLocation(), FQuat::Identity, ECC_WorldStatic, capsule))
	{
		for(auto& object : OutHits)
		{
			if (auto* tree = Cast<ATree>(object.GetActor()))
			{
				CameraHitMeshes.AddUnique(tree->Trunk);
				CameraHitMeshes.AddUnique(tree->Leaves);
			}
		}
	}
	
}

void APrototype_OneCharacter::SetShowMeshes()
{
	TArray<UActorComponent*> fadeComponents{};
	TArray<UStaticMeshComponent*> meshesToBeRemoved;
	for(auto& hiddenMesh : HiddenMeshes)
	{
		if (!CameraHitMeshes.Contains(hiddenMesh))
		{
			fadeComponents = hiddenMesh->GetAttachmentRootActor()->GetComponentsByClass(UFadeComponent::StaticClass());
			for(auto& component : fadeComponents)
			{
				if (auto* fadeComponent = Cast<UFadeComponent>(component))
				{
					fadeComponent->ToggleHideMesh(false);
					meshesToBeRemoved.AddUnique(hiddenMesh);
					return;
				}
			}
		}
	}

	for(auto* mesh : meshesToBeRemoved)
	{
		HiddenMeshes.Remove(mesh);
	}
	
}

void APrototype_OneCharacter::SetHiddenMeshes()
{
	TArray<UActorComponent*> fadeComponents{};
	for(auto& hitMesh : CameraHitMeshes)
	{
		fadeComponents = hitMesh->GetAttachmentRootActor()->GetComponentsByClass(UFadeComponent::StaticClass());
		for(auto& component : fadeComponents)
		{
			if (auto* fadeComponent = Cast<UFadeComponent>(component))
			{
				fadeComponent->ToggleHideMesh(true);
				HiddenMeshes.AddUnique(hitMesh);
				return;
			}
		}
	}
}

void APrototype_OneCharacter::LookAtCursor()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);
	Cast<APrototypePlayerController>(Controller)->SetInputMode(InputMode);

	// Show the mouse cursor
	Cast<APrototypePlayerController>(Controller)->bShowMouseCursor = true;
	
	float mouseX;
	float mouseY;
	auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	controller->GetMousePosition(mouseX, mouseY);
	FVector worldPos{};
	FVector worldDir{};
	UGameplayStatics::DeprojectScreenToWorld(controller, {mouseX, mouseY}, worldPos, worldDir);

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, worldPos, worldPos + worldDir * 10000, ECC_Visibility, QueryParams))
	{
		SetActorRotation(FRotator{GetActorRotation().Pitch, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Hit.Location).Yaw, GetActorRotation().Roll});
	}

	CanSprint = false;
}

void APrototype_OneCharacter::TryOpenBag()
{
	if (HasRespawnedOnce == false)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
		for(auto bagActor : actors)
		{
			if (auto* bag = Cast<ABag>(bagActor))
			{
				bag->IsOpen = !bag->IsOpen;

				// Call event for audio
				OnBagOpenOrClose();
			}
		}
	}
}

void APrototype_OneCharacter::TakeDamage(int _amount)
{
	if (IsDashing != true && IsAttacking != true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Damaged by enemy"));
		EntityComponent->TakeDamage(_amount);
		if (PlayerHud)
		{
			PlayerHud->UpdateHealth(EntityComponent->Properties.CurrentHealth, EntityComponent->Properties.MaxHealth);
		}
		if (EntityComponent->Properties.CurrentHealth <= 0)
		{
			//Ragdoll();
		
			//Controller->SetIgnoreMoveInput(true);
			//Controller->Possess(nullptr);
		}
		OnPlayerHitEvent();
	}
	
}

void APrototype_OneCharacter::RecoverHealth(int _amount)
{
	EntityComponent->Heal(_amount);
	if (PlayerHud)
	{
		PlayerHud->UpdateHealth(EntityComponent->Properties.CurrentHealth, EntityComponent->Properties.MaxHealth);
	}
}

void APrototype_OneCharacter::PlayerRespawn()
{
	if (EntityComponent->Properties.CurrentHealth <= 0)
	{
		if (IsRespawning == false)
		{
			TArray<AActor*> actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
		
			if (!HasRespawnedOnce) // Bag remains in place, player has one chance to get
			{
				HasRespawnedOnce = true;
				PlayerInventory->BagDropped();
				for (auto* bagActor:actors)
				{
					if (auto* bag = Cast<ABag>(bagActor))
					{
						bag->IsBiengPulled = false;
						bag->IsOpen = false;
						if (bag->CableComponent)
							bag->CableComponent->bAttachEnd = false;
					}
				}
			}
			else // Player loses contents of bag, but get it back
			{
				HasRespawnedOnce = false;
				PlayerInventory->BagLost();
				for (auto* bagActor:actors)
				{
					if (auto* bag = Cast<ABag>(bagActor))
					{
						bag->IsBiengPulled = true;
						bag->IsOpen = false;
					}
				}
				// Delete contents of bag & reset size etc
			}

			IsRespawning = true;
			this->GetMesh()->SetVisibility(false); // temporary, make player invisible

			TArray<AActor*> swordActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASword::StaticClass(), swordActors);

			for (auto* swordActor:swordActors)
			{
				if (auto* sword = Cast<ASword>(swordActor))
				{
					sword->Mesh->SetVisibility(false);
				}
			}
			
			UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
			if (CharacterComp)
			{
				CharacterComp->Deactivate();
			}

			UE_LOG(LogTemp, Warning, TEXT("Player respawned"));
		}
	}

	if (IsRespawning == true)
	{
		RespawnTimer -= Dt;
		PauseMovementTimer -= Dt;

		if (fadeState != FadeState::FadeIn)
		{
			fadeState = FadeState::FadeIn;
			PlayerHud->FadeIn();
		}

		if (RespawnTimer <= 0)
		{
			SetActorLocation(StartLocation);
			
			this->GetMesh()->SetVisibility(true);

			TArray<AActor*> swordActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASword::StaticClass(), swordActors);

			for (auto* swordActor:swordActors)
			{
				if (auto* sword = Cast<ASword>(swordActor))
				{
					sword->Mesh->SetVisibility(true);
				}
			}
		}
		
		if (PauseMovementTimer <= 0)
		{
			IsRespawning = false;

			if (fadeState != FadeState::FadeOut)
			{
				fadeState = FadeState::FadeOut;
				PlayerHud->FadeOut();
			}
			
			RespawnTimer = TimeBeforeRespawn;
			PauseMovementTimer = TimeRespawnPauseMovement;

			EntityComponent->Properties.CurrentHealth = EntityComponent->Properties.MaxHealth; // Reset health
			EntityComponent->Properties.CurrentStamina = EntityComponent->Properties.MaxStamina; // Reset stamina
			
			UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
			if (CharacterComp)
			{
				CharacterComp->Activate();
			}
		}
	}
}

void APrototype_OneCharacter::RegenHealth()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADialogueNPC::StaticClass(), actors);
	for(auto npcActor : actors)
	{
		if (auto* npc = Cast<ADialogueNPC>(npcActor))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Distance between player & mole %f"), (npc->GetActorLocation() - GetActorLocation()).Length()); // Output distance between player & mole npc
			
			if ((npc->GetActorLocation() - GetActorLocation()).Length() <= DistanceToMole)
			{
				if (EntityComponent->Properties.CurrentHealth < EntityComponent->Properties.MaxHealth)
				{
					if (NiagaraHealComp)
					{
						NiagaraHealComp->SetVisibility(true);
					}
					if (HealthRegenTimer >= 0)
					{
						HealthRegenTimer -= Dt;
					}
					if (HealthRegenTimer < 0)
					{
						EntityComponent->Properties.CurrentHealth += 1;
						HealthRegenTimer = MaxTimeUntilHealthRegen;
					}
				}
			}
			else
			{
				NiagaraHealComp->SetVisibility(false);
			}
		}
	}
}


void APrototype_OneCharacter::Heal()
{
	if (PlayerInventory->UsePotion())
	{
		int recover = healthPotion->amountRestored;
		RecoverHealth(recover);

		// Call event for audio
		OnDrinkPotionEvent();
	}
}

void APrototype_OneCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this) 
	{
		if (auto* enemy = Cast<APrototypeEnemy>(OtherActor))
		{
			enemy->UpdateAttackOutline(true);
			
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Triggered with enemy"));
	}
}

void APrototype_OneCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != this) 
	{
		if (auto* enemy = Cast<APrototypeEnemy>(OtherActor))
		{
			enemy->UpdateAttackOutline(false);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Stopped triggering with enemy"));
	}
}


