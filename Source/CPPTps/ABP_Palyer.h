// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABP_Palyer.generated.h"

/**
 *
 */
UCLASS()
class CPPTPS_API UABP_Palyer : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABP_Palyer();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dirV = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dirH = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bAir = false;

	class ATpsPlayer* me;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UAnimMontage* attackAnimMontage;

	void PlayAttackAnim();
};
