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

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->SetupAttachment(RootComponent);
	springArm->SetRelativeLocation(FVector(0, 70, 90));
	springArm->TargetArmLength = 600;

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(springArm);

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

	ConstructorHelpers::FClassFinder<UUserWidget> tempScope(TEXT("WidgetBlueprint'/Game/Blueprints/BP_SCOPE.BP_SCOPE_C'"));
	if (tempScope.Succeeded())
	{
		Scope = tempScope.Class;
	}

	ConstructorHelpers::FClassFinder<UUserWidget> tempCom(TEXT("WidgetBlueprint'/Game/Blueprints/BP_Common.BP_Common_C'"));
	if (tempCom.Succeeded())
	{
		Common = tempCom.Class;
	}

	//BP_Bullet 클래스 가져오자
	ConstructorHelpers::FClassFinder<ABullet> tempBullet(TEXT("Blueprint'/Game/Blueprints/BP_Bullet.BP_Bullet_C'"));
	if (tempBullet.Succeeded())
	{
		BulletFactory = tempBullet.Class;
	}

	bUseControllerRotationYaw = true;
	springArm->bUsePawnControlRotation = true;

	//Explo Particle 을 가져오자
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempEffect(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tempEffect.Succeeded())
	{
		Effect = tempEffect.Object;
	}

	//점프 횟수를 2개로 하자
	JumpMaxCount = 2;

	//움직이는 속도를 1000으로 하자
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	//점프하는 높이를 600으로 하자
	GetCharacterMovement()->JumpZVelocity = 600;

	//카메라 쉐이크를 넣어주자
	/*ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShake(TEXT("Blueprint'/Game/Blueprints/BP_CamShake.BP_CamShake_C'"));
	if (tempShake.Succeeded())
	{
		cameraShake = tempShake.Class;
	}*/
}

// Called when the game starts or when spawned
void ATpsPlayer::BeginPlay()
{
	Super::BeginPlay();

	//카메라 상하 회전갑 최소값과 최대값 설정
	APlayerController* playerController = Cast<APlayerController>(Controller);
	playerController->PlayerCameraManager->ViewPitchMin = -45;
	playerController->PlayerCameraManager->ViewPitchMax = 45;

	ChangeWeapon(true);

	ScopeUI = CreateWidget(GetWorld(), Scope);
	/*bUseControllerRotationYaw = false;
	springArm->bUsePawnControlRotation = false;*/
	CommonUI = CreateWidget(GetWorld(), Common);
	CommonUI->AddToViewport();
}

// Called every frame
void ATpsPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//이동
	MoveAction(DeltaTime);
	//회전
	//RotateAction();

	CamShake();

}

// Called to bind functionality to input
void ATpsPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATpsPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATpsPlayer::InputVertical);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATpsPlayer::InputTurn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATpsPlayer::InputLookUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATpsPlayer::InputJump);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATpsPlayer::InputFire);
	//같은 함수를 쓰고싶을때는 DELEGATE 사용해야한다
	DECLARE_DELEGATE_OneParam(FChangeWeapon, bool);
	PlayerInputComponent->BindAction<FChangeWeapon>(TEXT("Rifle"), IE_Pressed, this, &ATpsPlayer::ChangeWeapon, false);
	PlayerInputComponent->BindAction<FChangeWeapon>(TEXT("Sniper"), IE_Pressed, this, &ATpsPlayer::ChangeWeapon, true);
	/*PlayerInputComponent->BindAction(TEXT("Rifle"), IE_Pressed, this, &ATpsPlayer::InputRifle);
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATpsPlayer::InputSniper);*/
	PlayerInputComponent->BindAction(TEXT("Scope"), IE_Pressed, this, &ATpsPlayer::InputCtrl);
	PlayerInputComponent->BindAction(TEXT("Scope"), IE_Released, this, &ATpsPlayer::OutputCtrl);


	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATpsPlayer::InputRun);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATpsPlayer::InputRun);
}

void ATpsPlayer::InputHorizontal(float value)
{
	Y = value;
}
void ATpsPlayer::InputVertical(float value)
{
	X = value;
}
void ATpsPlayer::InputTurn(float value)
{
	AddControllerYawInput(value);
	//MouseX += value;
}
void ATpsPlayer::InputLookUp(float value)
{
	AddControllerPitchInput(value);
	//MouseY += value;
	//MouseY 값을  최소 -45, 최대 45 로 고정한다
	//MouseY = FMath::Clamp(MouseY, -45, 45);
	////만약에 MouseY 가 -45도 보다 작다면
	//if (MouseY < -45)
	//{
	//	// MouseY를 - 45도로 고정
	//	MouseY = -45;
	//}
	////만약에 MouseY 가 45도 보다 크다면
	//if (MouseY > 45)
	//{
	//	// MouseY를  45도로 고정
	//	MouseY = 45;
	//}
}

void ATpsPlayer::InputJump()
{
	Jump();
}

