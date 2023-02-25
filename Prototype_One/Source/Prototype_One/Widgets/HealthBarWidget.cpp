#include "HealthBarWidget.h"

#include "Components/ProgressBar.h"

void UHealthBarWidget::SetHealthPercent(int _health, int _maxHealth)
{
	HealthBar->SetPercent(_health /_maxHealth);
}
