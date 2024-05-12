﻿// "Unreal Pokémon" created by Retro & Chill.

#include "Graphics/GraphicsLoadingSubsystem.h"
#include "Pokemon/Pokemon.h"
#include "Repositories/StaticImageRepository.h"
#include "Repositories/TextureRepository.h"
#include "Settings/AssetLoaderSettings.h"
#include "Settings/SpriteMaterialSettings.h"
#include "Species/SpeciesData.h"
#include "Trainers/Trainer.h"
#include "Trainers/TrainerType.h"

#include <cmath>

static TArray<FName> CreatePokemonSpriteResolutionList(FName Species, const FPokemonAssetParams &Params, FStringView Subfolder);

void UGraphicsLoadingSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
    Super::Initialize(Collection);

    auto SpriteMaterialSettings = GetDefault<USpriteMaterialSettings>();
    PokemonSpriteMaterials = SpriteMaterialSettings->GetPokemonSpriteSettings();
    TrainerSpriteMaterials = SpriteMaterialSettings->GetTrainerSpriteSettings();
}

std::pair<UMaterialInstanceDynamic *, FVector2D> UGraphicsLoadingSubsystem::GetPokemonBattleSprite(
    const IPokemon &Pokemon,
    UObject *Outer, bool bBack) const {
    return GetPokemonBattleSprite(Pokemon.GetSpecies().ID, Outer, bBack, {
        .Gender = Pokemon.GetGender(),
        .bShiny = Pokemon.IsShiny()
    });
}

std::pair<UMaterialInstanceDynamic *, FVector2D> UGraphicsLoadingSubsystem::GetPokemonBattleSprite(FName Species,
    UObject *Outer, bool bBack, const FPokemonAssetParams &AdditionalParams) const {
    auto SpriteResolutionList = CreatePokemonSpriteResolutionList(Species, AdditionalParams,
        bBack ? TEXT("Back") : TEXT("Front"));
    auto Texture = GetDefault<UAssetLoaderSettings>()->GetPokemonSpriteRepository()->ResolveAsset(SpriteResolutionList);
    if (Texture == nullptr) {
        return {nullptr, FVector2D()};
    }

    static FName SourceTexture = "SourceTexture";
    auto Material = UMaterialInstanceDynamic::Create(PokemonSpriteMaterials.BattleSpritesMaterial.Get(), Outer);
    Material->SetTextureParameterValue(SourceTexture, Texture);
    return {Material, FVector2D(Texture->GetSizeY(), Texture->GetSizeY())};
}

UMaterialInstanceDynamic *UGraphicsLoadingSubsystem::GetPokemonIcon(const IPokemon &Pokemon, UObject *Outer) {
    return GetPokemonIcon(Pokemon.GetSpecies().ID, Outer, {
        .Gender = Pokemon.GetGender()
    });
}

UMaterialInstanceDynamic * UGraphicsLoadingSubsystem::GetPokemonIcon(FName Species, UObject *Outer,
    const FPokemonAssetParams &AdditionalParams) {
    auto SpriteResolutionList = CreatePokemonSpriteResolutionList(Species, AdditionalParams, TEXT("Icons"));
    auto Texture = GetDefault<UAssetLoaderSettings>()->GetPokemonSpriteRepository()->ResolveAsset(SpriteResolutionList);
    if (Texture == nullptr) {
        return nullptr;
    }

    static FName SourceTexture = "SourceTexture";
    auto Material = UMaterialInstanceDynamic::Create(PokemonSpriteMaterials.IconMaterial.Get(), Outer);
    Material->SetTextureParameterValue(SourceTexture, Texture);
    return Material;
}

std::pair<UMaterialInstanceDynamic *, FVector2D> UGraphicsLoadingSubsystem::GetTrainerSprite(const ITrainer &Trainer,
                                                                                         UObject *Outer) const {
    return GetTrainerSprite(Trainer.GetTrainerType().ID, Outer);
}

