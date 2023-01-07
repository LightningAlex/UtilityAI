// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityTypes.h"

void FAIData::SetRawValue(float DeltaValue, bool bReplace /*= false*/, bool bResetElapsedTime /*= true*/)
{
	RawValue = bReplace ? DeltaValue : RawValue + DeltaValue;

	if (bResetElapsedTime) { LastUpdateElapsedTime = 0.f; }
}

void FAIData::TickData(float XDeltaTime)
{
	if (bCanTick)
	{
		LastUpdateElapsedTime = FMath::Min(LastUpdateElapsedTime + XDeltaTime, UE_BIG_NUMBER);

		if (DecayInterpSpeed > 0.f && LastUpdateElapsedTime > TimeToDecay)
		{
			const float NewRaw = FMath::FInterpConstantTo(RawValue, DecayGoal, XDeltaTime, DecayInterpSpeed);
			SetRawValue(NewRaw, true, false);
		}
	}
}

FString FAIData::ToString() const
{
	FString RetStr = DataName.ToString();
	RetStr.Append(", raw value: ");
	RetStr.Append(FString::SanitizeFloat(RawValue));
	return RetStr;
}

void FAIEvaluation::SetRealValue(float InRawValue)
{
	const float NormalizedReal = FMath::Clamp(InRawValue * GetMaxRawInverse(), 0.f, 1.f);
	if (EvaluationCurve.IsNull())
	{
		RealValue = NormalizedReal;
	}
	else
	{
		if (!EvaluationCurve.IsValid())
		{
			EvaluationCurve.LoadSynchronous();
		}
		RealValue = EvaluationCurve->GetFloatValue(NormalizedReal);
	}
	RealValue *= ImportanceMultiplier;
}

float FAIEvaluation::GetMaxRawInverse()
{
	if (!bMaxRawInverseSet)
	{
		MaxRaw_Inv = MaxRawValue == 0.f ? 0.f : (1.f / MaxRawValue);
		bMaxRawInverseSet = true;
	}
	return MaxRaw_Inv;
}

FString FAIEvaluation::ToString() const
{
	FString RetVal = UsedDataName.ToString();
	RetVal.Append(": ");
	RetVal.Append(FString::SanitizeFloat(RealValue));
	return RetVal;
}

void FAIDecision::SetFinalScore()
{
	float AdditiveScore = AdditiveEvaluations.Num() == 0 ? 1.f : 0.f;
	for (const FAIEvaluation& ObservedEvaluation : AdditiveEvaluations)
	{
		AdditiveScore += ObservedEvaluation.GetRealValue();
	}

	float MultiplicativeScore = 1.f;
	for (const FAIEvaluation& ObservedEvaluation : MultiplicativeEvaluations)
	{
		MultiplicativeScore *= ObservedEvaluation.GetRealValue();
	}

	FinalScore = AdditiveScore * MultiplicativeScore * FinalMultiplier;
}

void FAIDecision::OverrideFinalScore(float InNewScore)
{
	FinalScore = InNewScore;
}

FString FAIDecision::ToString() const
{
	FString RetVal = DecisionName.ToString();
	RetVal.Append(", ");
	for (const FAIEvaluation& Additive : AdditiveEvaluations)
	{
		RetVal.Append(Additive.ToString());
		RetVal.Append(", ");
	}

	for (const FAIEvaluation& Multi : MultiplicativeEvaluations)
	{
		RetVal.Append(Multi.ToString());
		RetVal.Append(", ");
	}
	RetVal.Append("||");
	return RetVal;
}
