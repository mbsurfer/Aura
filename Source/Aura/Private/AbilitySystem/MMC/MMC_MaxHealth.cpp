


#include "AbilitySystem/MMC/MMC_MaxHealth.h"

#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"


UMMC_MaxHealth::UMMC_MaxHealth()
{
    VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
    VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
    VigorDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(VigorDef);
}


float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const 
{
    // Gather tags from source & target
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    float Vigor = 0.f;
    GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);

    // Make sure Vigor cannot be below 0
    Vigor = FMath::Max<float>(Vigor, 0);

    // Get the player level from our CombatInterface
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
    const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

    // Base life is 80
    return 80.f + (2.5f * Vigor) + (10 * PlayerLevel);
}