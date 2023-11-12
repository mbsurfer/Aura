


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"


UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
    if (!OverlayWidgetController)
    {
        OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
        OverlayWidgetController->SetWidgetControllerParams(WCParams);
        OverlayWidgetController->BindCallbacksToDependencies();
    }

    return OverlayWidgetController;
}


UAttributeMenuWidgetController* AAuraHUD::GetAttributeWidgetController(const FWidgetControllerParams& WCParams)
{
    if (!AttributeMenuWidgetController)
    {
        AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
        AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
        AttributeMenuWidgetController->BindCallbacksToDependencies();
    }

    return AttributeMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
    // checkf() checks a condition and returns a formatted string to the crash log if check fails
    checkf(OverlayWidgetClass, TEXT("Overlay Widget Class has not been initialized, please set in BP_AuraHUD."));
    checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class has not been initialized, please set in BP_AuraHUD."));

    checkf(AttributeMenuWidgetControllerClass, TEXT("Attribute Menu Widget Controller Class has not been initialized, please set in BP_AuraHUD."));

    // Construct the widget controller
    const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
    UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

    // Construct the widget
    OverlayWidget = Cast<UAuraUserWidget>(CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass));

    // Set the widget's controller
    OverlayWidget->SetWidgetController(WidgetController);

    // Initialize player's ability values. This should be called after the widget has had it's controller set.
    WidgetController->BroadcastInitialValues();

    // Show the UI
    OverlayWidget->AddToViewport();
}
