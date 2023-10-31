


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
    PlayerController = WCParams.PlayerController;
    PlayerState = WCParams.PlayerState;
    AbilitySystemComponent = WCParams.AbilitySystemComponent;
    AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
    // This function is implimented by classes that are derived from UAuraWidgetController
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
    // This function is implimented by classes that are derived from UAuraWidgetController
}