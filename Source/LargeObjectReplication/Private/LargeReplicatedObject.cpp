#include "LargeReplicatedObject.h"
#include "LargeObjectReplication.h"
#include "Engine/PackageMapClient.h"


bool FLargeReplicatedObject::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) {
    // On the Server, we want to first Check if we need to Send another Chunk. Otherwise we won't do anything
    // On the Client, we want to first read and then write into our data. That's why we diverge here.
    return Ar.IsSaving() ? SerializeOnServer(Ar, Map, bOutSuccess) : DeserializeOnClient(Ar, Map, bOutSuccess);
}

bool FLargeReplicatedObject::SerializeOnServer(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) {
    bOutSuccess = true;

    // What's the next Chunk we need to send to this Client
    const FUniqueNetIdRepl Receiver = IdentifyReceiver(Map);
    uint32 ChunkStart = SyncStatus.FindOrAdd(Receiver, 0);
    UE_LOG(LogLargeObjectReplication, Log, TEXT("Serializing Data[%d:] for Client '%s' on Server: "), ChunkStart,
           *Receiver->GetHexEncodedString())

    if (ChunkStart == Data.Num()) {
        // We are fully mapped, no need to send anything anymore
        UE_LOG(LogLargeObjectReplication, Log, TEXT("  There are no more Chunks to Serialize"))
        return true;
    }

    // Get the Subsection for this Send
    const uint32 ChunkLength = FPlatformMath::Min(ChunkSize, Data.Num() - ChunkStart);
    uint32 ArraySize = Data.Num();
    TArray<uint8> Chunk(&Data[ChunkStart], ChunkLength);
    UE_LOG(LogLargeObjectReplication, Log, TEXT("  Data[%d:%d] (%d of %d Bytes)"), ChunkStart, ChunkStart + ChunkLength,
           ChunkLength, ArraySize)

    // Send the next Chunk next time
    SyncStatus.FindChecked(Receiver) += ChunkLength;

    // Write ArraySize and ChunkStart
    Ar.SerializeBits(&ArraySize, FMath::CeilLogTwo(MaxSize) + 1);
    Ar.SerializeBits(&ChunkStart, FMath::CeilLogTwo(MaxSize) + 1);

    // Add Chunk to the Array (Adds ChunkLength and Chunk itself)
    // ToDo: Compression
    Chunk.BulkSerialize(Ar);

    RepKey++;
    return false;
}

bool FLargeReplicatedObject::DeserializeOnClient(FArchive& Ar, UPackageMap*, bool& bOutSuccess) {
    UE_LOG(LogLargeObjectReplication, Log, TEXT("Deserializing received Chunk on Client:"))
    bOutSuccess = true;

    // If there are at less than 2 Sizes serialized, then there is no data
    if (static_cast<FBitReader&>(Ar).GetBytesLeft() < static_cast<int64>(2 * FMath::CeilLogTwo(MaxSize))) {
        UE_LOG(LogLargeObjectReplication, Log, TEXT("  There is nothing to Deserialize"))
        return true;
    }

    // Get ArraySize
    uint32 ArraySize = 0;
    Ar.SerializeBits(&ArraySize, FMath::CeilLogTwo(MaxSize) + 1);

    // Get ChunkStart
    uint32 ChunkStart = 0;
    Ar.SerializeBits(&ChunkStart, FMath::CeilLogTwo(MaxSize) + 1);

    // Get Chunk
    TArray<uint8> Chunk;
    Chunk.BulkSerialize(Ar);

    UE_LOG(LogLargeObjectReplication, Log, TEXT("  Data[%d:%d] (%d of %d Bytes)"), ChunkStart, ChunkStart + Chunk.Num(),
           Chunk.Num(), ArraySize)

    // If the Array sizes are not the same, we are receiving new data (first or replaced) and need to reset the array
    if (Data.Num() != ArraySize) {
        UE_LOG(LogLargeObjectReplication, Log, TEXT("Resizing Data Array to %d"), ArraySize)
        Data.Reset(ArraySize);
    }

    // Write read Data to out Data Array
    FPlatformMemory::Memmove(&Data[ChunkStart], Chunk.GetData(), Chunk.Num());

    return false;
}

void FLargeReplicatedObject::Reset() {
    for (auto& Pair : SyncStatus) {
        Pair.Value = 0;
    }
}


FUniqueNetIdRepl FLargeReplicatedObject::IdentifyReceiver(UPackageMap* Map) {
    UPackageMapClient* MapClient = Cast<UPackageMapClient>(Map);
    check(MapClient != nullptr);

    return MapClient->GetConnection()->PlayerId;
}
