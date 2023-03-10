#include "FadeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Prototype_One/Tree.h"

UFadeComponent::UFadeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFadeComponent::BeginPlay()
{
	Super::BeginPlay();
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

	if (auto* tree = Cast<ATree>(GetOwner()))
	{
		for(auto* material : tree->Trunk->GetMaterials())
		{
			if (UMaterialInstanceDynamic* dynamicMaterial = UMaterialInstanceDynamic::Create(material, tree->Trunk))
			{
				FLinearColor color;
				dynamicMaterial->GetVectorParameterValue(FName("Colour"), color);
				color.A = FadeCurve->GetFloatValue(CurrentFade);
				dynamicMaterial->SetVectorParameterValue("Colour", color);
			}
		}
		for(auto* material : tree->Leaves->GetMaterials())
		{
			if (UMaterialInstanceDynamic* dynamicMaterial = UMaterialInstanceDynamic::Create(material, tree->Leaves))
			{
				FLinearColor color;
				dynamicMaterial->GetVectorParameterValue(FName("Colour"), color);
				color.A = FadeCurve->GetFloatValue(CurrentFade);
				dynamicMaterial->SetVectorParameterValue("Colour", color);
			}
		}
	}
}

void UFadeComponent::ToggleHideMesh(bool _hide)
{
}

