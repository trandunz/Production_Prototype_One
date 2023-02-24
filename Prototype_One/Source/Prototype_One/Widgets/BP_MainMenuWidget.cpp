#include "BP_MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UBP_MainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PlayButton->OnPressed.AddDynamic(this, &UBP_MainMenuWidget::PlayGame);
	OptionsButton->OnPressed.AddDynamic(this, &UBP_MainMenuWidget::OpenOptionsMenu);
	QuitButton->OnPressed.AddDynamic(this, &UBP_MainMenuWidget::QuitGame);
}

void UBP_MainMenuWidget::PlayGame()
{
	
}

void UBP_MainMenuWidget::OpenOptionsMenu()
{
}

void UBP_MainMenuWidget::QuitGame()
{
}