void ATpsPlayer::InputFire()
{
	//카메라를 흔들자
	/*APlayerController* controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StopAllCameraShakes();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);*/
	bFire = true;


	//총 쏘는 애니메이션을 하자
	UABP_Palyer* playerAnim = Cast<UABP_Palyer>(GetMesh()->GetAnimInstance());
	playerAnim->PlayAttackAnim();

	//만약에 Rifle 이 보이는 상태라면
	if (Rifle->IsVisible())
	{
		//FirePos Transform 가져오기
		FTransform trFire = Rifle->GetSocketTransform(TEXT("FirePos"));
		//총알생성 (위치,회전 세팅)
		GetWorld()->SpawnActor<ABullet>(BulletFactory, trFire);
	}
	//Rifle 이 보이지 않는 상태라면
	else
	{
		//Start 지점 설정
		FVector Start = camera->GetComponentLocation();
		//End 지점 설정( 카메라 위치 + 카메라 앞방향 * 거리)
		FVector	End = Start + camera->GetForwardVector() * 5000;
		//충돌이 되었을 때 정보를 담을 변수설정
		FHitResult Hit;
		//충돌 옵션 설정
		FCollisionQueryParams Param;
		//충돌 옵션에서 나를 빼버린다
		Param.AddIgnoredActor(this);
		//Line 을 발사한다(LineTrace 이용)
		bool Line = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Param);
		//만약에 누군가 Line 에 맞았다면
		if (Line)
		{
			//맞은 위치에 폭발 효과를 보여준다
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Effect, Hit.ImpactPoint);
			//만약에 맞은애가 isSimulatingPhysics 가 활성화 되어있다면
			if (Hit.GetComponent()->IsSimulatingPhysics())
			{
				//맞은 지점에 F=ma 를 가한다.(camera->GetForwardVector()는 가속도방향 50000은 가속도수치)
				FVector F = Hit.GetComponent()->GetMass() * camera->GetForwardVector() * 50000;
				Hit.GetComponent()->AddForceAtLocation(F, Hit.ImpactPoint);

			}

			AEnemy* enemy = Cast<AEnemy>(Hit.GetActor());
			if (enemy != nullptr)
			{
				enemy->fsm->ReceiveDamaged();
			}
		}
	}
}

void ATpsPlayer::ChangeWeapon(bool usesniper)
{
	//만약에 Rifle 모드 라면
	if (usesniper == false && ScopeUI->IsInViewport())
	{
		OutputCtrl();
	}

	Rifle->SetVisibility(!usesniper);
	Sniper->SetVisibility(usesniper);
	//if (usesniper == false)
	//{
	//	Rifle->SetVisibility(true);
	//	Sniper->SetVisibility(false);
	//}
	//else
	//{
	//	Rifle->SetVisibility(false);
	//	Sniper->SetVisibility(true);
	//}
}

void ATpsPlayer::InputRifle()
{
	ChangeWeapon(0);
}
void ATpsPlayer::InputSniper()
{
	ChangeWeapon(1);
}

void ATpsPlayer::MoveAction(float DeltaTime)
{
	FVector p0 = GetActorLocation();
	FVector dir = GetActorForwardVector() * X + GetActorRightVector() * Y;
	FVector vt = walkSpeed * dir.GetSafeNormal() * DeltaTime;

	AddMovementInput(dir); //FVector p = p0 + vt;  //SetActorLocation(p); 


	/*FVector p0 = GetActorLocation();
	FVector dirX = GetActorForwardVector() * X;
	FVector dirY = GetActorRightVector()* Y;
	FVector dir = dirX + dirY;
	dir.Normalize();
	FVector p = p0 + dir * speed * DeltaTime;
	SetActorLocation(p);*/
}
void ATpsPlayer::RotateAction()
{
	SetActorRotation(FRotator(0, MouseX, 0));
	/*FRotator yaw = GetActorRotation();
	yaw.Yaw = MouseX;
	SetActorRotation(yaw);*/

	springArm->SetRelativeRotation(FRotator(-MouseY, 0, 0));
	//FRotator pit;
	//pit.Pitch = -MouseY;
	//springArm->SetRelativeRotation(pit);
}


void ATpsPlayer::InputCtrl()
{
	if (Sniper->IsVisible())
	{
		ScopeUI->AddToViewport();
		camera->SetFieldOfView(45);
		CommonUI->RemoveFromParent();
	}
}
void ATpsPlayer::OutputCtrl()
{
	if (Sniper->IsVisible())
	{
		ScopeUI->RemoveFromParent();
		camera->SetFieldOfView(90);
		CommonUI->AddToViewport();
	}
}

void  ATpsPlayer::InputRun()
{
	if (GetCharacterMovement()->MaxWalkSpeed > walkSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = runSpeed;
	}
}

void ATpsPlayer::CamShake()
{
	if (bFire == true)
	{
		currCamShakeTime += GetWorld()->GetDeltaSeconds();
		if (currCamShakeTime < camShakeTime)
		{
			float Yy = FMath::RandRange(-1.0f, 1.0f);
			float Zz = FMath::RandRange(-1.0f, 1.0f);

			camera->SetRelativeLocation(FVector(0, Yy, Zz));
		}
		else
		{
			currCamShakeTime = 0;
			bFire = false;
			camera->SetRelativeLocation(FVector(0));
		}
	}
}