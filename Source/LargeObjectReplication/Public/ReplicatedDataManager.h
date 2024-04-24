// Copyright (c) 2021-2022, Forschungszentrum Jülich GmbH. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ReplicatedDataManager.generated.h"


USTRUCT(BlueprintType)
struct FReplicatedDataItem {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    FString Identifier;
    
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    TArray<uint8> Data;
};


/**
 * The Data Manager is the central data storage for any data that gets Replicated. For now, it will primarily be used on
 * the Server, where other Actors on the Server can add some Data via a Key-Value pair. Clients can then query some of
 * these BLOBs by using the associated key(s). The Data is then copied from server to the client using RPC, which will
 * split the BLOB up into multiple small-enough chunks and reassemble them on the client.
 *
 * ToDo: Allow Adding BLOBs on Clients and push them to the Server
 */
UCLASS()
class LARGEOBJECTREPLICATION_API AReplicatedDataManager : public AInfo {
    GENERATED_BODY()

    // Contains the associated Identifiers of available data.
    UPROPERTY(Replicated)
    TArray<FString> Identifiers;

    // Contains the sizes of the available data BLOBs.
    UPROPERTY(Replicated)
    TArray<int32> ItemSizes;

    // Size of the replicated chunks
    UPROPERTY(Replicated)
    int32 ChunkSize = 16 * 1024;  // 16 KB

    /**
     * Contains the available Items. This list is designated to be out-of-sync: When a client queries some data,
     * it will be added to this List on the client. The Server's list will be the only one which contains all BLOBs
     */
    UPROPERTY()
    TArray<FReplicatedDataItem> Items = {};
    
public:
    AReplicatedDataManager();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    void RegisterItem(const FString Identifier, const TArray<uint8>& Data);

    UFUNCTION(BlueprintCallable)
    void QueryItem(const FString& Identifier);
    
    UFUNCTION(BlueprintPure)
    int32 Num() const {
        
        
        return Identifiers.Num();
    }

    UFUNCTION(BlueprintPure)
    TArray<FString> GetIdentifiers() const {
        return Identifiers;
    }

    UFUNCTION(BlueprintPure)
    int32 GetItemSize(const FString& Identifier) const {
        return ItemSizes[GetIndexByIdentifier(Identifier)];
    }

    UFUNCTION(BlueprintPure)
    FReplicatedDataItem GetItem(const FString& Identifier) const {
        return Items[GetIndexByIdentifier(Identifier)];
    }

    UFUNCTION(BlueprintPure)
    bool IsServer() const {
        return GetWorld()->GetNetMode() != NM_Client;
    }
    
private:
    int32 GetIndexByIdentifier(const FString& Identifier) const {
        const int32 Index = Identifiers.Find(Identifier);
        checkf(Index != INDEX_NONE, TEXT("No Item with '%s' as Identifier"), *Identifier)

        return Index;
    }

public:
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void StartItemQuery(const FString& Identifier);

private:
    UFUNCTION(NetMulticast, Reliable)
    void ReceiveItemQueryChunk(const FString& Identifier, const TArray<uint8>& Chunk, int32 StartIndex);
};
