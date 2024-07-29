// Fill out your copyright notice in the Description page of Project Settings.


#include "DaPlayerState.h"

ADaPlayerState::ADaPlayerState()
{
	Credits = 0.f;
}

float ADaPlayerState::GetCredits() const
{
	return Credits;
}

void ADaPlayerState::AdjustCredits(float Delta)
{
	Credits += Delta;
	OnCreditsChanged.Broadcast(GetInstigator(), Credits, Delta);
}

