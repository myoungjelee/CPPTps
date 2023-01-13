// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyFSM.h"
#include <Engine/SkeletalMesh.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/CapsuleComponent.h>
#include "EnemyAnim.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("SkeletalMesh'/Game/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	ConstructorHelpers::FClassFinder<UEnemyAnim> tempAnim(TEXT("AnimBlueprint'/Game/Blueprints/ABP_Enemy.ABP_Enemy_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}

	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();


	GetCharacterMovement()->bOrientRotationToMovement = true;


}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::SetActive(bool bActive)
{
	//활성화
	if (bActive)
	{
		//충돌 되게 세팅
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//생성위치 재설정
		fsm->initPos = GetActorLocation();
	}
	//비활성화
	else
	{
		//충돌 안되게 세팅
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		dieDelegate.ExecuteIfBound(this);
		
	}
	//메쉬를 활성/비활성
	GetMesh()->SetActive(bActive);
	//메쉬를 보이고/안보이고
	GetMesh()->SetVisibility(bActive);
	//캐릭터 무브먼트 활성/비활성
	GetCharacterMovement()->SetActive(bActive);
	//fsm 활성/비활성
	fsm->SetActive(bActive);
}