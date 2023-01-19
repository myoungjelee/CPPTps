// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "EnemyAnim.generated.h"

/**
 *
 */
UCLASS()
class CPPTPS_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeBeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EEnemyState state;
	UPROPERTY()
		class AEnemy* enemy;

	UFUNCTION()
		void AnimNotify_DieEnd();
	UFUNCTION()
		void AnimNotify_Attack();
};


