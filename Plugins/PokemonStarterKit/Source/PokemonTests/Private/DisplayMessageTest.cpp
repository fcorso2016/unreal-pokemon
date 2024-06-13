﻿#include "Nodes/DisplayMessage.h"
#include "Asserts.h"
#include "BlueprintActionDatabase.h"
#include "External/accessor.hpp"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/AutomationTest.h"
#include "Screens/TextDisplayScreen.h"
#include "UI/K2Node_DisplayMessage.h"
#include "Utilities/K2Nodes.h"
#include "Utilities/ReflectionUtils.h"
#include "Utilities/WidgetTestUtilities.h"

using namespace accessor;

MEMBER_ACCESSOR(AccessProxyFactory, UK2Node_BaseAsyncTask, ProxyFactoryClass, TObjectPtr<UClass>)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(DisplayMessageTest, "Unit Tests.Nodes.DisplayMessageTest.DisplayMessage",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool DisplayMessageTest::RunTest(const FString &Parameters) {
    auto Subclasses = UReflectionUtils::GetAllSubclassesOfClass<UTextDisplayScreen>();
    ASSERT_NOT_EQUAL(0, Subclasses.Num());
    auto WidgetClass = Subclasses[0];

    using enum ENodeTitleType::Type;
    FBlueprintActionDatabase::Get().RefreshAll();

    MakeTestableBP(TestBP, TestGraph);

    auto TestNode = NewObject<UK2Node_DisplayMessage>(TestGraph.Get());
    AddExpectedError(TEXT("Was a class deleted or saved on a non promoted build?"),
                     EAutomationExpectedErrorFlags::MatchType::Contains, 0);

    auto Count = MakeShared<uint32>(1);
    TestNode->Initialize(WidgetClass, Count);
    CHECK_EQUAL(TEXT("Async Task: Missing Function"), TestNode->GetNodeTitle(MenuTitle).ToString());

    (*Count)++;
    auto ExpandedName =
        FString::Format(TEXT("Async Task: Missing Function ({0})"), {WidgetClass->GetDisplayNameText().ToString()});
    CHECK_EQUAL(ExpandedName, TestNode->GetNodeTitle(MenuTitle).ToString());

    return true;
}