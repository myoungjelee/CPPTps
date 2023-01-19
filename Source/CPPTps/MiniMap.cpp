// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMap.h"
#include <Engine/TextureRenderTarget2D.h>
#include <Components/SceneCaptureComponent2D.h>
#include "TpsPlayer.h"
#include <Kismet/GameplayStatics.h>

AMiniMap::AMiniMap()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D>tempTex(TEXT("TextureRenderTarget2D'/Game/Images/RT_MIniMap.RT_MIniMap'"));
	if (tempTex.Succeeded())
	{
		GetCaptureComponent2D()->TextureTarget = tempTex.Object;
	}
}

void AMiniMap::BeginPlay()
{
	Super::BeginPlay();

	player = UGameplayStatics::GetActorOfClass(GetWorld(), ATpsPlayer::StaticClass());
}

void AMiniMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector pos = player->GetActorLocation();
	pos.Z = 540;
	SetActorLocation(pos);
}