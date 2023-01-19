// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
#include <Components/TextBlock.h>
#include <Components/ProgressBar.h>
#include <Components/Image.h>
#include <Components/CanvasPanelSlot.h>

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();

	currHP = Cast<UTextBlock>(GetWidgetFromName(TEXT("_currHP")));
	currHPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("_currHPBar")));
	rifle = Cast<UImage>(GetWidgetFromName(TEXT("_Rifle")));
	sniper = Cast<UImage>(GetWidgetFromName(TEXT("_Sniper")));
}

void UMainUI::UpdateCurrHp(float curr, float max)
{
	float hp = curr / max;
	FString strHP = FString::Printf(TEXT("%d%%"), (int32)(hp * 100));
	currHP->SetText(FText::FromString(strHP));

	currHPBar->SetPercent(hp);
}

void UMainUI::UpdateWeapon(bool useSniper)
{
// 	if (useSniper)
// 	{
// 		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(rifle->Slot);
// 		slot->SetZOrder(0);
// 
// 
// 		slot = Cast<UCanvasPanelSlot>(sniper->Slot);
// 		slot->SetZOrder(2);
// 	}
// 	else
// 	{
// 		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(rifle->Slot);
// 		slot->SetZOrder(2);
// 
// 		slot = Cast<UCanvasPanelSlot>(sniper->Slot);
// 		slot->SetZOrder(0);
// 	}

	SetZorderWeapon(useSniper, sniper);
	SetZorderWeapon(!useSniper, rifle);
}

void UMainUI::SetZorderWeapon(bool bFocus, class UImage* img)
{
	UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(img->Slot);
	if (bFocus)
	{
		slot->SetZOrder(2);
	}
	else
	{
		slot->SetZOrder(0);
	}
}