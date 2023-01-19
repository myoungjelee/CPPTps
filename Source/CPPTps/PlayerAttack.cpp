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

	//BP_Bullet Ŭ���� ��������
	ConstructorHelpers::FClassFinder<ABullet> tempBullet(TEXT("Blueprint'/Game/Blueprints/BP_Bullet.BP_Bullet_C'"));
	if (tempBullet.Succeeded())
	{
		BulletFactory = tempBullet.Class;
	}


	//Explo Particle �� ��������
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempEffect(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tempEffect.Succeeded())
	{
		Effect = tempEffect.Object;
	}

	//ī�޶� ����ũ�� �־�����
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
	//���� �Լ��� ����������� DELEGATE ����ؾ��Ѵ�
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
	//ī�޶� �����
	/*APlayerController* controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StopAllCameraShakes();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);*/
	bFire = true;

	//�� ��� �ִϸ��̼��� ����
	UABP_Palyer* playerAnim = Cast<UABP_Palyer>(me->GetMesh()->GetAnimInstance());
	playerAnim->PlayAttackAnim();

	//���࿡ Rifle �� ���̴� ���¶��
	if (me->Rifle->IsVisible())
	{
		//FirePos Transform ��������
		FTransform trFire = me->Rifle->GetSocketTransform(TEXT("FirePos"));
		//�Ѿ˻��� (��ġ,ȸ�� ����)
		GetWorld()->SpawnActor<ABullet>(BulletFactory, trFire);
	}
	//Rifle �� ������ �ʴ� ���¶��
	else
	{
		//Start ���� ����
		FVector Start = me->camera->GetComponentLocation();
		//End ���� ����( ī�޶� ��ġ + ī�޶� �չ��� * �Ÿ�)
		FVector	End = Start + me->camera->GetForwardVector() * 5000;
		//�浹�� �Ǿ��� �� ������ ���� ��������
		FHitResult Hit;
		//�浹 �ɼ� ����
		FCollisionQueryParams Param;
		//�浹 �ɼǿ��� ���� ��������
		Param.AddIgnoredActor(me);
		//Line �� �߻��Ѵ�(LineTrace �̿�)
		bool Line = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Param);
		//���࿡ ������ Line �� �¾Ҵٸ�
		if (Line)
		{
			//���� ��ġ�� ���� ȿ���� �����ش�
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Effect, Hit.ImpactPoint);
			//���࿡ �����ְ� isSimulatingPhysics �� Ȱ��ȭ �Ǿ��ִٸ�
			if (Hit.GetComponent()->IsSimulatingPhysics())
			{
				//���� ������ F=ma �� ���Ѵ�.(camera->GetForwardVector()�� ���ӵ����� 50000�� ���ӵ���ġ)
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
	//���࿡ Rifle ��� ���
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

