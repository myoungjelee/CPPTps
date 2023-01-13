// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TpsPlayer.generated.h"

UCLASS()
class CPPTPS_API ATpsPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATpsPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		class USpringArmComponent* springArm;

	UPROPERTY(EditAnywhere)
		class UCameraComponent* camera;

	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* Rifle;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* Sniper;

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
	float X;
	float Y;
	float MouseX;
	float MouseY;
	float walkSpeed = 350;
	float runSpeed = 1500;
	bool bFire = false;
	float currCamShakeTime = 0;
	float camShakeTime = 0.5f;

public:
	void InputHorizontal(float value);
	void InputVertical(float value);
	void InputTurn(float value);
	void InputLookUp(float value);
	void InputJump();
	void InputFire();
	void ChangeWeapon(bool usesniper);
	void InputRifle();
	void InputSniper();
	void InputCtrl();
	void OutputCtrl();
	void InputRun();
	void MoveAction(float DeltaTime);
	void RotateAction();
	void CamShake();
};
