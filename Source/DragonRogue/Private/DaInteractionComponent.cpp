// Fill out your copyright notice in the Description page of Project Settings.


#include "DaInteractionComponent.h"
#include "DaGameplayInterface.h"
#include "DaWorldUserWidget.h"

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("da.InteractionDebugDraw"), false, TEXT("Enable Debug Lines For Interact Component."), ECVF_Cheat);

UDaInteractionComponent::UDaInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;

	CollisionChannel = ECC_WorldDynamic;
}

void UDaInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDaInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* MyPawn = Cast<APawn>(GetOwner());

	if(MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
}

void UDaInteractionComponent::FindBestInteractable()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	
	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	
	TArray<FHitResult> Hits;
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clear Focused Actor before hit
	FocusedActor = nullptr;
	
	for (FHitResult Hit : Hits)
	{
		if (bDebugDraw)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 1.0f);
		}
		
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<UDaGameplayInterface>())
			{
				FocusedActor = HitActor;
				break;
			}
		}
	}

	// focus effect - loads a custom widget onto interactable
	if (FocusedActor)
	{
		// Lazily load the widget when its first needed
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<UDaWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
				if (bDebugDraw)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Added Widget to Viewport.");
				}
			}
		}
	}
	else
	{
		if (DefaultWidgetInstance && DefaultWidgetInstance->IsInViewport())
		{
			DefaultWidgetInstance->RemoveFromParent();
			if (bDebugDraw)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Removed Widget to Viewport.");
			}
		}
	}
	
	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 1.0f, 0, 2.0f);
	}
}


void UDaInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void UDaInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if (InFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "DaInteractionComponent: No FocusedActor to Interact.");
		return;
	}
	APawn* MyPawn = Cast<APawn>(GetOwner());
	IDaGameplayInterface::Execute_Interact(InFocus, MyPawn);
}