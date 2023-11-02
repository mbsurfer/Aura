


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Scene Root"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, FGameplayEffectParams GameplayEffect)
{
	// User helper function provided by UAbilitySystemBlueprintLibrary to get the Ability System Component form the target Actor
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	// If the target actor has not implimented an ASC, do nothing
	if (!TargetASC)
	{
		return;
	}

	// We have problems if the GameplayEffectClass has not been set and should abort
	check(GameplayEffect.GameplayEffectClass)

	// What is a GameplayEffectContextHandle??
	// A lightweight wrapper that stores the GameplayEffectContext as a prt called Data.
	// Mostly consists of helper functions when dealing with the context.
	FGameplayEffectContextHandle GameplayEffectContextHandle = TargetASC->MakeEffectContext();

	// Remember what applied this effect
	GameplayEffectContextHandle.AddSourceObject(this);

	// FGameplayEffectSpecHandle is another type of wrapper that holds and EffectSpec.
	// Also contains a data member that stores the actual struct.
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffect.GameplayEffectClass, 1.f, GameplayEffectContextHandle);

	// Remember to derefrence the EffectSpec pointer!
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	// Keep track of infinate effects (if they have a removal policy) so we know how to remove them in the future
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && GameplayEffect.GameplayEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}


void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	// Apply all instant overlap effects
	for (FGameplayEffectParams InstantGameplayEffect : InstantGameplayEffects)
	{
		if (InstantGameplayEffect.GameplayEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, InstantGameplayEffect);
		}
	}

	// Apply all duration overlap effects
	for (FGameplayEffectParams DurationGameplayEffect : DurationGameplayEffects)
	{
		if (DurationGameplayEffect.GameplayEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, DurationGameplayEffect);
		}
	}

	// Apply all infinite overlap effects
	for (FGameplayEffectParams InfiniteGameplayEffect : InfiniteGameplayEffects)
	{
		if (InfiniteGameplayEffect.GameplayEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, InfiniteGameplayEffect);
		}
	}
}


void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	// Apply all instant end overlap effects
	for (FGameplayEffectParams InstantGameplayEffect : InstantGameplayEffects)
	{
		if (InstantGameplayEffect.GameplayEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, InstantGameplayEffect);
		}
	}

	// Apply all duration end overlap effects
	for (FGameplayEffectParams DurationGameplayEffect : DurationGameplayEffects)
	{
		if (DurationGameplayEffect.GameplayEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, DurationGameplayEffect);
		}
	}

	// Apply all infinite end overlap effects
	for (FGameplayEffectParams InfiniteGameplayEffect : InfiniteGameplayEffects)
	{
		if (InfiniteGameplayEffect.GameplayEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, InfiniteGameplayEffect);
		}

		if (InfiniteGameplayEffect.GameplayEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			// Find and remove the infinite effect from the actor
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (!IsValid(TargetASC))
			{
				return;
			}

			for (auto HandlePair = ActiveEffectHandles.CreateIterator(); HandlePair; ++HandlePair)
			{
				if (TargetASC == HandlePair->Value)
				{
					TargetASC->RemoveActiveGameplayEffect(HandlePair->Key, 1);
					HandlePair.RemoveCurrent();
				}
			}
		}
	}
}