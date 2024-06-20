﻿#include "Asserts.h"
#include "Battle/Actions/BattleAction.h"
#include "Battle/Battlers/Battler.h"
#include "Battle/BattleSide.h"
#include "Battle/PokemonBattle.h"
#include "External/accessor.hpp"
#include "Misc/AutomationTest.h"
#include "Mocking/UnrealMock.h"
#include "Mocks/MockBattleAction.h"
#include "Mocks/MockBattler.h"
#include "Mocks/MockBattleSide.h"
#include "Utilities/WidgetTestUtilities.h"
#include "UtilityClasses/BattleActors/TestPokemonBattle.h"

class FMockBattleAction;
class FMockBattleSide;

using namespace testing;
using namespace accessor;

MEMBER_ACCESSOR(AccessActionsQueueing, APokemonBattle, SelectedActions, TArray<TUniquePtr<IBattleAction>>)
MEMBER_ACCESSOR(AccessActionQueueQueueing, APokemonBattle, ActionQueue, TQueue<TUniquePtr<IBattleAction>>)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestActionQueueing, "Unit Tests.Battle.TestActionQueueing",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestActionQueueing::RunTest(const FString &Parameters) {
    auto [DudOverlay, World, GameInstance] = UWidgetTestUtilities::CreateTestWorld();
    CREATE_MOCK(IBattleSide, Side1, FMockBattleSide, MockSide1);
    CREATE_MOCK(IBattleSide, Side2, FMockBattleSide, MockSide2);

    auto Battle = World->SpawnActor<ATestPokemonBattle>();
    Battle->Initialize({Side1, Side2});

    auto QueueBattleAction = [&Battle](const TScriptInterface<IBattler> &Battler) {
        auto MockAction = MakeUnique<FMockBattleAction>();
        ON_CALL(*MockAction, GetBattler).WillByDefault(ReturnRef(Battler));
        Battle->QueueAction(MoveTemp(MockAction));
    };

    TArray<TScriptInterface<IBattler>> Side1Battlers;
    CREATE_MOCK(IBattler, Battler1, FMockBattler, MockBattler1);
    auto Battler1ID = FGuid::NewGuid();
    ON_CALL(MockBattler1, GetInternalId).WillByDefault(Return(Battler1ID));
    ON_CALL(MockBattler1, SelectActions).WillByDefault(std::bind_front(QueueBattleAction, Battler1));
    ON_CALL(MockBattler1, GetActionCount).WillByDefault(Return(1));
    ON_CALL(MockBattler1, IsFainted).WillByDefault(Return(false));
    ON_CALL(MockBattler1, GetNickname).WillByDefault(Return(FText::FromStringView(TEXT("Battler 1"))));
    Side1Battlers.Add(Battler1);
    CREATE_MOCK(IBattler, Battler2, FMockBattler, MockBattler2);
    auto Battler2ID = FGuid::NewGuid();
    ON_CALL(MockBattler2, GetInternalId).WillByDefault(Return(Battler2ID));
    ON_CALL(MockBattler2, SelectActions).WillByDefault(std::bind_front(QueueBattleAction, Battler2));
    ON_CALL(MockBattler2, GetActionCount).WillByDefault(Return(1));
    ON_CALL(MockBattler2, IsFainted).WillByDefault(Return(false));
    ON_CALL(MockBattler2, GetNickname).WillByDefault(Return(FText::FromStringView(TEXT("Battler 2"))));
    Side1Battlers.Add(Battler2);
    ON_CALL(MockSide1, GetBattlers).WillByDefault(ReturnRef(Side1Battlers));

    TArray<TScriptInterface<IBattler>> Side2Battlers;
    CREATE_MOCK(IBattler, Battler3, FMockBattler, MockBattler3);
    auto Battler3ID = FGuid::NewGuid();
    ON_CALL(MockBattler3, GetInternalId).WillByDefault(Return(Battler3ID));
    ON_CALL(MockBattler3, SelectActions).WillByDefault(std::bind_front(QueueBattleAction, Battler3));
    ON_CALL(MockBattler3, GetActionCount).WillByDefault(Return(2));
    ON_CALL(MockBattler3, IsFainted).WillByDefault(Return(false));
    ON_CALL(MockBattler3, GetNickname).WillByDefault(Return(FText::FromStringView(TEXT("Battler 3"))));
    Side2Battlers.Add(Battler3);
    ON_CALL(MockSide2, GetBattlers).WillByDefault(ReturnRef(Side2Battlers));

    Battle->StartBattle();
    UE_ASSERT_FALSE(Battle->ActionSelectionFinished());
    AddExpectedError(TEXT("Battler 1 attempted to queue an action, but is already at capacity!"));
    QueueBattleAction(Battler1);
    UE_ASSERT_FALSE(Battle->ActionSelectionFinished());
    QueueBattleAction(Battler3);
    UE_ASSERT_TRUE(Battle->ActionSelectionFinished());

    accessMember<AccessActionsQueueing>(*Battle).get().Empty();
    accessMember<AccessActionQueueQueueing>(*Battle).get().Empty();
    return true;
}