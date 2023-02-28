#include "Prototype_OneCharacter.h"

#include "AITypes.h"
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
#include "Prototype_One/Widgets/PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "Prototype_One/Sword.h"
#include "Prototype_One/Components/FadeComponent.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"

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

	// Components
	EntityComponent = CreateDefaultSubobject<URPGEntityComponent>(TEXT("Entity Component"));
	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("Player Inventory"));
}

void APrototype_OneCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitInputMappingContext();
	InitGUI();
	CameraBoom->TargetArmLength = FMath::Lerp(LargestZoomDistance, 300,ZoomRatio );
	if (PlayerHud)
	{
		PlayerHud->UpdateHealth(EntityComponent->CurrentHealth, EntityComponent->MaxHealth);
		PlayerHud->UpdateMana(EntityComponent->CurrentMana, EntityComponent->MaxMana);
		PlayerHud->UpdateStamina(EntityComponent->CurrentStamina, EntityComponent->MaxStamina);
	}

	EndSprint();
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
	if (dodgeMovementCurrentTime > 0)
		dodgeMovementCurrentTime -= DeltaSeconds;
	if (dodgeMovementCurrentTime <= 0)
		IsDodging = false;
	// Dodging
	if (IsDodging == true)
	{
		GetCharacterMovement()->Velocity.Set(DodgeMovementVector.Y * 1000.0f, DodgeMovementVector.X * 1000.0f, GetCharacterMovement()->Velocity.Z);
	}
	
	// Timer for combat
	if (combatMovementCurrentTime > 0)
		combatMovementCurrentTime -= DeltaSeconds;
	if (combatMovementCurrentTime <= 0)
		IsAttacking = false;

	// Sprint related
	if (EntityComponent->CurrentStamina <= 0)
		EndSprint();

	UpdateFadeActors();
	SetShowMeshes();
	SetHiddenMeshes();
}

void APrototype_OneCharacter::InitInputMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		PlayerController->SetInputMode(FInputModeGameAndUI{});
		PlayerController->bShowMouseCursor = true;
	}
}

void APrototype_OneCharacter::InitGUI()
{
	if (!PlayerHud && PlayerHudPrefab)
	{
		PlayerHud = CreateWidget<UPlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0), PlayerHudPrefab);
		PlayerHud->AddToViewport();
	}
}

void APrototype_OneCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::TryRoll);
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
	}
}

void APrototype_OneCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (dodgeMovementCurrentTime <= 0 && combatMovementCurrentTime <= 0)
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
	else // Rolling code
	{
		if (IsDodging == true)
		{
			if (HasStartedDodge == true)
			{
				DodgeMovementVector = Value.Get<FVector2D>();
				DodgeMovementVector.Normalize();
				
				if (Controller != nullptr)
				{
					const FRotator Rotation = Controller->GetControlRotation();
					const FRotator YawRotation(0, Rotation.Yaw, 0);
					DodgeForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
					DodgeRightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
					
					AddMovementInput(DodgeForwardDirection , DodgeMovementVector.Y);
					AddMovementInput(DodgeRightDirection, DodgeMovementVector.X);
					
					HasStartedDodge = false;
					
					UE_LOG(LogTemp, Log, TEXT("Dodge Movement Vecotr: %s"), *DodgeMovementVector.ToString());
				}
			}
		}
	}
}

void APrototype_OneCharacter::StartSprint()
{
	if (GetCharacterMovement()->GetLastUpdateVelocity().Length() != 0)
	{
		if (EntityComponent->CurrentStamina > EntityComponent->MinimumStaminaToSprint)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
			EntityComponent->IsStaminaDraining = true;
		}
	}
}

void APrototype_OneCharacter::EndSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	EntityComponent->IsStaminaDraining = false;
}

void APrototype_OneCharacter::TryRoll()
{
	if (GetCharacterMovement()->GetLastUpdateVelocity().Length() != 0)
	{
		if (dodgeMovementCurrentTime <= 0 && EntityComponent->CurrentStamina > EntityComponent->StaminaDamageDodge)
		{
			if (RollAnimation)
			{
				IsDodging = true;
				HasStartedDodge = true;
				EntityComponent->CurrentStamina -= EntityComponent->StaminaDamageDodge;
				if (PlayerHud)
				{
					PlayerHud->UpdateStamina(EntityComponent->CurrentStamina, EntityComponent->MaxStamina);
				}
				GetMesh()->GetAnimInstance()->Montage_Play(RollAnimation, 1.5f);
				dodgeMovementCurrentTime = dodgeMovementMaxTime;
			}
		}
	}
}

void APrototype_OneCharacter::TryMelee()
{
	if (combatMovementCurrentTime <= 0 && EntityComponent->CurrentStamina > EntityComponent->StaminaDamageAttack)
	{
		IsAttacking = true;
		EntityComponent->CurrentStamina -= EntityComponent->StaminaDamageAttack;
		if (PlayerHud)
		{
			PlayerHud->UpdateStamina(EntityComponent->CurrentStamina, EntityComponent->MaxStamina);
		}
		if (MeleeAnimation)
			GetMesh()->GetAnimInstance()->Montage_Play(MeleeAnimation, 1.5f);
		combatMovementCurrentTime = combatMovementMaxTime;
	}
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
	if (Controller != nullptr)
	{
		ZoomRatio += MovementVector * Dt * 10;
		ZoomRatio = FMath::Clamp(ZoomRatio, 0, 1);
		UE_LOG(LogTemp, Warning, TEXT("ZoomRatio (%f)" ), ZoomRatio );
		CameraBoom->TargetArmLength = FMath::Lerp(LargestZoomDistance, 300,ZoomRatio );
		CameraBoom->SetRelativeRotation({FMath::Lerp(-50.0f, 0.0f,ZoomRatio / 2.0f),0.0f,0});
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
			if (auto* interactable = Cast<IInteractInterface>(actor))
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
			if (auto* hitActor = LastHitResult.GetActor())
			{
				if (auto* npc = Cast<ADialogueNPC>(hitActor))
				{
					// Render GUI
					//PlayerHud->UpdateInteractionText("LMouse", "To Talk With NPC");
				}
			}
		}
	}
}

void APrototype_OneCharacter::Ragdoll()
{
	SetReplicateMovement(false);

	DetachFromControllerPendingDestroy();
	
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

	SetLifeSpan(10.0f);
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
			TArray<UActorComponent*> meshes{};
			meshes = object.GetActor()->GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for(auto& mesh : meshes)
			{
				if (auto* staticMesh = Cast<UStaticMeshComponent>(mesh))
				{
					UE_LOG(LogTemp, Warning, TEXT("Mesh in front of player!!" ));
					CameraHitMeshes.AddUnique(staticMesh);
				}
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

void APrototype_OneCharacter::TakeDamage(int _amount)
{
	EntityComponent->TakeDamage(_amount);
	if (PlayerHud)
	{
		PlayerHud->UpdateHealth(EntityComponent->CurrentHealth, EntityComponent->MaxHealth);
	}
	if (EntityComponent->CurrentHealth <= 0)
	{
		Ragdoll();
		
		//Controller->SetIgnoreMoveInput(true);
		//Controller->Possess(nullptr);
	}
}



