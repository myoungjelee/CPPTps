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

	//1. 소유 폰 가져오기
	APawn* pawn = TryGetPawnOwner();
	//2. ATpsplayer로 캐스팅
	me = Cast<ATpsPlayer>(pawn);
}




void UABP_Palyer::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//만약에 player 가 nullptr 아니면
	if (me != nullptr)
	{
		//3. 플레이어의 속도와 앞방향 가져오기
		FVector V = me->GetVelocity();
		FVector U = me->GetActorForwardVector();
		FVector R = me->GetActorRightVector();
		//4. 두 벡터를 내적한 값을 dirV에 세팅
		dirV = FVector::DotProduct(V, U);
		dirH = FVector::DotProduct(V, R);


		bAir = me->GetCharacterMovement()->IsFalling();
	}
}


void UABP_Palyer::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}