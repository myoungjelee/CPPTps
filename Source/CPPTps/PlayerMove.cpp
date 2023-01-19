// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"
#include "TpsPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	//Controller �� ȸ������ ���� �� �Ӽ� ����
	me->bUseControllerRotationYaw = true;
	me->springArm->bUsePawnControlRotation = true;

	//���� Ƚ���� 2���� ����
	me->JumpMaxCount = 2;

	//�����̴� �ӵ��� 1000���� ����
	me->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	//�����ϴ� ���̸� 600���� ����
	me->GetCharacterMovement()->JumpZVelocity = 600;

	//ī�޶� ���� ȸ���� �ּҰ��� �ִ밪 ����
	APlayerController* playerController = Cast<APlayerController>(me->Controller);
	playerController->PlayerCameraManager->ViewPitchMin = -45;
	playerController->PlayerCameraManager->ViewPitchMax = 45;
}

void UPlayerMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//�̵�
	MoveAction(DeltaTime);
	//ȸ��
	//RotateAction();
}

void UPlayerMove::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);


	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &UPlayerMove::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &UPlayerMove::InputVertical);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &UPlayerMove::InputTurn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UPlayerMove::InputLookUp);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UPlayerMove::InputJump);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &UPlayerMove::InputRun);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &UPlayerMove::InputRun);
}

void UPlayerMove::MoveAction(float DeltaTime)
{
	FVector p0 = me->GetActorLocation();
	FVector dir = me->GetActorForwardVector() * X + me->GetActorRightVector() * Y;
	FVector vt = walkSpeed * dir.GetSafeNormal() * DeltaTime;

	me->AddMovementInput(dir); //FVector p = p0 + vt;  //SetActorLocation(p); 


	/*FVector p0 = GetActorLocation();
	FVector dirX = GetActorForwardVector() * X;
	FVector dirY = GetActorRightVector()* Y;
	FVector dir = dirX + dirY;
	dir.Normalize();
	FVector p = p0 + dir * speed * DeltaTime;
	SetActorLocation(p);*/
}

void UPlayerMove::RotateAction()
{
	me->SetActorRotation(FRotator(0, MouseX, 0));
	/*FRotator yaw = GetActorRotation();
	yaw.Yaw = MouseX;
	SetActorRotation(yaw);*/

	me->springArm->SetRelativeRotation(FRotator(-MouseY, 0, 0));
	//FRotator pit;
	//pit.Pitch = -MouseY;
	//springArm->SetRelativeRotation(pit);
}

void UPlayerMove::InputHorizontal(float value)
{
	Y = value;
}

void UPlayerMove::InputVertical(float value)
{
	X = value;
}

void  UPlayerMove::InputRun()
{
	if (me->GetCharacterMovement()->MaxWalkSpeed > walkSpeed)
	{
		me->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		me->GetCharacterMovement()->MaxWalkSpeed = runSpeed;
	}
}

void UPlayerMove::InputTurn(float value)
{
	me->AddControllerYawInput(value);
	//MouseX += value;
}
void UPlayerMove::InputLookUp(float value)
{
	me->AddControllerPitchInput(value);
	//MouseY += value;
	//MouseY ����  �ּ� -45, �ִ� 45 �� �����Ѵ�
	//MouseY = FMath::Clamp(MouseY, -45, 45);
	////���࿡ MouseY �� -45�� ���� �۴ٸ�
	//if (MouseY < -45)
	//{
	//	// MouseY�� - 45���� ����
	//	MouseY = -45;
	//}
	////���࿡ MouseY �� 45�� ���� ũ�ٸ�
	//if (MouseY > 45)
	//{
	//	// MouseY��  45���� ����
	//	MouseY = 45;
	//}
}

void UPlayerMove::InputJump()
{
	me->Jump();
}