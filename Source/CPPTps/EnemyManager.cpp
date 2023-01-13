// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemyManager::AEnemyManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FClassFinder<AEnemy> tempEm(TEXT("Blueprint'/Game/Blueprints/BP_Enemy.BP_Enemy_C'"));
	if (tempEm.Succeeded())
	{
		EnemyFactory = tempEm.Class;
	}
}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	makeTime = FMath::RandRange(minTime, maxTime);

	FindSpawnPos();

	//���� �̸� 10�� ������
	for (int32 i = 0; i < 3; i++)
	{
		//� ��Ȳ�̴� ������ �����Ѵٴ� �ɼǼ���
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//�� ����
		AEnemy* e = GetWorld()->SpawnActor<AEnemy>(EnemyFactory,GetActorLocation(),GetActorRotation(),param);
		//���� �׾����� ȣ��Ǵ� �Լ� ���
		e->dieDelegate.BindUObject(this, &AEnemyManager::AddEnemy);
		//���� ��Ȱ��ȭ ����
		e->SetActive(false);
		//������ ���� �迭�� ����
		//enemyArray.Add(e);
	}
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (enemyArray.Num() == 0) return;
	currTime += DeltaTime;

	AActor* spawn = spawnPosArray[spawnIdx];
	//��� ��ġ�� ������ ����
	//����
	//int32 rand = FMath::RandRange(0, spawnPosArray.Num() - 1);
	//�������
	if (currTime > makeTime)
	{
		float yaw = FMath::FRandRange(0.0f, 360.0f);
		spawn->SetActorRotation(FRotator(0, yaw, 0));
		float length = FMath::FRandRange(3000.0f, 1000.0f);
		FVector pos = spawn->GetActorForwardVector()* length;
		
		//0��°�� ��ġ, ȸ�� ����
		enemyArray[0]->SetActorLocation(spawn->GetActorLocation() + pos);
		enemyArray[0]->SetActorRotation(spawn->GetActorRotation());
		//enemy �迭���� 0��°�� Ȱ��ȭ
		enemyArray[0]->SetActive(true);
		//Ȱ��ȭ�� ���� enemyArray���� ����
		enemyArray.RemoveAt(0);

		//GetWorld()->SpawnActor<AEnemy>(EnemyFactory, spawn->GetActorLocation() + pos, spawn->GetActorRotation());
		spawnIdx++;
		spawnIdx %= spawnPosArray.Num();
		/*if (spawnIdx == spawnPosArray.Num())
		{
			spawnIdx = 0;
		}*/
		currTime = 0;
		makeTime = FMath::RandRange(minTime, maxTime);
	}	
}

void AEnemyManager::FindSpawnPos()
{
	//���� ���͸� ã�Ƽ� ��������
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);

	for (int32 i = 0; i < actors.Num(); i++)
	{
		//���࿡ �̸��� SpawnPos �� ���ԵǾ� �ִٸ�
		if (actors[i]->GetName().Contains(TEXT("SpawnPos")))
		{
			//spawnPosArray �� �߰��Ѵ�
			spawnPosArray.Add(actors[i]);
		}
	}
}

void AEnemyManager::AddEnemy(class AEnemy* e)
{
	enemyArray.Add(e);
}