std::pair<UMaterialInstanceDynamic *, FVector2D> UGraphicsLoadingSubsystem::GetTrainerSprite(FName TrainerType,
                                                                                         UObject *Outer) const {
    auto Texture = GetDefault<UAssetLoaderSettings>()->GetTrainerFrontSpriteRepository()->FetchAsset(TrainerType);
    if (Texture == nullptr) {
        return {nullptr, FVector2D()};
    }

    static FName SourceTexture = "SourceTexture";
    auto Material = UMaterialInstanceDynamic::Create(TrainerSpriteMaterials.FrontSpriteBaseMaterialUI.Get(), Outer);
    Material->SetTextureParameterValue(SourceTexture, Texture);
    return {Material, FVector2D(Texture->GetSizeY(), Texture->GetSizeY())};
}

TArray<UObject *> UGraphicsLoadingSubsystem::GetTypeIconGraphics(TArrayView<FName> Types) const {
    auto Repo = GetDefault<UAssetLoaderSettings>()->GetTypeIconRepository();
    TArray<UObject*> Ret;
    Algo::Transform(Types, Ret, [Repo](FName Type) {
        return Repo->FetchAsset(Type);
    });
    return Ret;
}

UObject * UGraphicsLoadingSubsystem::GetPokeBallIcon(FName PokeBall) const {
    return GetDefault<UAssetLoaderSettings>()->GetTypeIconRepository()->FetchAsset(PokeBall);
}

static TArray<FName> CreatePokemonSpriteResolutionList(FName Species,
    const FPokemonAssetParams &Params, FStringView Subfolder) {
    auto SubfolderString = FString::Format(TEXT("{0}/"), {Subfolder});
    auto ShinyExtension = FString::Format(TEXT("{0}Shiny/"), {Subfolder});
    auto FormExtension = FString::Format(TEXT("_{0}"), {{Params.Form}});
    auto SpeciesExtension = Species.ToString();
    TArray<std::tuple<int32, FStringView, FStringView>> Factors;
    if (Params.bShiny) {
        Factors.Emplace(4, ShinyExtension, SubfolderString);
    }
    if (Params.bShadow) {
        Factors.Emplace(3, TEXT("_shadow"), TEXT(""));
    }
    if (Params.Gender == EPokemonGender::Female) {
        Factors.Emplace(2, TEXT("_female"), TEXT(""));
    }
    if (Params.Form > 0) {
        Factors.Emplace(1, FormExtension, TEXT(""));
    }
    Factors.Emplace(0, SpeciesExtension, TEXT("000"));

    TArray<FString> FormattedStrings;
    auto FactorPower = static_cast<int32>(std::pow(2, Factors.Num()));
    for (int i = 0; i < FactorPower; i++) {
        FStringView TrySpecies;
        FStringView TryForm;
        FStringView TryGender;
        FStringView TryShadow;
        FStringView TrySubfolder = SubfolderString;
        for (int j = 0; j < Factors.Num(); j++) {
            auto &[FactorIndex, Extension, Fallback] = Factors[j];
            auto TestValue = i / static_cast<int32>(std::pow(2, j)) % 2 == 0 ? Extension : Fallback;
            switch (FactorIndex) {
            case 0:
                TrySpecies = TestValue;
                break;
            case 1:
                TryForm = TestValue;
                break;
            case 2:
                TryGender = TestValue;
                break;
            case 3:
                TryShadow = TestValue;
                break;
            default:
                TrySubfolder = TestValue;
                break;
            }
        }
        auto FormattedName = FString::Format(TEXT("{0}{1}{2}{3}{4}"),
                {TrySubfolder, TrySpecies, TryForm, TryGender, TryShadow}
            );
        FormattedStrings.Add(MoveTemp(FormattedName));
    }
    
    TArray<FName> Ret;
    TSet<FStringView> Explored;
    for (const auto &Name : FormattedStrings) {
        if (Explored.Contains(Name)) {
            continue;
        }
        
        Explored.Add(Name);
        Ret.Emplace(*Name);
    }
    return Ret;
}