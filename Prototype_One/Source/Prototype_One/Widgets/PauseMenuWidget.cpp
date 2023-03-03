#include "PauseMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetVisibility(ESlateVisibility::Hidden);
	ResumeButton->OnPressed.AddDynamic(this, &UPauseMenuWidget::ResumeButtonPress);
	SaveButton->OnPressed.AddDynamic(this, &UPauseMenuWidget::SaveButtonPress);
	LoadButton->OnPressed.AddDynamic(this, &UPauseMenuWidget::LoadButtonPress);
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
}

void UPauseMenuWidget::SaveButtonPress()
{
}

void UPauseMenuWidget::LoadButtonPress()
{
}

void UPauseMenuWidget::QuitButtonPress()
{
	UGameplayStatics::OpenLevel(GetWorld(), "Level_MainMenu");
}
