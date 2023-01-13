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

	//적을 미리 10개 만들자
	for (int32 i = 0; i < 3; i++)
	{
		//어떤 상황이던 무조건 생성한다는 옵션설정
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//적 생성
		AEnemy* e = GetWorld()->SpawnActor<AEnemy>(EnemyFactory,GetActorLocation(),GetActorRotation(),param);
		//적이 죽었을때 호출되는 함수 등록
		e->dieDelegate.BindUObject(this, &AEnemyManager::AddEnemy);
		//적을 비활성화 하자
		e->SetActive(false);
		//생성한 적을 배열에 담자
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
	//어느 위치에 만들지 고른다
	//랜덤
	//int32 rand = FMath::RandRange(0, spawnPosArray.Num() - 1);
	//순서대로
	if (currTime > makeTime)
	{
		float yaw = FMath::FRandRange(0.0f, 360.0f);
		spawn->SetActorRotation(FRotator(0, yaw, 0));
		float length = FMath::FRandRange(3000.0f, 1000.0f);
		FVector pos = spawn->GetActorForwardVector()* length;
		
		//0번째의 위치, 회전 세팅
		enemyArray[0]->SetActorLocation(spawn->GetActorLocation() + pos);
		enemyArray[0]->SetActorRotation(spawn->GetActorRotation());
		//enemy 배열에서 0번째를 활성화
		enemyArray[0]->SetActive(true);
		//활성화된 놈을 enemyArray에서 빼자
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
	//스폰 액터를 찾아서 가져오자
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);

	for (int32 i = 0; i < actors.Num(); i++)
	{
		//만약에 이름에 SpawnPos 가 포함되어 있다면
		if (actors[i]->GetName().Contains(TEXT("SpawnPos")))
		{
			//spawnPosArray 에 추가한다
			spawnPosArray.Add(actors[i]);
		}
	}
}

void AEnemyManager::AddEnemy(class AEnemy* e)
{
	enemyArray.Add(e);
}