


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

void UOverlayWidgetController::BindToAttributeValueChangeDelegate(const FGameplayAttribute& Attribute, FOnAttributeChangedSignature* AttributeChangeDelegate)
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
	// Vital Attributes
	// ----------------------------------------------------------------------------------------------------------------

	/** Health */
    BindToAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute(), &OnHealthChanged);

    /** Max Health */
    BindToAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute(), &OnMaxHealthChanged);

    /** Mana */
    BindToAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute(), &OnManaChanged);

    // /** Max Mana */
    BindToAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute(), &OnMaxManaChanged);

    // ----------------------------------------------------------------------------------------------------------------
	// Assets Tags
	// ----------------------------------------------------------------------------------------------------------------

    Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
        [this](const FGameplayTagContainer& AssetTags)
        {
            for (const auto Tag : AssetTags)
            {
                UE_LOG(LogTemp, Warning, TEXT("Source Tag: %s"), *Tag.ToString());

                // "A.1".MatchesTag("A") = True
                // "A".Matches("A.1")    = False
                const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
                if (Tag.MatchesTag(MessageTag))
                {
                    const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWudgetDataTable, Tag);
                    MessageWidgetRowDelegate.Broadcast(*Row);
                }
            }
        }
    );

}