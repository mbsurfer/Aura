

#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
    InitHealth(50.f);
    InitMaxHealth(200.f);
    InitMana(0.f);
    InitMaxMana(100.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

// Epic recommends to only use this function when doing clamping (the process of limiting a value to a range between a minimum and a maximum value).
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    else if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
    }
}


void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData &Data, FEffectProperties& Props) const
{
    Props.EffectContexthandle = Data.EffectSpec.GetContext();

    // Source = causer of the effect
    Props.Source.ASC = Props.EffectContexthandle.GetOriginalInstigatorAbilitySystemComponent();
    if (IsValid(Props.Source.ASC) && Props.Source.ASC->AbilityActorInfo.IsValid() && Props.Source.ASC->AbilityActorInfo->AvatarActor.IsValid())
    {
        Props.Source.AvatarActor = Props.Source.ASC->AbilityActorInfo->AvatarActor.Get();
        Props.Source.Controller = Props.Source.ASC->AbilityActorInfo->PlayerController.Get();

        // If we are unable to get the controller from the ASC, try to get it from the pawn
        if (!Props.Source.Controller && Props.Source.AvatarActor)
        {
            if (const APawn* Pawn = Cast<APawn>(Props.Source.AvatarActor))
            {
                Props.Source.Controller = Pawn->GetController();
            }
        }

        if (Props.Source.Controller)
        {
           Props.Source.Character = Cast<ACharacter>(Props.Source.Controller->GetPawn());
        }
    }

    // Target = target of the effect (owner of this AbilitySystem)
    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        Props.Target.AvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
        Props.Target.Controller = Data.Target.AbilityActorInfo->PlayerController.Get();
        Props.Target.Character = Cast<ACharacter>(Props.Target.AvatarActor);
        Props.Target.ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.Target.AvatarActor);
    }
}

void UAuraAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
    Super::PostGameplayEffectExecute(Data);

    FEffectProperties Props;
    SetEffectProperties(Data, Props);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMaxMana);
}
