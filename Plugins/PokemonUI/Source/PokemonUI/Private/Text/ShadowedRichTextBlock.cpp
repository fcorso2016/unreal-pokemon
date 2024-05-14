﻿// "Unreal Pokémon" created by Retro & Chill.


#include "Text/ShadowedRichTextBlock.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "RenderDeferredCleanup.h"

template< class ObjectType >
struct FDeferredDeletor : FDeferredCleanupInterface
{
    explicit FDeferredDeletor(ObjectType* InInnerObjectToDelete) : InnerObjectToDelete(InInnerObjectToDelete) {
    }

private:
    TUniquePtr<ObjectType> InnerObjectToDelete;
};

template<class ObjectType, typename... ArgsType>
static TSharedPtr<ObjectType> MakeShareableDeferredCleanup(ArgsType... Args)
{
    return MakeShareable(new ObjectType(Args...), [](ObjectType* ObjectToDelete) { BeginCleanup(new FDeferredDeletor<ObjectType>(ObjectToDelete)); });
}

TSharedRef<SWidget> UShadowedRichTextBlock::RebuildWidget() {
    auto MainText = Super::RebuildWidget();

    Overlay = SNew(SOverlay);

    TArray< TSharedRef< class ITextDecorator > > CreatedDecorators;
    CreateDecorators(CreatedDecorators);

    for (auto &Style : ModifiedTextStyles) {
        auto Marshaller = FRichTextLayoutMarshaller::Create(CreateMarkupParser(), CreateMarkupWriter(),
            CreatedDecorators, Style.StyleSet.Get());

        Style.RichTextBlock = SNew(SRichTextBlock)
                .TextStyle(bOverrideDefaultStyle ? &Style.DefaultTextStyleOverride : &Style.DefaultTextStyle)
                .Marshaller(Marshaller);
        
        Overlay->AddSlot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SAssignNew(Style.RichTextBlock, SRichTextBlock)
                .TextStyle(bOverrideDefaultStyle ? &Style.DefaultTextStyleOverride : &Style.DefaultTextStyle)
                .Marshaller(Marshaller)
        ];
    }
    
    Overlay->AddSlot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            MainText
        ];
    return Overlay.ToSharedRef();
}

void UShadowedRichTextBlock::SynchronizeProperties() {
    Super::SynchronizeProperties();

    for (auto &Style : ModifiedTextStyles) {
        if (Style.RichTextBlock == nullptr)
            continue;
        Style.RichTextBlock->SetText(GetText());
        Style.RichTextBlock->SetTransformPolicy(GetTransformPolicy());
        Style.RichTextBlock->SetMinDesiredWidth(GetMinDesiredWidth());
        Style.RichTextBlock->SetOverflowPolicy(GetOverflowPolicy());
        SynchronizeTextLayoutProperties(*Style.RichTextBlock);
    }
}

void UShadowedRichTextBlock::UpdateStyleData() {
    Super::UpdateStyleData();
    GenerateStyleSets();
}

void UShadowedRichTextBlock::ReleaseSlateResources(bool bReleaseChildren) {
    Super::ReleaseSlateResources(bReleaseChildren);

    Overlay.Reset();
    ModifiedTextStyles.Empty();
}

void UShadowedRichTextBlock::GenerateStyleSets() {
    ModifiedTextStyles.Empty();
    if (GetTextStyleSet() == nullptr || !GetTextStyleSet()->GetRowStruct()->IsChildOf(FRichTextStyleRow::StaticStruct())) {
        return;
    }
    
    ModifiedTextStyles.Emplace(MakeShareableDeferredCleanup<FSlateStyleSet>(TEXT("Shadow1")));
    ModifiedTextStyles.Emplace(MakeShareableDeferredCleanup<FSlateStyleSet>(TEXT("Shadow2")));
    SetDefaultStyleOverride(ModifiedTextStyles[0], FVector2D(0, 1));
    SetDefaultStyleOverride(ModifiedTextStyles[1], FVector2D(1, 0));
    
    for (const auto& Entry : GetTextStyleSet()->GetRowMap()) {
        FName SubStyleName = Entry.Key;
        auto RichTextStyle = reinterpret_cast<FRichTextStyleRow *>(Entry.Value);

        ProcessStyleSet(ModifiedTextStyles[0], SubStyleName, *RichTextStyle, FVector2D(0, 1));
        ProcessStyleSet(ModifiedTextStyles[1], SubStyleName, *RichTextStyle, FVector2D(1, 0));
    }
}

void UShadowedRichTextBlock::SetDefaultStyleOverride(FModifiedTextStyle &StyleSet, const FVector2D &ShadowMask) {
    StyleSet.DefaultTextStyleOverride = GetDefaultTextStyleOverride();
    StyleSet.DefaultTextStyleOverride.ShadowOffset.X *= ShadowMask.X;
    StyleSet.DefaultTextStyleOverride.ShadowOffset.Y *= ShadowMask.Y;
}

void UShadowedRichTextBlock::ProcessStyleSet(FModifiedTextStyle &StyleSet, FName SubStyleName, const FRichTextStyleRow &Row,
                                             const FVector2D &ShadowMask) {
    auto ModifiedTextStyle = Row.TextStyle;
    ModifiedTextStyle.ShadowOffset.X *= ShadowMask.X;
    ModifiedTextStyle.ShadowOffset.Y *= ShadowMask.Y;
    
    if (SubStyleName == FName(TEXT("Default"))) {
        StyleSet.DefaultTextStyle = ModifiedTextStyle;
    }

    StyleInstance->Set(SubStyleName, ModifiedTextStyle);
}