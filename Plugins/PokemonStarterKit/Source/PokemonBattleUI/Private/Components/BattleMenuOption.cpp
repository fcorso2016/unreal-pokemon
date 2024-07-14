﻿// "Unreal Pokémon" created by Retro & Chill.

#include "Components/BattleMenuOption.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Handlers/BattleMenuHandler.h"
#include "Primatives/DisplayText.h"

void UBattleMenuOption::InitFromHandler(const UBattleMenuHandler *Handler) {
    OptionText->SetText(Handler->GetOptionText());
    BackgroundImage->SetBrush(Handler->GetBackgroundImage());
}

void UBattleMenuOption::NativeOnCurrentTextStyleChanged() {
    Super::NativeOnCurrentTextStyleChanged();
    OptionText->SetStyle(GetCurrentTextStyleClass());
}