


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{
    // How often the game server will update clients (per second)
	NetUpdateFrequency = 100.f;

    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);

    /*
        Mixed Replication Mode
        ======================
        Gameplay Effects are replicated to the onwing client only.
        Gameplay Cues and Gameply Tags replicated to all clients.
        
        Use Case: Multiplayer, Player-Controlled

        1. The OwnerActor's Owner must be the Controller. For Pawns, this is set automatically in PossessedBy().
        2. The PlayerState's Owner is automatically set to the Controller (current use case for this project).
        3. If your OwnerActor is not the PlayerState, and you use Mixed Replication Mode, then you must call SetOwner()
           on the OwnerActor to set its owner to the Controller.
    */
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAuraPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}