#include "LargeReplicatedObject.h"
#include "Net/UnrealNetwork.h"


ALargeReplicatedObject::ALargeReplicatedObject() {
    bReplicates = true;
}


void ALargeReplicatedObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALargeReplicatedObject, Chunks);
    DOREPLIFETIME(ALargeReplicatedObject, ChunkSize);
    DOREPLIFETIME(ALargeReplicatedObject, Size);
}


void ALargeReplicatedObject::SetBytes(const TArray<uint8>& Bytes) {
    SetBytes(Bytes.GetData(), Bytes.Num() * sizeof(uint8));
}

void ALargeReplicatedObject::SetBytes(const void* Ptr, uint64 Count) {
    checkf(ChunkSize < 64 * 1024, TEXT("ChunkSize must be smaller that 64kB, is %d"), ChunkSize)
    
    Size = Count;
    Chunks.Reset(FPlatformMath::CeilToInt32(1.0f * Count / ChunkSize));

    const uint8* Src = static_cast<const uint8*>(Ptr);
    while (Count > 0) {
        const int32 BytesInChunk = FPlatformMath::Min(Count, static_cast<uint64>(ChunkSize));

        FReplicatedBlobChunkData Chunk{};
        Chunk.Data = TArray{Src, BytesInChunk};
        Chunks.Push(Chunk);

        Src += BytesInChunk;
        Count -= BytesInChunk;
    }
}


void ALargeReplicatedObject::AppendBytes(const TArray<uint8>& Bytes) {
    AppendBytes(Bytes.GetData(), Bytes.Num() * sizeof(uint8));
}

void ALargeReplicatedObject::AppendBytes(const void* Ptr, uint64 Count) {
    checkf(ChunkSize < 64 * 1024, TEXT("ChunkSize must be smaller that 64kB, is %d"), ChunkSize)
    
    Size += Count;
    Chunks.Reserve(Chunks.Num() + FPlatformMath::CeilToInt32(1.0f * Count / ChunkSize));

    const uint8* Src = static_cast<const uint8*>(Ptr);
    
    // First, try to finish the last chunk
    const int32 RemainingBytes = FPlatformMath::Min(Count, static_cast<uint64>(ChunkSize - Chunks.Last().Data.Num()));
    Chunks.Last().Data.Append(Src, RemainingBytes);
    Src += RemainingBytes;
    Count -= RemainingBytes;

    // Now, start creating new Chunks
    while (Count > 0) {
        const int32 BytesInChunk = FPlatformMath::Min(Count, static_cast<uint64>(ChunkSize));

        FReplicatedBlobChunkData Chunk{};
        Chunk.Data = TArray{Src, BytesInChunk};
        Chunks.Push(Chunk);

        Src += BytesInChunk;
        Count -= BytesInChunk;
    }
}
TArray<uint8> ALargeReplicatedObject::GetBytes() const {
    TArray<uint8> Bytes;
    Bytes.Reserve(Size);

    for (const auto& [Data] : Chunks) {
        Bytes.Append(Data);
    }

    return Bytes;
}
