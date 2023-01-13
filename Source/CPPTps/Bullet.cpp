// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	Sphere->SetSphereRadius(13);
	Sphere->SetCollisionProfileName(TEXT("BlockAll"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Sphere);
	Mesh->AddRelativeLocation(FVector(0, 0, -13));
	Mesh->SetRelativeScale3D(FVector(0.26f, 0.26f, 0.26f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh>tempMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (tempMesh.Succeeded())
	{
		Mesh->SetStaticMesh(tempMesh.Object);
	}

	//Projectile Component ����
	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("PROJECTILE"));
	//Movement �� �������� �� Component �� ����
	Projectile->SetUpdatedComponent(Sphere);
	//�ʱ�ӷ�
	Projectile->InitialSpeed = 2000;
	//�ִ�ӷ�
	Projectile->MaxSpeed = 2000;
	//Bounce ���� ����
	Projectile->bShouldBounce = true;
	//�󸶳� �� Bounce �ǰ� �� ������ (0~1)
	Projectile->Bounciness = 0.5f;

	//2�� �Ŀ� �������� �Ѵ�
	//InitialLifeSpan = 2;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	//Time  �����ϱ�
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &ABullet::Die, 2, false);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector p0 = GetActorLocation();
	FVector vt = 500 * GetActorForwardVector() * DeltaTime;

	SetActorLocation(p0 + vt);

	//CurrentTime += DeltaTime;
	//if (CurrentTime>DeTroyTime)
	//{
	//	Destroy();
	//}

}

void ABullet::Die()
{
	Destroy();
}