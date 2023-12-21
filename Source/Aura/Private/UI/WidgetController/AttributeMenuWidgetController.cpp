


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

// Set the inital attribute values to display in the attribute menu
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

    check(AttributeInfo);

    for (auto& Pair : AS->TagsToAttributes)
    {
        FAuraAttributeInfo Info = AttributeInfo->GetAttributeInfoByTag(Pair.Key);
        Info.AttributeValue = Pair.Value().GetNumericValue(AS);
        AttributeInfoDelegate.Broadcast(Info);
    }
}

// Update attribute menu values when attributes change
void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
    UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

    for (auto& Pair : AS->TagsToAttributes)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
            [this, Pair, AS](const FOnAttributeChangeData& Data)
            {
                FAuraAttributeInfo Info = AttributeInfo->GetAttributeInfoByTag(Pair.Key);
                Info.AttributeValue = Pair.Value().GetNumericValue(AS);
                AttributeInfoDelegate.Broadcast(Info);
            }
        );
    }
}