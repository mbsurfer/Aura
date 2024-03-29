


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "Game/AuraGameModeBase.h"
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

//todo: this function repeats a lot of code from GetOverlayWidgetController(). maybe we can write a helper function that builds the controller params?
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
    UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

    // Make sure that we add a SOURCE object to our effect handle, some effects will reference the source object
    // Forgetting this step will lead to a nullptr reference
    const AActor* AvatarActor = ASC->GetAvatarActor();
    FGameplayEffectContextHandle AttributesContextHandle = ASC->MakeEffectContext();
    AttributesContextHandle.AddSourceObject(AvatarActor);
    
    const FGameplayEffectSpecHandle PrimaryAttributesSpecHandel = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, AttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandel.Data.Get());
    
    const FGameplayEffectSpecHandle SecondaryAttributesSpecHandel = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, AttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandel.Data.Get());

    const FGameplayEffectSpecHandle VitalAttributesSpecHandel = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, AttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandel.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
    UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    for (const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
    {
        ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
    }
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
    const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    return (AuraGameMode == nullptr) ? nullptr : AuraGameMode->CharacterClassInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandel)
{
    if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandel.Get()))
    {
        return AuraEffectContext->IsBlockedHit();
    }
    return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandel)
{
    if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandel.Get()))
    {
        return AuraEffectContext->IsCriticalHit();
    }
    return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandel, bool bIsBlockedHit)
{
    if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandel.Get()))
    {
        AuraEffectContext->SetIsBlockedHit(bIsBlockedHit);
    }
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandel, bool bIsCriticalHit)
{
    if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandel.Get()))
    {
        AuraEffectContext->SetIsCriticalHit(bIsCriticalHit);
    }
}