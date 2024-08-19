﻿// "Unreal Pokémon" created by Retro & Chill.

#include "Graphics/GraphicsLoadingSubsystem.h"
#include "DynamicAssetLoadingSettings.h"
#include "Pokemon/Pokemon.h"
#include "PokemonDataSettings.h"
#include "range/v3/view/filter.hpp"
#include "range/v3/view/transform.hpp"
#include "Ranges/Algorithm/ToArray.h"
#include "Ranges/Views/ContainerView.h"
#include "Species/SpeciesData.h"
#include "TextureCompiler.h"
#include "Ranges/Optional/GetPtrOrNull.h"
#include "Ranges/Optional/Map.h"
#include "Ranges/Optional/Or.h"
#include "Ranges/Optional/OrElse.h"
#include "Ranges/Views/Extract.h"
#include "Ranges/Views/Map.h"
#include "Trainers/Trainer.h"
#include "Trainers/TrainerType.h"
#include <cmath>

template <typename T>
    requires std::is_base_of_v<UObject, T>
static TOptional<T&> LookupAssetByName(FStringView BasePackageName, FStringView AssetName) {
    FStringView Prefix;
    if (int32 CharIndex; AssetName.FindLastChar('/', CharIndex)) {
        int32 PrefixLength = CharIndex + 1;
        Prefix = AssetName.SubStr(0, PrefixLength);
        AssetName = AssetName.RightChop(PrefixLength);
    }
    auto SearchKey = FString::Format(TEXT("{0}/{1}{2}.{2}"), {BasePackageName, Prefix, AssetName});
    return UE::Optionals::OfNullable<T>(Cast<T>(StaticLoadObject(T::StaticClass(),
        nullptr, *SearchKey, nullptr, LOAD_NoWarn)));
}

template <typename T>
    requires std::is_base_of_v<UObject, T>
static TOptional<T&> LookupAssetByName(FStringView BasePackageName, FName AssetName) {
    return LookupAssetByName<T>(BasePackageName, AssetName.ToString());
}

static FString GetFullAssetName(FName Identifier, FStringView Prefix) {
    return FString::Format(TEXT("{0}{1}"), {Prefix, Identifier.ToString()});
}

/**
 * Fetch the first matching asset for the provided keys
 * @tparam T The type of asset we're looking for
 * @param BasePackageName The base package to search for when resolving
 * @param Keys The keys to use when resolving
 * @return The found asset, if it exists
 */
template <typename T>
    requires std::is_base_of_v<UObject, T>
static TOptional<T&> ResolveAsset(FStringView BasePackageName, const TArray<FString> &Keys) {
    for (const auto &Key : Keys) {
        auto Lookup = LookupAssetByName<T>(BasePackageName, Key);
        if (!Lookup.IsSet()) {
            continue;
        }

        return Lookup;
    }

    return nullptr;
}

static TArray<FString> CreatePokemonSpriteResolutionList(FName Species, const FPokemonAssetParams &Params,
                                                         FStringView Subfolder);

static FMaterialInstanceWithSize ConvertTextureToMaterial(UTexture2D& Texture, const TSoftObjectPtr<UMaterialInterface>& BaseMaterial, UObject* Outer) {
#if WITH_EDITOR
    FTextureCompilingManager::Get().FinishCompilation({&Texture});
#endif

    static FName SourceTexture = "SourceTexture";
    auto Material =
    UMaterialInstanceDynamic::Create(BaseMaterial.LoadSynchronous(), Outer);
    Material->SetTextureParameterValue(SourceTexture, &Texture);
    int32 Height = Texture.GetSizeY();
    return {Material, FVector2D(Height, Height)};
}

void UGraphicsLoadingSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
    Super::Initialize(Collection);

    auto Settings = GetDefault<UDynamicAssetLoadingSettings>();
    PokemonSpriteMaterials = Settings->PokemonSprites;
    TrainerSpriteMaterials = Settings->TrainerSprites;
}

FMaterialInstanceWithSize UGraphicsLoadingSubsystem::GetPokemonBattleSprite(const TScriptInterface<IPokemon> &Pokemon,
                                                                            UObject *Outer, bool bBack) const {
    return GetSpeciesBattleSprite(Pokemon->GetSpecies().ID, Outer, bBack,
                                  {.Gender = Pokemon->GetGender(), .bShiny = Pokemon->IsShiny()});
}

