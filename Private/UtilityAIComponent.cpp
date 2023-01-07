// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityAIComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UtilityAIController.h"

UUtilityAIComponent::UUtilityAIComponent()
	:
	CurrentState("Idle")
{
	PrimaryComponentTick.bCanEverTick = true;

	AddNewData("LastDecisionElapsedTime", 0.f, 0.f, 0.f, false);
	AddNewData("HasTarget", 0.f, 0.f, 0.f, false);
	AddNewData("TargetAwareness", 10.f, 0.f, 0.f, false); /*This one is used to store the highest awareness of an observed target when the AI doesn't have an ACTIVE target!*/
	AddNewData("DistanceToTarget", 0.f, 0.f, 0.f, false);
	AddNewData("TargetDirectionDot", 0.f, 0.f, 0.f, false);
	AddNewData("DistanceFromOrigin", 0.f, 0.f, 0.f, false);
	AddNewData("Health", 0.f, 0.f, 0.f, false);
	AddNewData("Mana", 0.f, 0.f, 0.f, false);
	AddNewData("Stamina", 0.f, 0.f, 0.f, false);
	AddNewData("TargetHealth", 0.f, 0.f, 0.f, false);
	AddNewData("TargetMana", 0.f, 0.f, 0.f, false);
	AddNewData("TargetStamina", 0.f, 0.f, 0.f, false);
	AddNewData("ImmediateThreat", 1.f, 50.f);
	AddNewData("AuditoryAwareness", 10.f);
	AddNewData("UnseenThreat", 15.f);
}

void UUtilityAIComponent::BeginPlay()
{
	Super::BeginPlay();

}

bool UUtilityAIComponent::CanPerformDecision(const FAIDecision& InDecision)
{
	AUtilityAIController* UAICont = Cast<AUtilityAIController>(GetOwner());
	return UAICont && UAICont->CanPerformDecision(InDecision);
}

void UUtilityAIComponent::MakeDecision()
{
	FDecisionCollection* UsedCollection = StateDecisions.Find(CurrentState);
	if (!UsedCollection)
	{
		UE_LOG(LogTemp, Error, TEXT("No collection found for state: %s"), *CurrentState.ToString());
		return;
	}

	UpdateDecisionCollection(*UsedCollection);
	UsedCollection->Decisions.Sort(); /*Maybe work on a copy of this array?*/

	const float TotalBestScore = UsedCollection->Decisions.Top().GetFinalScore();
	if (TotalBestScore == 0.f) { return; }

	UpdateAIData("LastDecisionElapsedTime", 0.f, true);
	if (UsedCollection->MaxRandomDifference > 0.f)
	{
		float CrrntBestScore = TotalBestScore;
		TArray<FAIDecision> BestDecisions;
		for (int32 i = UsedCollection->Decisions.Num() - 1; i >= 0; i--)
		{
			if (i == UsedCollection->Decisions.Num() - 1)
			{
				BestDecisions.Add(UsedCollection->Decisions[i]);
				continue;
			}

			const float ObservedScore = UsedCollection->Decisions[i].GetFinalScore();
			if (ObservedScore > 0.f && CrrntBestScore - ObservedScore >= UsedCollection->MaxRandomDifference)
			{
				BestDecisions.Add(UsedCollection->Decisions[i]);
			}
		}

		const int32 UsedIndex = FMath::RandRange(0, BestDecisions.Num() - 1);
		SetCurrentDecision(BestDecisions[UsedIndex]);
	}
	else
	{
		SetCurrentDecision(UsedCollection->Decisions.Top());
	}
}

void UUtilityAIComponent::SetCurrentDecision(const FAIDecision& InDecision)
{
	const FName OldDec = CurrentDecision.DecisionName;
	CurrentDecision = InDecision;
	
	TryUpdateBlackboard();
	OnDecisionMade.Broadcast(OldDec, InDecision.DecisionName);
}

