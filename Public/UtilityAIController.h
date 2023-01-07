// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UtilityTypes.h"
#include "UtilityAIController.generated.h"

class UUtilityAIComponent;
class UBehaviorTree;

UCLASS()
class UTILITYAI_API AUtilityAIController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UUtilityAIComponent* UtilityAIComponent;
	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere)
	FName StartingState;

private:
	FTransform OriginalTransform;

public:
	AUtilityAIController(const FObjectInitializer& OI);
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	UFUNCTION(BlueprintNativeEvent)
	bool CanPerformDecision(const FAIDecision& InDecision);

	UFUNCTION(BlueprintPure)
	FORCEINLINE const FTransform& GetOriginalTransform() const { return OriginalTransform; }

protected:
	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateDistanceToTarget(const FVector& TgtLocation);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateTargetDirectionDot(float NewVal);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateDistanceFromOrigin();

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateHealth(float NewVal);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateMana(float NewVal);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateStamina(float NewVal);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateTargetHealth(float NewVal);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateTargetMana(float NewVal);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateTargetStamina(float NewVal);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateImmediateThreat(float NewVal);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateAuditoryAwareness(float NewVal, const FVector& NoiseSourceLocation);

	UFUNCTION(BlueprintNativeEvent)
	void AI_UpdateUnseenThreat(float NewVal, const FVector& ThreatLocation);

};
