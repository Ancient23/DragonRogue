// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAttributeComponent.h"

// Sets default values
UDaAttributeComponent::UDaAttributeComponent()
{
	Health = 100.0f;
}



bool UDaAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	
	return true;
}



