﻿// "Unreal Pokémon" created by Retro & Chill.


#include "Tilemap/Tilemap3D.h"
#include "Algo/ForEach.h"
#include "Tileset/Tileset3D.h"


FTileHandle::FTileHandle(UTileset3D &InTileset, int32 InTileID) : Tileset(InTileset), TileID(InTileID) {
}

TOptional<const FTile3D &> FTileHandle::GetTile() const {
    auto &Tiles = Tileset->GetTiles();
    return Tiles.IsValidIndex(TileID) ? Tiles[TileID] : TOptional<const FTile3D&>();
}

bool FTileHandle::IsValidTile() const {
    auto &Tiles = Tileset->GetTiles();
    return Tiles.IsValidIndex(TileID);
}

FTileInfo::FTileInfo(UStaticMeshComponent &InTileMesh, const FTileHandle& InTile, const FIntVector2 &InTileOrigin)
    : TileMesh(InTileMesh), Tile(InTile), TileOrigin(InTileOrigin) {
    auto TileInfo = Tile.GetTile();
    if (TileInfo.IsSet()) {
        TileMesh->SetStaticMesh(TileInfo->TargetMesh.LoadSynchronous());
    }
}

bool FTileInfo::IsValidTile() const {
    return IsValid(TileMesh) && Tile.IsValidTile();
}

UStaticMeshComponent * FTileInfo::GetMeshComponent() const {
    return TileMesh;
}

FTileRow::FTileRow(int32 Size) {
    Tiles.SetNum(Size);
}

ATilemap3D::ATilemap3D() {
    TilemapRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TilemapRoot"));
    TilemapRoot->SetMobility(EComponentMobility::Type::Static);
    SetRootComponent(TilemapRoot);
}

void ATilemap3D::AddTile(const FTileHandle& Tile, int32 X, int32 Y, int32 Layer) {
    auto &CurrentTile = GetAdjustedTilePosition(X, Y);
    if (CurrentTile.IsValidTile()) {
        auto Mesh = CurrentTile.GetMeshComponent();
        Mesh->DestroyComponent();
        RemoveOwnedComponent(Mesh);
    }
    auto NewComponent = CastChecked<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(),
        false, FTransform(FVector(TileSize.X * X, TileSize.Y * Y, 0)), false));
    NewComponent->CreationMethod = EComponentCreationMethod::Instance;
    AddOwnedComponent(NewComponent);
    CurrentTile = FTileInfo(*NewComponent, Tile, FIntVector2(X, Y));
    OnTileChanged.Broadcast(X, Y, Layer);
}

void ATilemap3D::RemoveTile(int32 X, int32 Y, int32 Layer) {
    auto &Tile = GetAdjustedTilePosition(X, Y);
    if (Tile.IsValidTile()) {
        auto Mesh = Tile.GetMeshComponent();
        Mesh->DestroyComponent(false);
        RemoveOwnedComponent(Mesh);
    }
    Tile = FTileInfo();
    OnTileChanged.Broadcast(X, Y, Layer);
}

void ATilemap3D::OnConstruction(const FTransform &Transform) {
    Super::OnConstruction(Transform);
    for (int32 i = 0; i < TileRows.Num(); i++) {
        if (i >= SizeY) {
            Algo::ForEach(TileRows[i].Tiles, [this](const FTileInfo& Tile) {
                auto Component = Tile.GetMeshComponent();
                if (Component == nullptr) {
                    return;
                }
                
                Component->DestroyComponent(false);
                RemoveOwnedComponent(Component);
            });
        }

        auto &TileArray = TileRows[i].Tiles;
        for (int32 j = SizeX; j < TileArray.Num(); j++) {
            auto &Tile = TileArray[j];
            if (!Tile.IsValidTile()) {
                continue;
            }

            auto Mesh = Tile.GetMeshComponent();
            Mesh->DestroyComponent(false);
            RemoveOwnedComponent(Mesh);
        }
    }
    
    TileRows.SetNum(SizeY);
    Algo::ForEach(TileRows, [this](FTileRow& Row) { Row.Tiles.SetNum(SizeX); });
    OnMapSizeChanged.Broadcast();
}

int32 ATilemap3D::GetSizeX() const {
    return SizeX;
}

int32 ATilemap3D::GetSizeY() const {
    return SizeY;
}

USceneComponent * ATilemap3D::GetTile(int32 X, int32 Y) const {
    check(TileRows.IsValidIndex(Y))
    check(TileRows[Y].Tiles.IsValidIndex(X))
    return TileRows[Y].Tiles[X].GetMeshComponent();
}

FDelegateHandle ATilemap3D::BindToOnMapSizeChange(FSimpleMulticastDelegate::FDelegate &&Delegate) {
    return OnMapSizeChanged.Add(MoveTemp(Delegate));
}

void ATilemap3D::RemoveMapSizeChangedBinding(FDelegateHandle Handle) {
    OnMapSizeChanged.Remove(Handle);
}

FDelegateHandle ATilemap3D::BindToOnTileChange(FOnTileChanged::FDelegate &&Delegate) {
    return OnTileChanged.Add(MoveTemp(Delegate));
}

void ATilemap3D::RemoveTileChangedBinding(FDelegateHandle Handle) {
    OnTileChanged.Remove(Handle);
}

FTileInfo & ATilemap3D::GetAdjustedTilePosition(int32 X, int32 Y) {
    check(TileRows.IsValidIndex(Y))
    check(TileRows[Y].Tiles.IsValidIndex(X))
    return TileRows[Y].Tiles[X];
}