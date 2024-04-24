#pragma once

#include "LargeReplicatedObject.generated.h"


UCLASS(Blueprintable)
class LARGEOBJECTREPLICATION_API ULargeReplicatedObject : public UObject {
    GENERATED_BODY()

    FTimerHandle TickTimer;
    
    UPROPERTY(Replicated)
    int32 ChunkSize = 1024 * 16;  // Could be 64KB, but allows for smaller Packages

    UPROPERTY(Replicated)
    int32 Size = 0;

public:
    UPROPERTY()
    TArray<uint8> Bytes = {};

    ULargeReplicatedObject();
    
    virtual bool IsSupportedForNetworking() const override {
        return true;
    }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // UFUNCTION(BlueprintCallable)
    // virtual void SetBytes(const TArray<uint8>& Bytes);
    //
    // virtual void SetBytes(const void* Ptr, uint64 Count);
    //
    //
    // UFUNCTION(BlueprintCallable)
    // virtual void AppendBytes(const TArray<uint8>& Bytes);
    //
    // virtual void AppendBytes(const void* Ptr, uint64 Count);
    //
    //
    // UFUNCTION(BlueprintCallable)
    // TArray<uint8> GetBytes() const;

    
    UFUNCTION(BlueprintPure)
    int32 Num() const {
        return Size;
    }

    UFUNCTION(BlueprintPure)
    int32 GetChunkSize() const {
        return ChunkSize;
    }
    
    UFUNCTION(BlueprintCallable)
    void SetChunkSize(const int32 InSize) {
        checkf(InSize > 0, TEXT("ChunkSize must be positive"))
        ChunkSize = InSize;
    }

protected:
    UFUNCTION()
    void Tick();
};
