// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverUI.h"
#include <Kismet/GameplayStatics.h>
#include <Components/Button.h>
#include "TpsPlayer.h"

void UGameOverUI::NativeConstruct()
{
	Super::NativeConstruct();

	AActor* player = UGameplayStatics::GetActorOfClass(GetWorld(), ATpsPlayer::StaticClass());

	if (player != nullptr)
	{
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		Btn_Retry->OnHovered.AddDynamic(this, &UGameOverUI::OnHover);
		Btn_Retry->OnUnhovered.AddDynamic(this, &UGameOverUI::UnHover);
		Btn_Retry->OnPressed.AddDynamic(this, &UGameOverUI::OnPress);
		Btn_Retry->OnClicked.AddDynamic(this, &UGameOverUI::OnClick);

		PlayAnimation(BtnIdle, 0, 0);
	}
}

void UGameOverUI::OnHover()
{
	StopAnimation(BtnIdle);
	PlayAnimation(BtnHover, 0, 1);
}
void UGameOverUI::UnHover()
{
	PlayAnimation(BtnHover, 0, 1, EUMGSequencePlayMode::Reverse);
	PlayAnimation(BtnIdle, 0, 0);
}
void UGameOverUI::OnPress()
{
	PlayAnimation(BtnPress, 0, 1);
	
}
void UGameOverUI::OnClick()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("TpsMap"));
}