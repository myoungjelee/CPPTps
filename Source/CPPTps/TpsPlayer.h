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
		class UPlayerMove* compPlayerMove;

	UPROPERTY(EditAnywhere)
		class UPlayerAttack* compPlayerAttack;

		float currHP;
		float maxHP = 1;
		float prevHP;
		float bUpdateHP = false;
		float ratioHP = 0;

		void ReceivDamage(float damage);
		UPROPERTY(EditAnywhere)
		TSubclassOf<class UMainUI> mainUIFactory;
		UPROPERTY(EditAnywhere)
		class UMainUI* mainUI;

		UPROPERTY()
		TSubclassOf <class AMiniMap> miniFactory;

		UPROPERTY()
		TSubclassOf<class UGameOverUI> gameOverUIFactory;
};
