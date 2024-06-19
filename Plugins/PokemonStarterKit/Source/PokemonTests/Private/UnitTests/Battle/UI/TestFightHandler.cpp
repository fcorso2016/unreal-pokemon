﻿#include "Asserts.h"
#include "Battle/Battle.h"
#include "Battle/BattleSide.h"
#include "Data/SelectionInputs.h"
#include "External/accessor.hpp"
#include "Handlers/FightHandler.h"
#include "Lookup/InjectionUtilities.h"
#include "Misc/AutomationTest.h"
#include "Mocking/UnrealMock.h"
#include "Pokemon/Pokemon.h"
#include "Pokemon/PokemonDTO.h"
#include "Screens/PokemonBattleScreen.h"
#include "Utilities/InputUtilities.h"
#include "Utilities/ReflectionUtils.h"
#include "Utilities/WidgetTestUtilities.h"
#include "UtilityClasses/BattleActors/TestBattlerActor.h"
#include "Windows/BattleMoveSelect.h"
#include "Windows/PokemonActionOptions.h"
#include <range/v3/view/single.hpp>

using namespace accessor;
using namespace fakeit;

MEMBER_ACCESSOR(AccessInputMappingsBattleScreen, USelectableWidget, InputMappings, TObjectPtr<USelectionInputs>)
MEMBER_ACCESSOR(AccessConfirmInputBattleScreen, USelectionInputs, ConfirmInputs, TSet<FKey>)
MEMBER_ACCESSOR(AccessCancelInputBattleScreen, USelectionInputs, CancelInputs, TSet<FKey>)
MEMBER_ACCESSOR(AccessFightHandlers, UPokemonActionOptions, MenuActions, TArray<TObjectPtr<UBattleMenuHandler>>)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestFightHandler, "Unit Tests.Battle.UI.TestFightHandler",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestFightHandler::RunTest(const FString &Parameters) {
    auto [DudOverlay, World, GameInstance] = UWidgetTestUtilities::CreateTestWorld();
    auto Subclasses = UReflectionUtils::GetAllSubclassesOfClass<UPokemonBattleScreen>();
    UE_ASSERT_NOT_EQUAL(0, Subclasses.Num());
    auto WidgetClass = Subclasses[0];

    TWidgetPtr<UPokemonBattleScreen> Screen(CreateWidget<UPokemonBattleScreen>(World.Get(), WidgetClass));
    Screen->AddToViewport();

    FIND_CHILD_WIDGET(Screen.Get(), UPokemonActionOptions, ActionSelect);
    UE_ASSERT_NOT_NULL(ActionSelect);
    FIND_CHILD_WIDGET(Screen.Get(), UBattleMoveSelect, MoveSelect);
    UE_ASSERT_NOT_NULL(MoveSelect);

    auto [Battle, MockBattle] = UnrealMock::CreateMock<IBattle>();
    auto [Side, MockSide] = UnrealMock::CreateMock<IBattleSide>();
    Fake(Method(MockBattle, GetActiveBattlers));
    Fake(Method(MockBattle, QueueAction));
    ON_CALL(MockSide, GetOwningBattle).WillByDefault(Return(Battle));
    ON_CALL(MockSide, ShowBackSprites).WillByDefault(Return(false));
    ON_CALL(MockBattle, GetSides)).WillByDefault(Return(ranges::views::single(Side));
    auto Pokemon1 = UnrealInjector::NewInjectedDependency<IPokemon>(
        World.Get(),
        FPokemonDTO{.Species = TEXT("MIMIKYU"),
                    .Level = 50,
                    .Moves = {TEXT("SHADOWSNEAK"), TEXT("PLAYROUGH"), TEXT("SWORDSDANCE"), TEXT("SHADOWCLAW")}});
    auto Battler1 = World->SpawnActor<ATestBattlerActor>();
    Battler1->Initialize(Side, Pokemon1);

    auto Pokemon2 = UnrealInjector::NewInjectedDependency<IPokemon>(
        World.Get(),
        FPokemonDTO{.Species = TEXT("LUCARIO"),
                    .Level = 50,
                    .Moves = {TEXT("AURASPHERE"), TEXT("DRAGONPULSE"), TEXT("FLASHCANNON"), TEXT("NASTYPLOT")}});
    auto Battler2 = World->SpawnActor<ATestBattlerActor>();
    Battler2->Initialize(Side, Pokemon2);

    TArray<TScriptInterface<IBattler>> Battlers = {Battler1, Battler2};
    ON_CALL(MockSide, GetBattlers).WillByDefault(Return(Battlers));
    Screen->SetBattle(Battle);

    Screen->SelectAction(Battler1);
    Screen->SelectAction(Battler2);
    UE_CHECK_EQUAL(ESlateVisibility::Visible, ActionSelect->GetVisibility());

    USelectionInputs *InputMappings = accessMember<AccessInputMappingsBattleScreen>(*ActionSelect).get();
    UE_ASSERT_NOT_NULL(InputMappings);
    auto ConfirmButton = *accessMember<AccessConfirmInputBattleScreen>(*InputMappings).get().CreateIterator();
    auto CancelButton = *accessMember<AccessCancelInputBattleScreen>(*InputMappings).get().CreateIterator();

    auto &Options = accessMember<AccessFightHandlers>(*ActionSelect).get();
    int32 FightHandlerIndex =
        Options.IndexOfByPredicate([](UBattleMenuHandler *Handler) { return Handler->IsA<UFightHandler>(); });
    ActionSelect->SetIndex(FightHandlerIndex);
    UInputUtilities::SimulateKeyPress(ActionSelect, ConfirmButton);
    UE_CHECK_EQUAL(ESlateVisibility::Visible, MoveSelect->GetVisibility());
    UE_CHECK_EQUAL(ESlateVisibility::Hidden, ActionSelect->GetVisibility());

    UInputUtilities::SimulateKeyPress(MoveSelect, CancelButton);
    UE_CHECK_EQUAL(ESlateVisibility::Hidden, MoveSelect->GetVisibility());
    UE_CHECK_EQUAL(ESlateVisibility::Visible, ActionSelect->GetVisibility());

    ActionSelect->SetIndex(FightHandlerIndex);
    UInputUtilities::SimulateKeyPress(ActionSelect, ConfirmButton);
    UE_CHECK_EQUAL(ESlateVisibility::Visible, MoveSelect->GetVisibility());
    UE_CHECK_EQUAL(ESlateVisibility::Hidden, ActionSelect->GetVisibility());

    UInputUtilities::SimulateKeyPress(MoveSelect, ConfirmButton);
    UE_CHECK_EQUAL(ESlateVisibility::Hidden, MoveSelect->GetVisibility());
    UE_CHECK_EQUAL(ESlateVisibility::Visible, ActionSelect->GetVisibility());

    ActionSelect->SetIndex(FightHandlerIndex);
    UInputUtilities::SimulateKeyPress(ActionSelect, ConfirmButton);
    UE_CHECK_EQUAL(ESlateVisibility::Visible, MoveSelect->GetVisibility());
    UE_CHECK_EQUAL(ESlateVisibility::Hidden, ActionSelect->GetVisibility());

    UInputUtilities::SimulateKeyPress(MoveSelect, ConfirmButton);
    UE_CHECK_EQUAL(ESlateVisibility::Hidden, MoveSelect->GetVisibility());
    UE_CHECK_EQUAL(ESlateVisibility::Hidden, ActionSelect->GetVisibility());

    return true;
}