#pragma once
#include "Net/UnrealNetwork.h"

#include "ChunkData.generated.h"


UCLASS()
class LARGEOBJECTREPLICATION_API UReplicatedChunkData : public UObject{
    GENERATED_BODY()

public:
    UPROPERTY(Replicated)
    TArray<uint8> Data;

    virtual bool IsSupportedForNetworking() const override {
        return true;
    }
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        
        DOREPLIFETIME(UReplicatedChunkData, Data);
    }
};
