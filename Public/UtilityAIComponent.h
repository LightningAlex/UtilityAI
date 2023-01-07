// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UtilityTypes.h"
#include "UtilityAIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDecisionMade, FName, OldDecision, FName, NewDecision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, FName, OldState, FName, NewState);

USTRUCT(BlueprintType)
struct UTILITYAI_API FDecisionCollection
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAIDecision> Decisions;
	/*When making a choice, the AI will randomly pick a decision of which the difference
	between the score and the max score is smaller than this. Keep <= 0.f to skip this random selection.*/
	UPROPERTY(EditDefaultsOnly)
	float MaxRandomDifference;

public:
	FDecisionCollection()
		:MaxRandomDifference(0.f)
	{
	}

	FString ToString() const
	{
		FString RetVal;
		for (const FAIDecision& Dec : Decisions)
		{
			RetVal.Append(Dec.ToString());
		}
		return RetVal;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTILITYAI_API UUtilityAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FDecisionCollection> StateDecisions;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FAIData> AIData;

	UPROPERTY(BlueprintAssignable)
	FOnDecisionMade OnDecisionMade;
	UPROPERTY(BlueprintAssignable)
	FOnStateChanged OnStateChanged;

private:
	FName CurrentState;
	FAIDecision CurrentDecision;
	TMap<FName, int32> CachedDataIndexes;

public:	
	UUtilityAIComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	int32 GetAIData(const FName& InDataName);
	UFUNCTION(BlueprintCallable)
	float GetAIDataValue(const FName& InDataName);

	UFUNCTION(BlueprintCallable)
	void SetNewState(const FName& NewState, bool bMakeDecision = true);
	UFUNCTION(BlueprintPure)
	FORCEINLINE FName GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable)
	void UpdateAIData(FName InDataName, float NewVal, bool bReplace = false);
	UFUNCTION(BlueprintCallable)
	void AddNewData(const FName& InDataName, float InDecayTime, float InDecaySpeed = 30.f, float InDecayGoal = 0.f, bool bCanTick = true, bool bCache = true);

	UFUNCTION(BlueprintPure)
	FORCEINLINE FName GetCurrentDecision() const { return CurrentDecision.DecisionName; }

	UFUNCTION(BlueprintCallable)
	void MakeDecision();

protected:
	virtual void BeginPlay() override;

private:
	bool CanPerformDecision(const FAIDecision& InDecision);

	void SetCurrentDecision(const FAIDecision& InDecision);
	void TryInterruptDecision();
	void UpdateDecisionCollection(FDecisionCollection& InCollection);
	void UpdateSingleDecision(FAIDecision& InDecision);
	
	void TryUpdateBlackboard();
};
