#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FadeComponent.generated.h"

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_ONE_API UFadeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFadeComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ToggleHideMesh(bool);
	
	UPROPERTY(EditAnywhere)
	class UMaterial* FadeMaterial;

	UPROPERTY(EditAnywhere)
	class UMaterialInstanceDynamic* FadeMaterialI;

	UPROPERTY(EditAnywhere)
	class UMaterial* BaseMaterial;

	UPROPERTY(EditAnywhere)
	class UCurveFloat* FadeCurve;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ParentMesh;

	bool Hidden{};

	float CurrentFade{1.0f};
	float FadeRate{2.0f};
};
