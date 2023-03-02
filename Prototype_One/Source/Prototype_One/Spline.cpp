// Fill out your copyright notice in the Description page of Project Settings.


#include "Spline.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpline::ASpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("spline component"));
	Spline->SetupAttachment(RootComponent);
}

void ASpline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	
	
}

// Called when the game starts or when spawned
void ASpline::BeginPlay()
{
	Super::BeginPlay();

	if (Spline && Mesh)
	{
		for(int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
		{
			auto* s = NewObject<USplineMeshComponent>(RootComponent, USplineMeshComponent::StaticClass(), TEXT("spline mesh component"));

			s->RegisterComponent();
			s->SetupAttachment(RootComponent);
			s->InitializeComponent();
			s->SetStaticMesh(Mesh);
			s->SetForwardAxis(ESplineMeshAxis::Z);
			s->SetStartPosition(Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local), true);
			s->SetStartTangent(Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local), true);
			s->SetEndPosition(Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local), true);
			s->SetEndTangent(Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local), true);
		}
	}
}

// Called every frame
void ASpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

