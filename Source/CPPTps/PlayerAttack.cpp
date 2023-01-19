// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttack.h"
#include "TpsPlayer.h"
#include <Blueprint/UserWidget.h>
#include "Bullet.h"
#include <Particles/ParticleSystem.h>
#include "ABP_Palyer.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include <Camera/CameraComponent.h>
#include "EnemyFSM.h"
#include "MainUI.h"

UPlayerAttack::UPlayerAttack()
{
	PrimaryComponentTick.bCanEverTick = true;

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


	//Explo Particle 을 가져오자
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempEffect(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tempEffect.Succeeded())
	{
		Effect = tempEffect.Object;
	}

	//카메라 쉐이크를 넣어주자
/*ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShake(TEXT("Blueprint'/Game/Blueprints/BP_CamShake.BP_CamShake_C'"));
if (tempShake.Succeeded())
{
	cameraShake = tempShake.Class;
}*/
}
void UPlayerAttack::BeginPlay()
{
	Super::BeginPlay();

	ChangeWeapon(true);

	ScopeUI = CreateWidget(GetWorld(), Scope);
	/*bUseControllerRotationYaw = false;
	springArm->bUsePawnControlRotation = false;*/
	CommonUI = CreateWidget(GetWorld(), Common);
	CommonUI->AddToViewport();
}

void UPlayerAttack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CamShake();
}

void UPlayerAttack::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UPlayerAttack::InputFire);
	//같은 함수를 쓰고싶을때는 DELEGATE 사용해야한다
	DECLARE_DELEGATE_OneParam(FChangeWeapon, bool);
	PlayerInputComponent->BindAction<FChangeWeapon>(TEXT("Rifle"), IE_Pressed, this, &UPlayerAttack::ChangeWeapon, false);
	PlayerInputComponent->BindAction<FChangeWeapon>(TEXT("Sniper"), IE_Pressed, this, &UPlayerAttack::ChangeWeapon, true);
	/*PlayerInputComponent->BindAction(TEXT("Rifle"), IE_Pressed, this, &ATpsPlayer::InputRifle);
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATpsPlayer::InputSniper);*/
	PlayerInputComponent->BindAction(TEXT("Scope"), IE_Pressed, this, &UPlayerAttack::InputCtrl);
	PlayerInputComponent->BindAction(TEXT("Scope"), IE_Released, this, &UPlayerAttack::OutputCtrl);
}

void UPlayerAttack::InputFire()
{
	//카메라를 흔들자
	/*APlayerController* controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StopAllCameraShakes();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);*/
	bFire = true;

	//총 쏘는 애니메이션을 하자
	UABP_Palyer* playerAnim = Cast<UABP_Palyer>(me->GetMesh()->GetAnimInstance());
	playerAnim->PlayAttackAnim();

	//만약에 Rifle 이 보이는 상태라면
	if (me->Rifle->IsVisible())
	{
		//FirePos Transform 가져오기
		FTransform trFire = me->Rifle->GetSocketTransform(TEXT("FirePos"));
		//총알생성 (위치,회전 세팅)
		GetWorld()->SpawnActor<ABullet>(BulletFactory, trFire);
	}
	//Rifle 이 보이지 않는 상태라면
	else
	{
		//Start 지점 설정
		FVector Start = me->camera->GetComponentLocation();
		//End 지점 설정( 카메라 위치 + 카메라 앞방향 * 거리)
		FVector	End = Start + me->camera->GetForwardVector() * 5000;
		//충돌이 되었을 때 정보를 담을 변수설정
		FHitResult Hit;
		//충돌 옵션 설정
		FCollisionQueryParams Param;
		//충돌 옵션에서 나를 빼버린다
		Param.AddIgnoredActor(me);
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
				FVector F = Hit.GetComponent()->GetMass() * me->camera->GetForwardVector() * 50000;
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

void UPlayerAttack::ChangeWeapon(bool usesniper)
{
	//만약에 Rifle 모드 라면
	if (usesniper == false && ScopeUI->IsInViewport())
	{
		OutputCtrl();
	}

	me->Rifle->SetVisibility(!usesniper);
	me->Sniper->SetVisibility(usesniper);
	
	if (me->mainUI != nullptr)
	{
		me->mainUI->UpdateWeapon(usesniper);
	}
	
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

void UPlayerAttack::InputRifle()
{
	ChangeWeapon(0);
}
void UPlayerAttack::InputSniper()
{
	ChangeWeapon(1);
}

void UPlayerAttack::InputCtrl()
{
	if (me->Sniper->IsVisible())
	{
		ScopeUI->AddToViewport();
		me->camera->SetFieldOfView(45);
		CommonUI->RemoveFromParent();
	}
}
void UPlayerAttack::OutputCtrl()
{
	if (me->Sniper->IsVisible())
	{
		ScopeUI->RemoveFromParent();
		me->camera->SetFieldOfView(90);
		CommonUI->AddToViewport();
	}
}

void UPlayerAttack::CamShake()
{
	if (bFire == true)
	{
		currCamShakeTime += GetWorld()->GetDeltaSeconds();
		if (currCamShakeTime < camShakeTime)
		{
			float Yy = FMath::RandRange(-1.0f, 1.0f);
			float Zz = FMath::RandRange(-1.0f, 1.0f);

			me->camera->SetRelativeLocation(FVector(0, Yy, Zz));
		}
		else
		{
			currCamShakeTime = 0;
			bFire = false;
			me->camera->SetRelativeLocation(FVector(0));
		}
	}
}

