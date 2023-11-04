


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
    OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
    OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindAttributeChangeBroadcast(const FGameplayAttribute& Attribute, FOnAttributeChangedSignature* AttributeChangeDelegate)
{
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
        [AttributeChangeDelegate](const FOnAttributeChangeData& Data)
        {
            AttributeChangeDelegate->Broadcast(Data.NewValue);
        }
    );

}

/** Configure broadcast events to update the overlay widget components */
void UOverlayWidgetController::BindCallbacksToDependencies()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    // ----------------------------------------------------------------------------------------------------------------
	// Vital Attribute
	// ----------------------------------------------------------------------------------------------------------------

	/** Health */
    BindAttributeChangeBroadcast(AuraAttributeSet->GetHealthAttribute(), &OnHealthChanged);

    /** Max Health */
    BindAttributeChangeBroadcast(AuraAttributeSet->GetMaxHealthAttribute(), &OnMaxHealthChanged);

    /** Mana */
    BindAttributeChangeBroadcast(AuraAttributeSet->GetManaAttribute(), &OnManaChanged);

    // /** Max Mana */
    BindAttributeChangeBroadcast(AuraAttributeSet->GetMaxManaAttribute(), &OnMaxManaChanged);

    // ----------------------------------------------------------------------------------------------------------------
	// Assets Tags
	// ----------------------------------------------------------------------------------------------------------------

    Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
        [this](const FGameplayTagContainer& AssetTags)
        {
            for (const auto Tag : AssetTags)
            {
                UE_LOG(LogTemp, Display, TEXT("tag: %s"), *Tag.ToString());

                FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWudgetDataTable, Tag);
                MessageWidgetRowDelegate.Broadcast(*Row);
            }
        }
    );

}