#pragma once

#include "LargeReplicatedObject.generated.h"


class ULargeReplicatedActorComponent;


/**
 * This is the heart of the Plugin. This Struct contains the Array, that stores the data that will be replicated from
 * Server to Client. To ensure proper replication, we customize how this Struct is Serialize via the NetSerialize
 * function. This customized behaviour will slice up the data into manageable Chunks and then send them one after
 * another, circumventing the internal maximum Replication size.
 *
 * To enable this component, make sure the Property is set to "Replicated" and register it in
 * "GetLifetimeReplicatedProps". When you replace or append new data, you must call Reset to update the synchronization.
 *
 * ToDo: Compress Chunk Data
 * ToDo: Add more API (AppendBytes, ClearBytes, etc.)
 * ToDo: Delay when the Replication is started?
 * ToDo: Create a way to get the current Replication status (for Progress Bar)
 */
USTRUCT()
struct FLargeReplicatedObject {
    GENERATED_BODY()

    static inline constexpr uint32 MaxSize = 1024 * 1024 * 1024;  // 1GB of Data

    static inline constexpr uint32 ChunkSize = 16 * 1024;  // 16KB per Chunk

    UPROPERTY()
    TArray<uint8> Data;

    /**
     * Serialize the Data in Chunks. We need the PackageMap to identify, to which Client this Bunch is send and then
     * serialize the next Chunk of Bytes for them.
     *
     * @param Ar FArchive to read or write from.
     * @param Map PackageMap used to resolve references to UObject*
     * @param bOutSuccess return value to signify if the serialization was successful
     *
     * @return This doesn't do anything, despite what the Docs say >:(
     */
    bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

    void Reset();

private:
    /**
     * This is a small dummy variable, that we need so Unreal will properly Serialize this. By default, Unreal will
     * only call NetSerialize once and then never again, unless the Struct changes. So we add this small dummy byte and
     * increment it when we need to Serialize more. Unreal will detect, that a Property of the Struct changed and call
     * NetSerialize again.
     */
    UPROPERTY()
    uint8 RepKey = 0;

    bool SerializeOnServer(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

    bool DeserializeOnClient(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

    // Stores which Clients have received how many and which needs to be send next. Server Only!
    TMap<FUniqueNetIdRepl, uint32> SyncStatus;

    // Identified for which Client we are Serializing Data via the given UPackageMap. Server Only!
    static FUniqueNetIdRepl IdentifyReceiver(UPackageMap* Map);
};


template<>
struct TStructOpsTypeTraits<FLargeReplicatedObject> : public TStructOpsTypeTraitsBase2<FLargeReplicatedObject> {
    enum {
        WithNetSerializer = true,
    };
};
