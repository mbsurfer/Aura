


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Scene Root"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* Target, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// User helper function provided by UAbilitySystemBlueprintLibrary to get the Ability System Component form the target Actor
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);

	// If the target actor has not implimented an ASC, do nothing
	if (!TargetASC)
	{
		return;
	}

	// We have problems if the GameplayEffectClass has not been set and should abort
	check(GameplayEffectClass)

	// What is a GameplayEffectContextHandle??
	// A lightweight wrapper that stores the GameplayEffectContext as a prt called Data.
	// Mostly consists of helper functions when dealing with the context.
	FGameplayEffectContextHandle GameplayEffectContextHandle = TargetASC->MakeEffectContext();

	// Remember what applied this effect
	GameplayEffectContextHandle.AddSourceObject(this);

	// FGameplayEffectSpecHandle is another type of wrapper that holds and EffectSpec.
	// Also contains a data member that stores the actual struct.
	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, GameplayEffectContextHandle);

	// Remember to derefrence the EffectSpec pointer!
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}
