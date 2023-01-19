// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include "Enemy.h"
#include "TpsPlayer.h"

void UEnemyAnim::NativeBeginPlay()
{
	APawn* pawn = TryGetPawnOwner();

	enemy = Cast<AEnemy>(pawn);
}
void UEnemyAnim::AnimNotify_DieEnd()
{
	enemy->fsm->Dying = true;
}

void UEnemyAnim::AnimNotify_Attack()
{
	//UE_LOG(LogTemp, Error, TEXT("Attack!!"));

	enemy->fsm->player->ReceivDamage(1);
}