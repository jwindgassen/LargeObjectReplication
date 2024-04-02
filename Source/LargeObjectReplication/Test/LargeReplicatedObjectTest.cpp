
#if WITH_TESTS

#include "LargeReplicatedObject.h"
#include "Tests/AutomationCommon.h"
#include "Tests/TestHarnessAdapter.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLargeReplicatedObjectTest,
    "LargeObjectReplication.LargeReplicatedObject",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::EngineFilter
);


#define SPAWN_BLOB() \
    ALargeReplicatedObject* Blob = AutomationCommon::GetAnyGameWorld()->SpawnActor<ALargeReplicatedObject>(); \
    Blob->SetChunkSize(16)

bool FLargeReplicatedObjectTest::RunTest(const FString& Parameters) {

    SECTION("Constructors") {
        SPAWN_BLOB();

        CHECK(Blob->Num() == 0);
        CHECK(Blob->GetBytes().Num() == 0);
    }

    SECTION("Set Bytes") {
        SPAWN_BLOB();
        Blob->SetBytes({ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 });

        CHECK(Blob->Num() == 8);
        CHECK(Blob->GetBytes().Num() == 8);
        CHECK(Blob->GetBytes()[6] == 0x06)
    }
    
    return true;
}

#endif
