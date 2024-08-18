// "Unreal Pokémon" created by Retro & Chill.
#include "Species/Stat.h"
#include "Algo/RemoveIf.h"
#include "DataManager.h"
#include "Ranges/Algorithm/ToArray.h"
#include "Ranges/Views/ContainerView.h"
#include "Ranges/Views/Map.h"
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

TArray<FName> UStatHelper::GetStatNames() {
    return FDataManager::GetInstance().GetDataTable<FStat>().GetTableRowNames();
}

TArray<FName> UStatHelper::GetMainStatNames() {
    auto &StatTable = FDataManager::GetInstance().GetDataTable<FStat>();
    auto Stats = StatTable.GetAllRows();
    return Stats | ranges::views::filter([](const FStat *Stat) { return Stat->Type != EPokemonStatType::Battle; }) |
           UE::Ranges::Map(&FStat::ID) | UE::Ranges::ToArray;
}

TArray<FName> UStatHelper::GetBattleStatNames() {
    auto &StatTable = FDataManager::GetInstance().GetDataTable<FStat>();
    auto Stats = StatTable.GetAllRows();
    return Stats | ranges::views::filter([](const FStat *Stat) { return Stat->Type != EPokemonStatType::Main; }) |
           UE::Ranges::Map(&FStat::ID) | UE::Ranges::ToArray;
}

TArray<FName> UStatHelper::GetMainBattleStatNames() {
    auto &StatTable = FDataManager::GetInstance().GetDataTable<FStat>();
    auto Stats = StatTable.GetAllRows();
    return Stats | ranges::views::filter([](const FStat *Stat) { return Stat->Type == EPokemonStatType::MainBattle; }) |
           UE::Ranges::Map(&FStat::ID) | UE::Ranges::ToArray;
}
