// Fill out your copyright notice in the Description page of Project Settings.


#include "DaPlayerState.h"

#include "DaSaveGame.h"
#include "DragonRogue/DragonRogue.h"
#include "Net/UnrealNetwork.h"

ADaPlayerState::ADaPlayerState()
{
	Credits = 0;

	bReplicates = true;
}

void ADaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADaPlayerState, Credits);
}

bool ADaPlayerState::UpdatePersonalRecord(float NewTime)
{
	// Higher time is better
	if (NewTime > PersonalRecordTime)
	{
		float OldRecord = PersonalRecordTime;
		PersonalRecordTime = NewTime;
		OnRecordTimeChanged.Broadcast(this, PersonalRecordTime, OldRecord);
		return true;
	}
	return false;
}

int32 ADaPlayerState::GetCredits() const
{
	return Credits;
}

void ADaPlayerState::AdjustCredits(int32 Delta)
{
	if (HasAuthority())
	{
		Credits += Delta;

		// even though using onRepNotify still have to trigger events on server
		OnCreditsChanged.Broadcast(GetInstigator(), Credits, Delta);
	}
}

void ADaPlayerState::OnRep_Credits(int32 OldCredits)
{
	float Delta = Credits-OldCredits;
	
	// LOG("PlayerState: %s gets %f credits", *GetNameSafe(this), Delta);
	// FString Msg = FString::Printf(TEXT("PlayerState: %s gets %f credits"), *GetNameSafe(this), Delta);
	// LogOnScreen(this, Msg, HasAuthority() ? FColor::Green : FColor::Red);
	
	OnCreditsChanged.Broadcast(this, Credits, Delta);
}


void ADaPlayerState::LoadPlayerState_Implementation(UDaSaveGame* SaveObject)
{
	if (SaveObject)
	{
		FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
		if (FoundData)
		{
			// Makes sure we trigger credits changed event
			AdjustCredits(FoundData->Credits);
			PersonalRecordTime = FoundData->PersonalRecordTime;
		}
		else
		{
			LOG("Could not find SaveGame data for player id '%i'.", GetPlayerId());
		}
	}
}

void ADaPlayerState::SavePlayerState_Implementation(UDaSaveGame* SaveObject)
{
	if (SaveObject)
	{
		FPlayerSaveData SaveData;
		SaveData.Credits = Credits;
		SaveData.PersonalRecordTime = PersonalRecordTime;

		// Stored as FString for simplicity (original Steam ID is uint64)
		SaveData.PlayerID = GetUniqueId()->ToString();
		//SaveData.PlayerID = GetPlayerId();
		
		// May not be alive while we save
		if (APawn* MyPawn = GetPawn())
		{
			SaveData.Location = MyPawn->GetActorLocation();
			SaveData.Rotation = MyPawn->GetActorRotation();
			SaveData.bResumeAtTransform = true;
		}
		
		SaveObject->SavedPlayers.Add(SaveData);
	}
}