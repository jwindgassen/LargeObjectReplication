#pragma once

#include "LargeReplicatedObject.generated.h"


USTRUCT()
struct LARGEOBJECTREPLICATION_API FReplicatedBlobChunkData {
    GENERATED_BODY()

    UPROPERTY()
    TArray<uint8> Data;
};


UCLASS(Blueprintable)
class LARGEOBJECTREPLICATION_API ALargeReplicatedObject : public AInfo {
    GENERATED_BODY()

    UPROPERTY(Replicated)
    int32 ChunkSize = 1024 * 16;  // Could be 64KB, but allows for smaller Packages

    UPROPERTY(Replicated)
    int32 Size = 0;
    
    UPROPERTY(Replicated)
    TArray<FReplicatedBlobChunkData> Chunks = {};

public:
    ALargeReplicatedObject();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


    UFUNCTION(BlueprintCallable)
    virtual void SetBytes(const TArray<uint8>& Bytes);

    virtual void SetBytes(const void* Ptr, uint64 Count);

    
    UFUNCTION(BlueprintCallable)
    virtual void AppendBytes(const TArray<uint8>& Bytes);

    virtual void AppendBytes(const void* Ptr, uint64 Count);

    
    UFUNCTION(BlueprintCallable)
    TArray<uint8> GetBytes() const;

    
    UFUNCTION(BlueprintPure)
    int32 Num() const {
        return Size;
    }

    UFUNCTION(BlueprintCallable)
    void SetChunkSize(const int32 InSize) {
        ChunkSize = ensure(InSize > 0);
    }
};
