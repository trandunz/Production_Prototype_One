#include "PauseMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"

void UPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetVisibility(ESlateVisibility::Hidden);
	ResumeButton->OnPressed.AddDynamic(this, &UPauseMenuWidget::ResumeButtonPress);
	QuitButton->OnPressed.AddDynamic(this, &UPauseMenuWidget::QuitButtonPress);
}

void UPauseMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPauseMenuWidget::ResumeButtonPress()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	SetVisibility(ESlateVisibility::Hidden);
	auto* playercontroller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(auto* PlayerController = Cast<APrototypePlayerController>(playercontroller))
	{
		PlayerController->SetInputMode(FInputModeGameAndUI{});
	}
}

void UPauseMenuWidget::QuitButtonPress()
{
	UGameplayStatics::OpenLevel(GetWorld(), "Level_MainMenu");
}
