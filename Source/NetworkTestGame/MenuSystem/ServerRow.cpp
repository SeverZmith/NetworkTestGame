// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerRow.h"

#include "Components/Button.h"

#include "MainMenu.h"


void UServerRow::Setup(class UMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;

	// Subscribe RowButton with UServerRow::OnClicked method.
	RowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);

}

void UServerRow::OnClicked()
{
	// Update selected game index when row is clicked.
	Parent->SelectIndex(Index);

}
