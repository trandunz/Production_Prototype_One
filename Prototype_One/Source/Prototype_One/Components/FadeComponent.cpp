#include "FadeComponent.h"
#include "Components/StaticMeshComponent.h"

UFadeComponent::UFadeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFadeComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> meshes{};
	meshes = GetOwner()->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	for(auto& mesh : meshes)
	{
		if (auto* staticMesh = Cast<UStaticMeshComponent>(mesh))
		{
			ParentMesh = staticMesh;
			BaseMaterial = Cast<UMaterial>(ParentMesh->GetMaterial(0));
			break;
		}
	}

	
	FadeMaterialI = UMaterialInstanceDynamic::Create(FadeMaterial, ParentMesh);
}

void UFadeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Hidden)
	{
		CurrentFade -= (DeltaTime * FadeRate);
		CurrentFade = FMath::Clamp(CurrentFade, 0.2f, 1);
	}
	else
	{
		CurrentFade += (DeltaTime * FadeRate);
		CurrentFade = FMath::Clamp(CurrentFade, 0.2f, 1);
	}

	FadeMaterialI->SetScalarParameterValue("Fade", FadeCurve->GetFloatValue(CurrentFade));
	
	if (CurrentFade == 1 && !Hidden)
	{
		ParentMesh->SetMaterial(0, BaseMaterial);
	}
}

void UFadeComponent::ToggleHideMesh(bool _hide)
{
	TArray<UActorComponent*> meshes{};
	if (_hide != Hidden)
	{
		Hidden = _hide;
		if (Hidden)
		{
			meshes = GetOwner()->GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for(auto& mesh : meshes)
			{
				if (auto* staticMesh = Cast<UStaticMeshComponent>(mesh))
				{
					staticMesh->SetMaterial(0, FadeMaterialI);
				}
			}
		}
	}
}

