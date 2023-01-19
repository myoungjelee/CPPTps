// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"
#include "TpsPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	//Controller 의 회전값을 따라 갈 속성 세팅
	me->bUseControllerRotationYaw = true;
	me->springArm->bUsePawnControlRotation = true;

	//점프 횟수를 2개로 하자
	me->JumpMaxCount = 2;

	//움직이는 속도를 1000으로 하자
	me->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	//점프하는 높이를 600으로 하자
	me->GetCharacterMovement()->JumpZVelocity = 600;

	//카메라 상하 회전갑 최소값과 최대값 설정
	APlayerController* playerController = Cast<APlayerController>(me->Controller);
	playerController->PlayerCameraManager->ViewPitchMin = -45;
	playerController->PlayerCameraManager->ViewPitchMax = 45;
}

void UPlayerMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//이동
	MoveAction(DeltaTime);
	//회전
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
	//MouseY 값을  최소 -45, 최대 45 로 고정한다
	//MouseY = FMath::Clamp(MouseY, -45, 45);
	////만약에 MouseY 가 -45도 보다 작다면
	//if (MouseY < -45)
	//{
	//	// MouseY를 - 45도로 고정
	//	MouseY = -45;
	//}
	////만약에 MouseY 가 45도 보다 크다면
	//if (MouseY > 45)
	//{
	//	// MouseY를  45도로 고정
	//	MouseY = 45;
	//}
}

void UPlayerMove::InputJump()
{
	me->Jump();
}