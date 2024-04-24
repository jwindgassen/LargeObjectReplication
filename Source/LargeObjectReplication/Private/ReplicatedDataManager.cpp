#include "ReplicatedDataManager.h"
#include "Net/UnrealNetwork.h"


constexpr static uint8 FillByte = 0;


AReplicatedDataManager::AReplicatedDataManager() {
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = false;
}

void AReplicatedDataManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReplicatedDataManager, Identifiers);
    DOREPLIFETIME(AReplicatedDataManager, ItemSizes);
}

void AReplicatedDataManager::RegisterItem(const FString Identifier, const TArray<uint8>& Data) {
    Identifiers.Add(Identifier);
    ItemSizes.Add(Data.Num());

    Items.Emplace(Identifier, Data);
}

void AReplicatedDataManager::QueryItem(const FString& Identifier) {
    // We wanna keep the ordering of the Items the same, so first we create some empty items if there arent enough
    const int32 Index = GetIndexByIdentifier(Identifier);
    // if (Index + 1 > Items.Num()) {
    //     Items.SetNumUninitialized(Index + 1);
    // }

    // Initialize the Item
    // Items[Index] = FReplicatedDataItem(Identifier, TArray<uint8>());
    Items.AddDefaulted();
    Items[Index].Data.Reserve(ItemSizes[Index]);

    // Begin Query
    StartItemQuery(Identifier);
}

void AReplicatedDataManager::StartItemQuery_Implementation(const FString& Identifier) {
    const int32 Index = GetIndexByIdentifier(Identifier);
    const TArray<uint8>& Data = GetItem(Identifier).Data;
    
    // Send Item Chunk by Chunk
    int32 StartIndex = 0;
    while (StartIndex < ItemSizes[Index]) {
        const int32 Size = FPlatformMath::Min(Data.Num() - StartIndex, ChunkSize);
        TArray<uint8> Chunk{&Data[StartIndex], Size};
        ReceiveItemQueryChunk(Identifier, Chunk, StartIndex);

        StartIndex += ChunkSize;
    }
}

void AReplicatedDataManager::ReceiveItemQueryChunk_Implementation(
    const FString& Identifier, const TArray<uint8>& Chunk, const int32 StartIndex
) {
    const int32 Index = GetIndexByIdentifier(Identifier);
    FPlatformMemory::Memcpy(&Items[Index].Data[StartIndex], &Chunk[0], Chunk.Num());
}