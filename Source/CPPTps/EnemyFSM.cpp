// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "TpsPlayer.h"
#include "Enemy.h"
#include <Components/CapsuleComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "EnemyAnim.h"
#include <AIModule/Classes/AIController.h>
#include <NavigationSystem.h>
#include "EnemyManager.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Enemy�� SetActive �۵��ǰ� �Ҷ�� �������
	bAutoActivate = true;

	ConstructorHelpers::FObjectFinder<UAnimMontage> tempMont(TEXT("AnimMontage'/Game/Blueprints/AMT_Enemy_Damage.AMT_Enemy_Damage'"));
	if (tempMont.Succeeded())
	{
		damageMontage = tempMont.Object;
	}
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<ATpsPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATpsPlayer::StaticClass()));
	me = Cast<AEnemy>(GetOwner());

	HP = InitailHP;

	initPos = me->GetActorLocation();

	Anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());
	AI = Cast<AAIController>(me->GetController());

}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (State)
	{
	case EEnemyState::Idle:
		Idle();
		break;
	case EEnemyState::Move:
		Move();
		break;
	case EEnemyState::Attack:
		Attack();
		break;
	case EEnemyState::Damaged:
		Damaged();
		break;
	case EEnemyState::Die:
		Die();
		break;
	case EEnemyState::ReturnPos:
		ReturnPos();
		break;
	case EEnemyState::AttackDelay:
		AttackDelay();
		break;
	}
}

void UEnemyFSM::Idle()
{

	if (IsTargetTrace())
	{
		ChangeState(EEnemyState::Move);
	}
	else
	{
		if (FlowTime(IdleDelayTime))
		{
			ChangeState(EEnemyState::Move);
		}
	}
}
void UEnemyFSM::Move()
{
	bool bTrace = IsTargetTrace();
	//�÷��̾ ���ϴ� ����(�÷��̾� ��ġ - ���� ��ġ)
	FVector dir = player->GetActorLocation() - me->GetActorLocation();
	//���ʹ��� ó�� ��ġ�� ���� ��ġ �Ÿ� ���ϱ�
	float dist = FVector::Distance(initPos, me->GetActorLocation());

	if (dist > moveRange)
	{
		ChangeState(EEnemyState::ReturnPos);
	}
	else if (bTrace)
	{
		if (dir.Length() < AttackRange)
		{
			ChangeState(EEnemyState::Attack);
		}
		else
		{
			//me->AddMovementInput(dir.GetSafeNormal());
			AI->MoveToLocation(player->GetActorLocation());
		}
	}
	else
	{
		MoveToPos(RandPos);
	}

	}
	void UEnemyFSM::Attack()
	{
		ChangeState(EEnemyState::AttackDelay);
	}
	void UEnemyFSM::Damaged()
	{
		if (FlowTime(DamageDelayTime))
		{
			ChangeState(EEnemyState::Move);
		}
	}
	void UEnemyFSM::Die()
	{
		if (Dying == false) return;

		FVector p0 = me->GetActorLocation();
		FVector vt = FVector::DownVector * DieSpeed * GetWorld()->DeltaTimeSeconds;
		FVector P = p0 + vt;

		if (P.Z < -200)
		{
			//me->Destroy();
			//���� ��Ȱ��ȭ ����
			me->SetActive(false);
			////1.EnemyManager �� ã��
			//AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyManager::StaticClass());
			//AEnemyManager* am = Cast<AEnemyManager>(actor);
			////2.ã�� EnemyManager ���� enemyArray �� ���� �ٽ� ����
			//am->enemyArray.Add(me);

			//HP �� initialHP
			HP = InitailHP;
			//���¸� Idle
			ChangeState(EEnemyState::Idle);
			//��Ÿ�ָ� �����ش�
			me->StopAnimMontage(damageMontage);
			Dying = false;
		}
		else
		{
			me->SetActorLocation(P);
		}
	}
	void UEnemyFSM::ReturnPos()
	{
		MoveToPos(initPos);

		//FVector dir = initPos - me->GetActorLocation();

		//if (dir.Length() < 10)
		//{
		//	ChangeState(EEnemyState::Idle);
		//}
		//else
		//{
		//	me->AddMovementInput(dir);
		//}
	}
	void UEnemyFSM::AttackDelay()
	{
		bool bComplete = FlowTime(AttackDelayTime);

		if (bComplete == true)
		{
			FVector dir = player->GetActorLocation() - me->GetActorLocation();
			if (dir.Length() < AttackRange)
			{
				ChangeState(EEnemyState::Attack);
			}
			else
			{
				ChangeState(EEnemyState::Idle);
			}
		}
	}

	void UEnemyFSM::ChangeState(EEnemyState AfterState)
	{
		//���� ���� �α׸� �������
		UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState"), true);
		if (enumPtr != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s -----> %s"), *enumPtr->GetNameStringByIndex((int32)State), *enumPtr->GetNameStringByIndex((int32)AfterState));
		}
		//���� ���¸� ����
		State = AfterState;
		Anim->state = State;

		CurrTime = 0;

		AI->StopMovement();

		switch (State)
		{
		case EEnemyState::Move:
		{
			UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			FNavLocation loc;
			ns->GetRandomReachablePointInRadius(initPos, 1000, loc);
			RandPos = loc.Location;
		}
		break;
		case EEnemyState::Attack:
			//CurrTime = AttackDelayTime;
			break;
		case EEnemyState::Damaged:
		{
			int32 rand = FMath::RandRange(0, 1);
			FString sectionName = FString::Printf(TEXT("Damage%d"), rand);
			me->PlayAnimMontage(damageMontage, 1.0f, FName(*sectionName));
		}
		break;
		case EEnemyState::Die:
			me->PlayAnimMontage(damageMontage, 1.0f, FName(TEXT("Die")));
			me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		}


	}

	bool UEnemyFSM::FlowTime(float DelayTime)
	{
		CurrTime += GetWorld()->GetDeltaSeconds();
		if (CurrTime > DelayTime)
		{
			CurrTime = 0;
			return true;
		}
		return false;
	}

	void UEnemyFSM::ReceiveDamaged()
	{
		HP--;
		if (HP > 0)
		{
			ChangeState(EEnemyState::Damaged);
		}
		else
		{
			ChangeState(EEnemyState::Die);
		}
	}

	bool UEnemyFSM::IsTargetTrace()
	{
		FVector vectorA = me->GetActorForwardVector();
		FVector vectorB = (player->GetActorLocation() - me->GetActorLocation());
		float dotValue = FVector::DotProduct(vectorA, vectorB.GetSafeNormal());
		float angle = UKismetMathLibrary::DegAcos(dotValue);
		FHitResult Hitinfo;
		FCollisionQueryParams ignore;
		ignore.AddIgnoredActor(me);

		if (angle < 30 && vectorB.Length() < traceRange)
		{
			bool Line = GetWorld()->LineTraceSingleByChannel(Hitinfo, me->GetActorLocation(), player->GetActorLocation(), ECC_Visibility, ignore);
			if (Line)
			{
				if (Hitinfo.GetActor()->GetName().Contains(TEXT("Player")))
				{
					return true;
				}
			}
		}
		return false;
	}
	void UEnemyFSM::MoveToPos(FVector pos)
	{
		//�ش� ��ġ�� ����
		EPathFollowingRequestResult::Type result = AI->MoveToLocation(pos);
		//���� �������� �����ߴٸ�
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			//Idle�� ��ȯ
			ChangeState(EEnemyState::Idle);
		}
	}