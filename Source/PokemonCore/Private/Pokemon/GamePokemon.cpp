// "Unreal Pokémon" created by Retro & Chill.
#include "Pokemon/GamePokemon.h"

#include "DataManager.h"
#include "Pokemon/Stats/DefaultStatBlock.h"
#include "Species/GenderRatio.h"
#include "Species/SpeciesData.h"
#include "Utilities/PersonalityValueUtils.h"

// TODO: Instantiate the stat block dynamically based on a user config
FGamePokemon::FGamePokemon(FName Species, int32 Level) : Species(Species), PersonalityValue(UPersonalityValueUtils::GeneratePersonalityValue()) {
	auto &DataManager = FDataManager::GetInstance();
	auto &SpeciesTable = DataManager.GetDataTable<FSpeciesData>();

	auto SpeciesData = SpeciesTable.GetData(Species);
	check(SpeciesData != nullptr);
	StatBlock = MakeUnique<FDefaultStatBlock>(SpeciesData->GrowthRate, Level);
	StatBlock->CalculateStats(SpeciesData->BaseStats);
	CurrentHP = StatBlock->GetStat(GetHPStat()).GetStatValue();
}

FText FGamePokemon::GetName() const {
	return Nickname.IsSet() ? Nickname.GetValue() : GetSpecies().RealName;
}

EGender FGamePokemon::GetGender() const {
	using enum EGender;
	
	if (Gender.IsSet())
		return Gender.GetValue();
	
	auto &GenderRatio = GetSpecies().GetGenderRatio();
	if (GenderRatio.IsGenderless)
		return Genderless;

	return (PersonalityValue & UPersonalityValueUtils::LOWER_8_BITS) < GenderRatio.FemaleChance ? Female : Male;
}

int32 FGamePokemon::GetCurrentHP() const {
	return CurrentHP;
}

int32 FGamePokemon::GetMaxHP() const {
	return GetStatBlock().GetStat(GetHPStat()).GetStatValue();
}

bool FGamePokemon::IsFainted() const {
	return CurrentHP <= 0;
}

const FSpeciesData& FGamePokemon::GetSpecies() const {
	auto &DataTable = FDataManager::GetInstance().GetDataTable<FSpeciesData>();
	auto SpeciesData = DataTable.GetData(Species);
	check(SpeciesData != nullptr);

	return  *SpeciesData;
}

const IStatBlock& FGamePokemon::GetStatBlock() const {
	return *StatBlock;
}

FName FGamePokemon::GetHPStat() {
	static FName HPStat = TEXT("HP");
	return HPStat;
}
