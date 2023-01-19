// Fill out your copyright notice in the Description page of Project Settings.


#include "TpsPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Bullet.h"
#include <Components/StaticMeshComponent.h>
#include <Blueprint/UserWidget.h>
#include <Particles/ParticleSystem.h>
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "EnemyFSM.h"
#include <Components/CapsuleComponent.h>
#include "ABP_Palyer.h"
#include "PlayerMove.h"
#include "PlayerAttack.h"
#include "MainUI.h"
#include "MiniMap.h"
#include "GameOverUI.h"

// Sets default values
ATpsPlayer::ATpsPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	ConstructorHelpers::FClassFinder<UABP_Palyer>tempAni(TEXT("AnimBlueprint'/Game/Blueprints/ABP_Player.ABP_Player_C'"));
	if (tempAni.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAni.Class);
	}
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	//SkeletalMeshComponent 세팅(Rifle)
	Rifle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RIFLE"));
	Rifle->SetupAttachment(GetMesh(), TEXT("WeaponPos"));
	Rifle->SetRelativeLocation(FVector(-10, 5, -3));
	Rifle->SetRelativeRotation(FRotator(0, 90, 0));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempRifle(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (tempRifle.Succeeded())
	{
		Rifle->SetSkeletalMesh(tempRifle.Object);
	}

	//SkeletalMeshComponent 세팅(Sniper)
	Sniper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SNIPER"));
	Sniper->SetupAttachment(GetMesh(), TEXT("WeaponPos"));
	Sniper->SetRelativeScale3D(FVector(0.15));
	Sniper->SetRelativeLocation(FVector(-40, 5, 2));
	Sniper->SetRelativeRotation(FRotator(0, 90, 0));
	ConstructorHelpers::FObjectFinder<UStaticMesh>tempSniper(TEXT("StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	if (tempSniper.Succeeded())
	{
		Sniper->SetStaticMesh(tempSniper.Object);
	}
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->SetupAttachment(RootComponent);
	springArm->SetRelativeLocation(FVector(0, 70, 90));
	springArm->TargetArmLength = 600;

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(springArm);

	ConstructorHelpers::FClassFinder<UMainUI> tempUI(TEXT("WidgetBlueprint'/Game/Blueprints/BP_MainUI.BP_MainUI_C'"));
	if (tempUI.Succeeded())
	{
		mainUIFactory = tempUI.Class;
	}

	ConstructorHelpers::FClassFinder<AMiniMap> tempMini(TEXT("Blueprint'/Game/Blueprints/BP_MiniMap.BP_MiniMap_C'"));
	if (tempMini.Succeeded())
	{
		miniFactory = tempMini.Class;
	}

	ConstructorHelpers::FClassFinder<UGameOverUI> tempGO(TEXT("WidgetBlueprint'/Game/Blueprints/BP_GameOverUI.BP_GameOverUI_C'"));
	if (tempGO.Succeeded())
	{
		gameOverUIFactory = tempGO.Class;
	}
	compPlayerMove = CreateDefaultSubobject<UPlayerMove>(TEXT("Move"));
	compPlayerAttack = CreateDefaultSubobject<UPlayerAttack>(TEXT("Attack"));

}

// Called when the game starts or when spawned
void ATpsPlayer::BeginPlay()
{
	Super::BeginPlay();

	mainUI = CreateWidget<UMainUI>(GetWorld(), mainUIFactory);
	mainUI->AddToViewport();
	

	currHP = maxHP;
	prevHP = maxHP;

	GetWorld()->SpawnActor<AMiniMap>(
	miniFactory,
	FVector(GetActorLocation().X, GetActorLocation().Y, 540),
	FRotator(-90,0,0));
}

// Called every frame
void ATpsPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (bUpdateHP)
	//{
	//    ratioHP += DeltaTime*3;
	//	if (ratioHP >= 1)
	//	{
	//		ratioHP = 1;
	//		bUpdateHP = false;
	//	}
	//	float hp = FMath::Lerp(prevHP,currHP,ratioHP);
	//	mainUI->UpdateCurrHp(hp, maxHP);
	//}

	prevHP = FMath::Lerp(prevHP, currHP, DeltaTime * 20);
	mainUI->UpdateCurrHp(prevHP, maxHP);
}

// Called to bind functionality to input
void ATpsPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	compPlayerMove->SetupInputBinding(PlayerInputComponent);
	compPlayerAttack->SetupInputBinding(PlayerInputComponent);
}

void ATpsPlayer::ReceivDamage(float damage)
{
	//prevHP = currHP;
	//bUpdateHP = true;
	//ratioHP = 0;
	currHP -= damage;
	//mainUI->UpdateCurrHp(currHP,maxHP);
	if (currHP <= 0)
	{
		UGameOverUI* gameOverUI = CreateWidget<UGameOverUI>(GetWorld(), gameOverUIFactory);
		gameOverUI->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HP : %f"),currHP);
	}
}


