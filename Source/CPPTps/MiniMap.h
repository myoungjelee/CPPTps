// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "MiniMap.generated.h"

/**
 *
 */
UCLASS()
class CPPTPS_API AMiniMap : public ASceneCapture2D
{
	GENERATED_BODY()

public:
	AMiniMap();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

UPROPERTY()
class AActor* player;

};
