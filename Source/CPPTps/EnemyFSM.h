// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"



//상태 정의
UENUM(BlueprintType)//만들어 놓은것을 블루트린트에서 쓰고싶을때
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Skill,
	Damaged,
	Die,
	ReturnPos,
	AttackDelay,
};



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CPPTPS_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
		EEnemyState State;


	void Idle();
	void Move();
	void Attack();
	void Damaged();
	void Die();
	void ReturnPos();
	void AttackDelay();

	float CurrTime = 0;
	float IdleDelayTime = 2;
	float AttackRange = 200;
	float AttackDelayTime = 4;
	float DamageDelayTime = 2;
	float DieSpeed = 100;

	UPROPERTY(EditAnywhere)
		class ATpsPlayer* player;
	UPROPERTY(EditAnywhere)
		class AEnemy* me;
	UPROPERTY(EditAnywhere)
		class UEnemyAnim* Anim;

	//상태변경시 한번만!!! 호출되는 함수
	void ChangeState(EEnemyState AfterState);

	bool FlowTime(float DelayTime);

	void ReceiveDamaged();

	float InitailHP = 1;
	float HP;

	bool IsTargetTrace();

	float traceRange = 1000;

	float moveRange = 2000;

	FVector initPos;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* damageMontage;

	bool Dying = false;

	UPROPERTY(EditAnywhere)
	class AAIController* AI;

	FVector RandPos;

	void MoveToPos(FVector pos);

	

};
