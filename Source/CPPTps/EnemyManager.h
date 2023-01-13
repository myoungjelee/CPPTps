// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class CPPTPS_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	float minTime = 1;
	float maxTime = 5;
	float currTime;
	float makeTime;
	int32 spawnIdx = 0;
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemy> EnemyFactory;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> spawnPosArray;

	UPROPERTY(EditAnywhere)
	TArray<class AEnemy*> enemyArray;

public:
	void FindSpawnPos();
	UFUNCTION() //딜리게이트에 쓸 함수라 UFUNCTION 꼭 해줘야함
	void AddEnemy(class AEnemy* e);
};
