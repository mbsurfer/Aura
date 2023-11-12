


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "UI/HUD/AuraHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/AuraWidgetController.h"


UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController* PC =  UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
        {
            AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
            UAttributeSet* AS = PS->GetAttributeSet();

            const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);

            return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
        }
    }

    return nullptr;
}

//todo: this function repeat a lot of code from GetOverlayWidgetController(). maybe we can write a helper function that builds the controller params?
UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController* PC =  UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
        {
            AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
            UAttributeSet* AS = PS->GetAttributeSet();

            const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);

            return AuraHUD->GetAttributeWidgetController(WidgetControllerParams);
        }
    }

    return nullptr;
}