FMaterialInstanceWithSize
UGraphicsLoadingSubsystem::GetSpeciesBattleSprite(FName Species, UObject *Outer, bool bBack,
                                                  const FPokemonAssetParams &AdditionalParams) const {
    auto &[AssetPath] = GetDefault<UDynamicAssetLoadingSettings>()->PokemonSpritePackageName;
    auto SpriteResolutionList =
        CreatePokemonSpriteResolutionList(Species, AdditionalParams, bBack ? TEXT("Back") : TEXT("Front"));
    return ResolveAsset<UTexture2D>(AssetPath, SpriteResolutionList) |
        UE::Optionals::Map(&ConvertTextureToMaterial, PokemonSpriteMaterials.BattleSpritesMaterial, Outer) |
        UE::Optionals::OrElse(FMaterialInstanceWithSize{nullptr, FVector2D()});
}

FMaterialInstanceWithSize UGraphicsLoadingSubsystem::GetPokemonUISprite(const TScriptInterface<IPokemon> &Pokemon,
                                                                        UObject *Outer, bool bBack) const {
    return GetSpeciesUISprite(Pokemon->GetSpecies().ID, Outer, bBack,
                              {.Gender = Pokemon->GetGender(), .bShiny = Pokemon->IsShiny()});
}

FMaterialInstanceWithSize
UGraphicsLoadingSubsystem::GetSpeciesUISprite(FName Species, UObject *Outer, bool bBack,
                                              const FPokemonAssetParams &AdditionalParams) const {
    auto &[AssetPath] = GetDefault<UDynamicAssetLoadingSettings>()->PokemonSpritePackageName;
    auto SpriteResolutionList =
        CreatePokemonSpriteResolutionList(Species, AdditionalParams, bBack ? TEXT("Back") : TEXT("Front"));
    return ResolveAsset<UTexture2D>(AssetPath, SpriteResolutionList) |
        UE::Optionals::Map(&ConvertTextureToMaterial, PokemonSpriteMaterials.UISpritesMaterial, Outer) |
        UE::Optionals::OrElse(FMaterialInstanceWithSize{nullptr, FVector2D()});
}

FMaterialInstanceWithSize UGraphicsLoadingSubsystem::GetPokemonIcon(const TScriptInterface<IPokemon> &Pokemon,
                                                                    UObject *Outer) {
    return GetSpeciesIcon(Pokemon->GetSpecies().ID, Outer, {.Gender = Pokemon->GetGender()});
}

FMaterialInstanceWithSize UGraphicsLoadingSubsystem::GetSpeciesIcon(FName Species, UObject *Outer,
                                                                    const FPokemonAssetParams &AdditionalParams) {
    auto &[AssetPath] = GetDefault<UDynamicAssetLoadingSettings>()->PokemonSpritePackageName;
    auto SpriteResolutionList = CreatePokemonSpriteResolutionList(Species, AdditionalParams, TEXT("Icons"));
    auto Texture = ResolveAsset<UTexture2D>(AssetPath, SpriteResolutionList);
    return ResolveAsset<UTexture2D>(AssetPath, SpriteResolutionList) |
        UE::Optionals::Map(&ConvertTextureToMaterial, PokemonSpriteMaterials.IconMaterial, Outer) |
        UE::Optionals::OrElse(FMaterialInstanceWithSize{nullptr, FVector2D()});
}

FMaterialInstanceWithSize UGraphicsLoadingSubsystem::GetTrainerSprite(const TScriptInterface<ITrainer> &Trainer,
                                                                      UObject *Outer) const {
    return GetTrainerTypeSprite(Trainer->GetTrainerType().ID, Outer);
}

FMaterialInstanceWithSize UGraphicsLoadingSubsystem::GetTrainerTypeSprite(FName TrainerType, UObject *Outer) const {
    auto &[AssetPath] = GetDefault<UDynamicAssetLoadingSettings>()->TrainerSpritesPackageName;
    return LookupAssetByName<UTexture2D>(AssetPath, TrainerType.ToString()) |
        UE::Optionals::Map(&ConvertTextureToMaterial, TrainerSpriteMaterials.FrontSpriteBaseMaterialUI, Outer) |
        UE::Optionals::OrElse(FMaterialInstanceWithSize{nullptr, FVector2D()});
}

