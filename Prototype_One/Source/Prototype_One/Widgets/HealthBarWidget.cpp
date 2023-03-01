#include "HealthBarWidget.h"

#include "Components/ProgressBar.h"

void UHealthBarWidget::SetHealthPercent(int _health, int _maxHealth)
{
	HealthBar->SetPercent((float)_health /(float)_maxHealth);
}
