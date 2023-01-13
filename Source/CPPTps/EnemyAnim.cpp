// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include "Enemy.h"

void UEnemyAnim::AnimNotify_DieEnd()
{
	AEnemy* enemy = Cast<AEnemy>(TryGetPawnOwner());
	enemy->fsm->Dying = true;
}

void UEnemyAnim::AnimNotify_Attack()
{
	UE_LOG(LogTemp, Error, TEXT("Attack!!"));
}