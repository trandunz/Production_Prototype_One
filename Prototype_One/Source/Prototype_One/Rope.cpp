// Fill out your copyright notice in the Description page of Project Settings.


#include "Rope.h"

#include "Prototype_One/Bag.h"
#include "CableComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"

ARope::ARope()
{
	PrimaryActorTick.bCanEverTick = true;
	//Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Constraint"));
	//Constraint->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//Constraint->SetWorldLocation(GetActorLocation());
}

void ARope::BeginPlay()
{
	Super::BeginPlay();

	//if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	//{
	//	TArray<AActor*> actors;
	//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
	//	for(auto bagActor : actors)
	//	{
	//		if (auto* bag = Cast<ABag>(bagActor))
	//		{
	//			Constraint->ConstraintActor2 = bag;
	//			Constraint->ConstraintActor1 = player;
	//			return;
	//		}
	//	}
	//}
}

void ARope::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	//{
	//	SetActorLocation(player->GetActorLocation());
	//}
	//Constraint->SetWorldLocation(GetActorLocation());
}