UObject *UGraphicsLoadingSubsystem::GetTypeIconGraphic(FName Type) const {
    auto &PathSettings = *GetDefault<UDynamicAssetLoadingSettings>();
    auto &[AssetPath] = PathSettings.TypeIconsPackageName;
    auto FullName = GetFullAssetName(Type, PathSettings.TypeIconPrefix);
    return LookupAssetByName<UObject>(AssetPath, FullName).GetPtrOrNull();
}

TArray<UObject *> UGraphicsLoadingSubsystem::GetTypeIconGraphics(const TArray<FName> &Types) const {
    auto &PathSettings = *GetDefault<UDynamicAssetLoadingSettings>();
    auto &[AssetPath] = PathSettings.TypeIconsPackageName;
    static_assert(UE::Ranges::FunctionalType<decltype(&GetFullAssetName)>);
    auto Binding = Types | UE::Ranges::Map(&GetFullAssetName, PathSettings.TypeIconPrefix);
    auto Mapping = UE::Ranges::Map([&AssetPath](FStringView Name) { return LookupAssetByName<UObject>(AssetPath, Name); });
    return Types | UE::Ranges::Map(&GetFullAssetName, PathSettings.TypeIconPrefix) |
           UE::Ranges::Map([&AssetPath](FStringView Name) { return LookupAssetByName<UObject>(AssetPath, Name); }) |
           UE::Ranges::Extract |
           UE::Ranges::ToArray;
}

UObject *UGraphicsLoadingSubsystem::GetStatusIconGraphic(FName Status) const {
    auto &PathSettings = *GetDefault<UDynamicAssetLoadingSettings>();
    auto &[AssetPath] = PathSettings.StatusIconsPackageName;
    auto FullName = GetFullAssetName(Status, PathSettings.StatusIconPrefix);
    return LookupAssetByName<UObject>(AssetPath, FullName).GetPtrOrNull();
}

UObject *UGraphicsLoadingSubsystem::GetTypePanelGraphic(FName Type) const {
    auto &PathSettings = *GetDefault<UDynamicAssetLoadingSettings>();
    auto &[AssetPath] = PathSettings.TypePanelsPackageName;
    auto FullName = GetFullAssetName(Type, PathSettings.TypePanelPrefix);
    return LookupAssetByName<UObject>(AssetPath, FullName).GetPtrOrNull();
}

UObject *UGraphicsLoadingSubsystem::GetPokeBallIcon(FName PokeBall) const {
    auto &PathSettings = *GetDefault<UDynamicAssetLoadingSettings>();
    auto &[AssetPath] = PathSettings.SummaryBallPackageName;
    auto FullName = GetFullAssetName(PokeBall, PathSettings.SummaryBallPrefix);
    return LookupAssetByName<UObject>(AssetPath, FullName) |
        UE::Optionals::Or([this, &AssetPath] {
            return LookupAssetByName<UObject>(AssetPath, GetDefault<UPokemonDataSettings>()->DefaultPokeBall.ToString());
        }) | UE::Optionals::GetPtrOrNull;
}

UObject *UGraphicsLoadingSubsystem::GetItemIcon(FName ItemID) const {
    static const FName DefaultItem = "000";
    auto &[AssetPath] = GetDefault<UDynamicAssetLoadingSettings>()->ItemIconPackageName;
    return LookupAssetByName<UObject>(AssetPath, ItemID) |
        UE::Optionals::Or([&AssetPath] { return LookupAssetByName<UObject>(AssetPath, DefaultItem); }) |
        UE::Optionals::GetPtrOrNull;
}

static TArray<FString> CreatePokemonSpriteResolutionList(FName Species, const FPokemonAssetParams &Params,
                                                         FStringView Subfolder) {
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
            const auto &[FactorIndex, Extension, Fallback] = Factors[j];
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
        auto FormattedName =
            FString::Format(TEXT("{0}{1}{2}{3}{4}"), {TrySubfolder, TrySpecies, TryForm, TryGender, TryShadow});
        FormattedStrings.Add(MoveTemp(FormattedName));
    }

    return FormattedStrings;
}