// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 *
 */
UCLASS()
class CPPTPS_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
public:
	UPROPERTY()
		class UProgressBar* currHPBar;
	UPROPERTY()
		class UTextBlock* currHP;
	UPROPERTY()
		class UImage* rifle;
	UPROPERTY()
		class UImage* sniper;
public:
	void UpdateCurrHp(float curr, float max);
	void UpdateWeapon(bool useSniper);
	void SetZorderWeapon(bool bFocus, class UImage* img);
};
