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
#include "PrototypeEnemy.h"
#include "Prototype_One/Widgets/PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prototype_One/Bag.h"
#include "Prototype_One/Sword.h"
#include "Prototype_One/Components/FadeComponent.h"
#include "Prototype_One/Components/PlayerInventory.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Prototype_One/Controllers/EnemyController.h"
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

	EntityComponent = CreateDefaultSubobject<URPGEntityComponent>(TEXT("Entity Component"));
	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("Inventory"));
}

void APrototype_OneCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	InitGUI();
	CameraBoom->TargetArmLength = FMath::Lerp(LargestZoomDistance, 300,ZoomRatio );
	if (PlayerHud)
	{
		PlayerHud->UpdateHealth(EntityComponent->CurrentHealth, EntityComponent->MaxHealth);
		PlayerHud->UpdateStamina(EntityComponent->CurrentStamina, EntityComponent->MaxStamina);
	}

	EndSprint();

	InitInputMappingContext();
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
		IsDashing = false;
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
	if (EntityComponent->CurrentStamina <= 0)
		EndSprint();

	UpdateFadeActors();
	SetShowMeshes();
	SetHiddenMeshes();

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
		EnhancedInputComponent->BindAction(OpenBagAction, ETriggerEvent::Triggered, this, &APrototype_OneCharacter::TryOpenBag);
	}
}

void APrototype_OneCharacter::Move(const FInputActionValue& Value)
{
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
	else // Rolling code
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
					
					UE_LOG(LogTemp, Log, TEXT("Dash Movement Vecotr: %s"), *DashMovementVector.ToString());
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

void APrototype_OneCharacter::TryDash()
{
	if (GetCharacterMovement()->GetLastUpdateVelocity().Length() != 0)
	{
		if (DashMovementCurrentTime <= 0 && EntityComponent->CurrentStamina > EntityComponent->DashStaminaCost)
		{
			//if (DashAnimation)
			//{
				IsDashing = true;
				HasStartedDash = true;
				EntityComponent->CurrentStamina -= EntityComponent->DashStaminaCost;
				if (PlayerHud)
				{
					PlayerHud->UpdateStamina(EntityComponent->CurrentStamina, EntityComponent->MaxStamina);
				}
				//GetMesh()->GetAnimInstance()->Montage_Play(DashAnimation, 1.5f);
				DashMovementCurrentTime = DashMovementMaxTime;
			//}
		}
	}
}

void APrototype_OneCharacter::TryMelee()
{
	if (combatMovementCurrentTime <= 0 && EntityComponent->CurrentStamina > EntityComponent->AttackStaminaCost)
	{
		IsAttacking = true;
		EntityComponent->CurrentStamina -= EntityComponent->AttackStaminaCost;
		if (PlayerHud)
		{
			PlayerHud->UpdateStamina(EntityComponent->CurrentStamina, EntityComponent->MaxStamina);
		}
		if (MeleeAnimation)
			GetMesh()->GetAnimInstance()->Montage_Play(MeleeAnimation, 1.5f);
		combatMovementCurrentTime = combatMovementMaxTime;
	}
}

void APrototype_OneCharacter::StartAim()
{
	LookAtCursor();
    
	//// FHitResult will hold all data returned by our line collision query
	//FHitResult Hit;
	//ETraceTypeQuery Query{};
	//FHitResult EndHit;
//
	//// We set up a line trace from our current location to a point 1000cm ahead of us
	//FVector TraceStart = FollowCamera->GetComponentLocation();
	//FVector TraceEnd;
//
	//if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHitResultUnderCursorByChannel(Query, false, EndHit))
	//{
	//	TraceEnd = EndHit.Location;
	//}
//
	//// You can use FCollisionQueryParams to further configure the query
	//// Here we add ourselves to the ignored list so we won't block the trace
	//FCollisionQueryParams QueryParams;
	//QueryParams.AddIgnoredActor(this);
//
	//if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	//{
	//	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
	//}
//
	//Controller->SetControlRotation(FRotator{Controller->GetControlRotation().Pitch, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Hit.Location).Yaw, Controller->GetControlRotation().Roll});
	////GetMesh()->SetWorldRotation(FRotator{GetMesh()->GetComponentRotation().Pitch, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Hit.Location).Yaw, GetMesh()->GetComponentRotation().Roll});
}

//void APrototype_OneCharacter::EndAim()
//
//	
//

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
					//UE_LOG(LogTemp, Warning, TEXT("Mesh in front of player!!" ));
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
}

void APrototype_OneCharacter::TryOpenBag()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
	for(auto bagActor : actors)
	{
		if (auto* bag = Cast<ABag>(bagActor))
		{
			bag->IsOpen = !bag->IsOpen;
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



