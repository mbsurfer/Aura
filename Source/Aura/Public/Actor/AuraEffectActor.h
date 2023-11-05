

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	None,
	RemoveOnEndOverlap
};

USTRUCT(BlueprintType)
struct FGameplayEffectParams
{
	GENERATED_BODY()

	FGameplayEffectParams() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects");
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects");
	EEffectApplicationPolicy GameplayEffectApplicationPolicy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects");
	EEffectRemovalPolicy GameplayEffectRemovalPolicy;
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:

	AAuraEffectActor();

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, FGameplayEffectParams GameplayEffect);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects");
	bool bDestroyOnEffectRemoval = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects");
	TArray<FGameplayEffectParams> InstantGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects");
	TArray<FGameplayEffectParams> DurationGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects");
	TArray<FGameplayEffectParams> InfiniteGameplayEffects;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects");
	float ActorLevel = 1.f;

};
