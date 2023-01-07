// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilityTypes.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct UTILITYAI_API FAIData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FName DataName;

private:
	float TimeToDecay;
	float DecayInterpSpeed;
	float DecayGoal;
	bool bCanTick;

	float RawValue;
	float LastUpdateElapsedTime;

public:
	FAIData()
	{
	}

	FAIData(const FName& InDataName, float InDecayTime, float InDecaySpeed = 30.f, float InDecayGoal = 0.f, bool bInCanTick = true)
		:
		DataName(InDataName),
		TimeToDecay(InDecayTime),
		DecayInterpSpeed(InDecaySpeed),
		DecayGoal(InDecayGoal),
		bCanTick(bInCanTick)
	{
		RawValue = 0.f;
		LastUpdateElapsedTime = 0.f;
	}

	void SetRawValue(float DeltaValue, bool bReplace = false, bool bResetElapsedTime = true);
	FORCEINLINE float GetRawValue() const { return RawValue; }

	void TickData(float XDeltaTime);
	FString ToString() const;
};

USTRUCT(BlueprintType)
struct UTILITYAI_API FAIEvaluation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName UsedDataName;
	UPROPERTY(EditDefaultsOnly)
	float MinRawValue;
	UPROPERTY(EditDefaultsOnly)
	float MaxRawValue;
	UPROPERTY(EditDefaultsOnly)
	float ImportanceMultiplier;
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> EvaluationCurve;

private:
	float RealValue;
	float MaxRaw_Inv;
	bool bMaxRawInverseSet;

public:
	FAIEvaluation()
		:
		MinRawValue(0.f),
		MaxRawValue(1.f),
		ImportanceMultiplier(1.f),
		bMaxRawInverseSet(false)
	{
	}

	void SetRealValue(float InRawValue);
	float GetMaxRawInverse();
	FORCEINLINE float GetRealValue() const { return RealValue; }

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct UTILITYAI_API FDecisionInterruptData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName InterruptiveDecision;
	/*Minimal difference between the score of this decision and the one that interrupts it*/
	UPROPERTY(EditDefaultsOnly)
	float MinInterruptDifference;

	bool operator==(const FDecisionInterruptData& rhs) const
	{
		return InterruptiveDecision == rhs.InterruptiveDecision;
	}

public:
	FDecisionInterruptData()
		:
		InterruptiveDecision(NAME_None)
	{
	}

	FDecisionInterruptData(const FName& InDecisionName)
		:
		InterruptiveDecision(InDecisionName)
	{
	}
};

USTRUCT(BlueprintType)
struct UTILITYAI_API FAIDecision
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DecisionName;
	UPROPERTY(EditDefaultsOnly)
	TArray<FAIEvaluation> AdditiveEvaluations;
	UPROPERTY(EditDefaultsOnly)
	TArray<FAIEvaluation> MultiplicativeEvaluations;
	UPROPERTY(EditDefaultsOnly)
	float FinalMultiplier;
	/*How much time must pass before this decision is interrupted*/
	UPROPERTY(EditDefaultsOnly)
	float MinDuration;
	UPROPERTY(EditDefaultsOnly)
	TArray<FDecisionInterruptData> InterruptData;

private:
	float FinalScore;

public:
	FAIDecision()
		:
		FinalMultiplier(1.f),
		MinDuration(0.2f)
	{
	}

	void SetFinalScore();
	void OverrideFinalScore(float InNewScore);
	FORCEINLINE float GetFinalScore() const { return FinalScore; }

	bool operator==(const FAIDecision& rhs) const
	{
		return DecisionName == rhs.DecisionName;
	}
	bool operator>(const FAIDecision& rhs) const
	{
		return FinalScore > rhs.FinalScore;
	}
	bool operator<(const FAIDecision& rhs) const
	{
		return FinalScore < rhs.FinalScore;
	}
	bool operator>=(const FAIDecision& rhs) const
	{
		return FinalScore >= rhs.FinalScore;
	}
	bool operator<=(const FAIDecision& rhs) const
	{
		return FinalScore <= rhs.FinalScore;
	}

	FString ToString() const;
};