// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAction.h"

#include "DragonRogue/DragonRogue.h"

void UDaAction::StartAction_Implementation(AActor* Instigator)
{
	LOG("Running %s", *GetNameSafe(this));
}

void UDaAction::StopAction_Implementation(AActor* Instigator)
{
	LOG("Stopped %s", *GetNameSafe(this));
}
