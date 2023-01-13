// Fill out your copyright notice in the Description page of Project Settings.


#include "ABP_Palyer.h"
#include "TpsPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>


UABP_Palyer::UABP_Palyer()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempMontage(TEXT("AnimMontage'/Game/Blueprints/AMT_Attack.AMT_Attack'"));
	if (tempMontage.Succeeded())
	{
		attackAnimMontage = tempMontage.Object;
	}
}



void UABP_Palyer::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	//1. ���� �� ��������
	APawn* pawn = TryGetPawnOwner();
	//2. ATpsplayer�� ĳ����
	me = Cast<ATpsPlayer>(pawn);
}




void UABP_Palyer::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//���࿡ player �� nullptr �ƴϸ�
	if (me != nullptr)
	{
		//3. �÷��̾��� �ӵ��� �չ��� ��������
		FVector V = me->GetVelocity();
		FVector U = me->GetActorForwardVector();
		FVector R = me->GetActorRightVector();
		//4. �� ���͸� ������ ���� dirV�� ����
		dirV = FVector::DotProduct(V, U);
		dirH = FVector::DotProduct(V, R);


		bAir = me->GetCharacterMovement()->IsFalling();
	}
}


void UABP_Palyer::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}