void UUtilityAIComponent::TryInterruptDecision()
{
	const int32 ETimeIndex = GetAIData("LastDecisionElapsedTime");
	if (ETimeIndex == -1) { return; }

	const float ETime = AIData[ETimeIndex].GetRawValue();

	FDecisionCollection* UsedCollection = StateDecisions.Find(CurrentState);
	if (!UsedCollection) { return; }

	TArray<FAIDecision> BestDecisions;
	for (FAIDecision& ObservedDecision : UsedCollection->Decisions)
	{
		const int32 InterruptorIndex = CurrentDecision.InterruptData.Find(FDecisionInterruptData(ObservedDecision.DecisionName));
		if (CurrentDecision.InterruptData.IsValidIndex(InterruptorIndex) && ETime > CurrentDecision.InterruptData[InterruptorIndex].MinInterruptDifference)
		{
			UpdateSingleDecision(ObservedDecision);
			if (ObservedDecision.GetFinalScore() > 0.f)
			{
				BestDecisions.Add(ObservedDecision);
			}
		}
	}

	if (BestDecisions.Num() == 0) { return; }
	const int32 LastIndex = BestDecisions.Num() - 1;
	SetCurrentDecision(BestDecisions[FMath::RandRange(0, LastIndex)]);
}

void UUtilityAIComponent::UpdateDecisionCollection(FDecisionCollection& InCollection)
{
	for (FAIDecision& Decision : InCollection.Decisions)
	{
		UpdateSingleDecision(Decision);
	}
}

void UUtilityAIComponent::UpdateSingleDecision(FAIDecision& InDecision)
{
	if (!CanPerformDecision(InDecision))
	{
		InDecision.OverrideFinalScore(0.f);
		return;
	}
	for (FAIEvaluation& AIEval : InDecision.AdditiveEvaluations)
	{
		const int32 FoundDataIndex = GetAIData(AIEval.UsedDataName);
		if (FoundDataIndex != -1)
		{
			AIEval.SetRealValue(AIData[FoundDataIndex].GetRawValue());
		}
	}

	for (FAIEvaluation& AIEval : InDecision.MultiplicativeEvaluations)
	{
		const int32 FoundDataIndex = GetAIData(AIEval.UsedDataName);
		if (FoundDataIndex != -1)
		{
			AIEval.SetRealValue(AIData[FoundDataIndex].GetRawValue());
		}
	}
	InDecision.SetFinalScore();
}

void UUtilityAIComponent::TryUpdateBlackboard()
{
	AAIController* AICont = Cast<AAIController>(GetOwner());
	if (AICont && AICont->GetBlackboardComponent())
	{
		AICont->GetBlackboardComponent()->SetValueAsName("TaskKey", CurrentDecision.DecisionName);
	}
}

void UUtilityAIComponent::UpdateAIData(FName InDataName, float NewVal, bool bReplace /*= false*/)
{
	const int32 DataIndex = GetAIData(InDataName);
	if (DataIndex != -1)
	{
		AIData[DataIndex].SetRawValue(NewVal, bReplace, false);
	}
}

int32 UUtilityAIComponent::GetAIData(const FName& InDataName)
{
	int32* DataPtr = CachedDataIndexes.Find(InDataName);
	if (DataPtr) 
	{ 
		return *DataPtr;
	}

	for (int32 i = 0; i < AIData.Num(); i++)
	{
		if (AIData[i].DataName == InDataName)
		{
			CachedDataIndexes.Add(InDataName, i);
			return i;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("AI data %s not found!"), *InDataName.ToString());
	return -1;
}

float UUtilityAIComponent::GetAIDataValue(const FName& InDataName)
{
	int32 DI = GetAIData(InDataName);
	if (DI != -1) { return AIData[DI].GetRawValue(); }

	return -1.f;
}

void UUtilityAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateAIData("LastDecisionElapsedTime", DeltaTime);

	for (FAIData& ObservedData : AIData)
	{
		ObservedData.TickData(DeltaTime);
	}

	TryInterruptDecision();
}

void UUtilityAIComponent::SetNewState(const FName& NewState, bool bMakeDecision /*= true*/)
{
	const FName OldState = CurrentState;
	CurrentState = NewState;
	if (bMakeDecision)
	{
		MakeDecision();
	}

	OnStateChanged.Broadcast(OldState, NewState);
}

void UUtilityAIComponent::AddNewData(const FName& InDataName, float InDecayTime, float InDecaySpeed /*= 30.f*/, float InDecayGoal /*= 0.f*/, bool bCanTick /*= true*/, bool bCache /*= true*/)
{
	const int32 AddedIndex = AIData.Add(FAIData(InDataName, InDecayTime, InDecaySpeed, InDecayGoal, bCanTick));
	if (bCache) { CachedDataIndexes.Add(InDataName, AddedIndex); }
}

