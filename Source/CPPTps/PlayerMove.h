// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerMove.generated.h"

/**
 *
 */
UCLASS()
class CPPTPS_API UPlayerMove : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) override;
public:
	float walkSpeed = 350;
	float runSpeed = 1500;
	float X;
	float Y;
	float MouseX;
	float MouseY;
public:
	void MoveAction(float DeltaTime);
	void RotateAction();
	void InputHorizontal(float value);
	void InputVertical(float value);
	void InputRun();
	void InputTurn(float value);
	void InputLookUp(float value);
	void InputJump();
};
