// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class CPPTPS_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* Mesh;

	//발사체 움직임 Component
	UPROPERTY(EditAnywhere)
		class UProjectileMovementComponent* Projectile;

	float CurrentTime = 0;
	float DeTroyTime = 2;

	void Die();
};
