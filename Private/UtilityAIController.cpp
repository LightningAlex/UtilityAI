// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityAIController.h"
#include "UtilityAIComponent.h"

AUtilityAIController::AUtilityAIController(const FObjectInitializer& OI)
	:Super(OI),
	BehaviorTree(nullptr),
	StartingState("Idle")
{
	PrimaryActorTick.bStartWithTickEnabled = true;

	UtilityAIComponent = CreateDefaultSubobject<UUtilityAIComponent>(TEXT("UtilityAIComponent"));
}

void AUtilityAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AI_UpdateDistanceFromOrigin();
}

void AUtilityAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OriginalTransform = InPawn->GetActorTransform();
	const bool bBTRan = RunBehaviorTree(BehaviorTree);
	if (bBTRan)
	{
		UtilityAIComponent->SetNewState(StartingState);
	}
}

bool AUtilityAIController::CanPerformDecision_Implementation(const FAIDecision& InDecision)
{
	return true;
}

void AUtilityAIController::AI_UpdateDistanceToTarget_Implementation(const FVector& TgtLocation)
{
	const FVector DistVec = TgtLocation - GetPawn()->GetActorLocation();
	UtilityAIComponent->UpdateAIData("DistanceToTarget", DistVec.Size(), true);
}

void AUtilityAIController::AI_UpdateTargetDirectionDot_Implementation(float NewVal)
{
	UtilityAIComponent->UpdateAIData("TargetDirectionDot", NewVal, true);
}

void AUtilityAIController::AI_UpdateDistanceFromOrigin_Implementation()
{
	const FVector DistVec = OriginalTransform.GetLocation() - GetPawn()->GetActorLocation();
	UtilityAIComponent->UpdateAIData("DistanceFromOrigin", DistVec.Size(), true);
}

void AUtilityAIController::AI_UpdateHealth_Implementation(float NewVal)
{
	UtilityAIComponent->UpdateAIData("Health", NewVal, true);
}

void AUtilityAIController::AI_UpdateMana_Implementation(float NewVal)
{
	UtilityAIComponent->UpdateAIData("Mana", NewVal, true);
}

void AUtilityAIController::AI_UpdateStamina_Implementation(float NewVal)
{
	UtilityAIComponent->UpdateAIData("Stamina", NewVal, true);
}

void AUtilityAIController::AI_UpdateTargetHealth_Implementation(float NewVal)
{
	UtilityAIComponent->UpdateAIData("TargetHealth", NewVal, true);
}

void AUtilityAIController::AI_UpdateTargetMana_Implementation(float NewVal)
{
	UtilityAIComponent->UpdateAIData("TargetMana", NewVal, true);
}

void AUtilityAIController::AI_UpdateTargetStamina_Implementation(float NewVal)
{
	UtilityAIComponent->UpdateAIData("TargetStamina", NewVal, true);
}

void AUtilityAIController::AI_UpdateImmediateThreat_Implementation(float NewVal)
{
	UtilityAIComponent->UpdateAIData("ImmediateThreat", NewVal, true);
}

void AUtilityAIController::AI_UpdateAuditoryAwareness_Implementation(float NewVal, const FVector& NoiseSourceLocation)
{
	UtilityAIComponent->UpdateAIData("AuditoryAwareness", NewVal, false);
}

void AUtilityAIController::AI_UpdateUnseenThreat_Implementation(float NewVal, const FVector& ThreatLocation)
{
	UtilityAIComponent->UpdateAIData("UnseenThreat", 1.f, true);
}
