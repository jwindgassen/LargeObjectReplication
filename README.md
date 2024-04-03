# LargeObjectReplication
A small Unreal Engine Library to Replicate large Arrays / binary Data

## Usage
### Declare Replicated Property:

```c++
UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
ULargeReplicatedObject* SomeData;

virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABlobReplicationCharacter, SomeData);
}

virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override {
    return Super::ReplicateSubobjects(Channel, Bunch, RepFlags)
        | Channel->ReplicateSubobject(SomeData, *Bunch, *RepFlags);
}
```

### Add some Bytes:
```c++
TArray<int32> Data = {...};
SomeData->SetBytes(Data.GetData(), Data.Num() * sizeof(int32));
```

### Retrieve Bytes
```c++
TArray<uint8> Bytes = SomeData->GetBytes();
```