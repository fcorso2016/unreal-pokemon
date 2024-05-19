﻿// "Unreal Pokémon" created by Retro & Chill.


#include "Components/Summary/PokemonSkillsPage.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Summary/HPStatRow.h"
#include "Components/Summary/PokemonStatRow.h"
#include "Pokemon/Pokemon.h"
#include "Pokemon/Abilities/AbilityBlock.h"
#include "Primatives/DisplayText.h"
#include "Settings/PokemonSettings.h"

TSharedRef<SWidget> UPokemonSkillsPage::RebuildWidget() {
    auto Ret = Super::RebuildWidget();

    for (auto &StatRow : StatRows) {
        StatRow->RemoveFromParent();
    }
    StatRows.Empty();
    
    if (StatRowClass != nullptr) {
        auto HPStat = GetDefault<UPokemonSettings>()->GetHPStat();
        for (auto Stat : StatsToDisplay) {
            if (Stat == HPStat) {
                auto Row = WidgetTree->ConstructWidget(HPStatRow);
                Row->SetDisplayedStat(Stat);
                SlotHPStatRow(Row);
                StatRows.Emplace(Row);
            } else {
                auto Row = WidgetTree->ConstructWidget(StatRowClass);
                Row->SetDisplayedStat(Stat);
                SlotStatRow(Row);
                StatRows.Emplace(Row);
            }
            
        }
    }
    
    return Ret;
}

void UPokemonSkillsPage::RefreshInfo_Implementation(const TScriptInterface<IPokemon> &Pokemon) {
    Super::RefreshInfo_Implementation(Pokemon);
    for (auto &Row : StatRows) {
        Row->Refresh(Pokemon);
    }

    auto Ability = Pokemon->GetAbility();
    AbilityName->SetText(Ability->GetDisplayName());
    AbilityDescription->SetText(Ability->GetAbilityDescription());
}