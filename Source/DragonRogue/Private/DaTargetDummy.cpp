// Fill out your copyright notice in the Description page of Project Settings.


#include "DaTargetDummy.h"

#include "DaAttributeComponent.h"

// Sets default values
ADaTargetDummy::ADaTargetDummy()
{
 	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	AttributeComp = CreateDefaultSubobject<UDaAttributeComponent>("AttributeComp");
	AttributeComp->OnHealthChanged.AddDynamic(this, &ADaTargetDummy::OnHealthChanged);

}

void ADaTargetDummy::OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}

