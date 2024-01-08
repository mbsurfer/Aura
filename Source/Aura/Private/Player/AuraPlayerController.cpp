


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
    Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
    CursorTrace();
    AutoRun();
}

void AAuraPlayerController::AutoRun()
{
    if (!bAutoRunning)
    {
        return;
    }
    
    if (APawn* ControlledPawn = GetPawn())
    {
        const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
        const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
        ControlledPawn->AddMovementInput(Direction);

        // See if character has entered the auto-run acceptance radius
        const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
        if (DistanceToDestination <= AutoRunAcceptanceRadius)
        {
            bAutoRunning = false;
        }
    }
}

void AAuraPlayerController::CursorTrace()
{
    GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit)
    {
        return;
    }

    LastActor = ThisActor;
    ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

    // Check if player has targeted a new actor
    if (ThisActor != LastActor)
    {
        if (ThisActor)
        {
            ThisActor->HighlightActor();
        }

        if (LastActor)
        {
            LastActor->UnHighlightActor();
        }
    }
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    // LMB is a special input tag that is used for character movement
    if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        // check is the player's cursor is hovering over an enemy target
        bTargeting = ThisActor ? true : false;

        // we don't know if this is a long press yet
        bAutoRunning = false;
    }
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{

    // Let the ability system know that the input was released
    if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
    
    // If not LMB, our job here is done. LMB also deals with character movement
    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        return;
    }

    // This is the condition where the player is trying to use click-to-move
    if (!bTargeting && !bShiftKeyDown)
    {
        const APawn* ControlledPawn  = GetPawn();
        if (FollowTime <= ShortPressedThreshold && ControlledPawn)
        {
            // Create a path to the click
            if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
            {
                // Update spline points to destination
                Spline->ClearSplinePoints();
                for (const FVector& PointLoc : NavPath->PathPoints)
                {
                    Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
                }
                // Handle if the intended destination point is not available in the NavMesh
                CachedDestination = NavPath->PathPoints.Last();
                bAutoRunning = true;
            }
        }

        // Reset movement variables
        FollowTime = 0.f;
        bTargeting = false;
    }
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    // LMB is a special input tag that is used for character movement
    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagHeld(InputTag);
        }
        return;
    }

    // Is player targeting an enemy while holding LMB?
    // If TRUE: Try to trigger the relative ability
    // If FALSE: Move
    if (bTargeting || bShiftKeyDown)
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagHeld(InputTag);
        }
    }
    else
    {
        FollowTime += GetWorld()->GetDeltaSeconds();
        
        if (CursorHit.bBlockingHit)
        {
            CachedDestination = CursorHit.ImpactPoint;
        }

        if (APawn* ControlledPawn  = GetPawn())
        {
            const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
            ControlledPawn->AddMovementInput(WorldDirection);
        }
    }
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
    if (AuraAbilitySystemComponent == nullptr)
    {
        AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
    }
    return AuraAbilitySystemComponent;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
    check(AuraContext);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    
    // The subsystem is only available for local players
    if (Subsystem)
    {
        Subsystem->AddMappingContext(AuraContext, 0);
    }
   

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

    UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
    AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
    AuraInputComponent->BindAbilityAction(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (APawn* ControlledPawn = GetPawn<APawn>())
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
}