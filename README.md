# LargeObjectReplication
A small Unreal Engine Library to Replicate large Arrays / binary Data

## Usage
### Declare Replicated Property:

```c++
UPROPERTY(Replicated)
FLargeReplicatedObject SomeData;

virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMyActor, SomeData);
}
```
