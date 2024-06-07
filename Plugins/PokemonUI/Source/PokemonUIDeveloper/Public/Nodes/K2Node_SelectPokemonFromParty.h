﻿// "Unreal Pokémon" created by Retro & Chill.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_ScreenBoundNode.h"

#include "K2Node_SelectPokemonFromParty.generated.h"

/**
 *
 */
UCLASS()
class POKEMONUIDEVELOPER_API UK2Node_SelectPokemonFromParty : public UK2Node_ScreenBoundNode {
    GENERATED_BODY()

  public:
    void GetMenuActions(FBlueprintActionDatabaseRegistrar &ActionRegistrar) const override;
    void ExpandNode(class FKismetCompilerContext &CompilerContext, UEdGraph *SourceGraph) override;

  protected:
    void ForEachValidScreen(const TFunctionRef<void(UClass *)> &Action) const final;
};
