


#include "UI/Widget/AuraUserWidget.h"


void UAuraUserWidget::SetWidgetController(UObject* InWidnetController)
{
    WidgetController = InWidnetController;
    WidgetControllerSet();
}
