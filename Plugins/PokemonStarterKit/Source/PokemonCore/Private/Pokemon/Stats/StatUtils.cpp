// "Unreal Pokémon" created by Retro & Chill.
#include "Pokemon/Stats/StatUtils.h"
#include "DataManager.h"
#include "RetroLib/Ranges/Algorithm/NameAliases.h"
#include "Species/Nature.h"
#include "Species/Stat.h"

using namespace StatUtils;

constexpr int32 IV_MAX = 31;

POKEMONCORE_API TMap<FName, int32> StatUtils::RandomizeIVs() {
    const auto &DataSubsystem = FDataManager::GetInstance();
    auto &StatTable = DataSubsystem.GetDataTable<FStat>();

    TMap<FName, int32> Ret;
    StatTable.GetAllRows() | Retro::Ranges::ForEach([&Ret](const FStat &Stat) {
        if (Stat.Type == EPokemonStatType::Battle)
            return;

        Ret[Stat.ID] = RandomizeIV();
    });

    return Ret;
}

int32 StatUtils::RandomizeIV() {
    return FMath::RandRange(0, IV_MAX);
}

POKEMONCORE_API TMap<FName, int32> StatUtils::DefaultEVs() {
    const auto &DataSubsystem = FDataManager::GetInstance();
    auto &StatTable = DataSubsystem.GetDataTable<FStat>();

    TMap<FName, int32> Ret;
    StatTable.GetAllRows() | Retro::Ranges::ForEach([&Ret](const FStat &Stat) {
        if (Stat.Type == EPokemonStatType::Battle)
            return;

        Ret[Stat.ID] = 0;
    });

    return Ret;
}

POKEMONCORE_API TUniquePtr<Exp::IGrowthRate> StatUtils::CreateGrowthRate(FName GrowthRate) {
    return Exp::FGrowthRateRegistry::GetInstance().Construct(GrowthRate);
}

POKEMONCORE_API int32 StatUtils::GetMaxLevel() {
    // TODO: Allow this to be configurable
    return 100;
}
