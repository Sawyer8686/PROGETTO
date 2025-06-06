#include "TorchBatteryWidget.h"
#include "Components/ProgressBar.h"


void UTorchBatteryWidget::SetBatteryPercent(float Percent)
{
    if (BatteryProgressBar)
    {
        BatteryProgressBar->SetPercent(Percent);
    }
}
