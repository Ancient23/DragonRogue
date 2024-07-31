// Fill out your copyright notice in the Description page of Project Settings.


#include "DaWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "DragonRogue/DragonRogue.h"
#include "Kismet/GameplayStatics.h"

void UDaWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();
		LOG_WARNING("AttachedActor no longer valid, removing UDaWorldUserWidget derived widget");
		return;
	}
	
	FVector2D ScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition))
	{
		float scale = UWidgetLayoutLibrary::GetViewportScale(this);

		ScreenPosition /= scale;

		if (ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}
}
