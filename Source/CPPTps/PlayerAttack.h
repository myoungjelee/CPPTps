// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerAttack.generated.h"

/**
 *
 */
UCLASS()
class CPPTPS_API UPlayerAttack : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	UPlayerAttack();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) override;
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> Scope;
	UPROPERTY()
		class UUserWidget* ScopeUI;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> Common;
	UPROPERTY()
		class UUserWidget* CommonUI;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ABullet> BulletFactory;
	UPROPERTY(EditAnywhere)
		class UParticleSystem* Effect;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCameraShakeBase> cameraShake;
public:
	bool bFire = false;
	float currCamShakeTime = 0;
	float camShakeTime = 0.5f;
public:
	void InputFire();
	void ChangeWeapon(bool usesniper);
	void InputRifle();
	void InputSniper();
	void InputCtrl();
	void OutputCtrl();
	void CamShake();
};
