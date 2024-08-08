// Fill out your copyright notice in the Description page of Project Settings.


#include "DaPlayerState.h"

#include "DragonRogue/DragonRogue.h"
#include "Net/UnrealNetwork.h"

ADaPlayerState::ADaPlayerState()
{
	Credits = 0.f;

	SetReplicates(true);
}

void ADaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADaPlayerState, Credits);
}

float ADaPlayerState::GetCredits() const
{
	return Credits;
}

void ADaPlayerState::AdjustCredits(float Delta)
{
	if (HasAuthority())
	{
		Credits += Delta;

		// even though using onRepNotify still have to trigger events on server
		OnCreditsChanged.Broadcast(GetInstigator(), Credits, Delta);
	}
}

void ADaPlayerState::OnRep_Credits(float OldCredits)
{
	float Delta = Credits-OldCredits;
	
	// LOG("PlayerState: %s gets %f credits", *GetNameSafe(this), Delta);
	// FString Msg = FString::Printf(TEXT("PlayerState: %s gets %f credits"), *GetNameSafe(this), Delta);
	// LogOnScreen(this, Msg, HasAuthority() ? FColor::Green : FColor::Red);
	
	OnCreditsChanged.Broadcast(GetInstigator(), Credits, Delta);
}

