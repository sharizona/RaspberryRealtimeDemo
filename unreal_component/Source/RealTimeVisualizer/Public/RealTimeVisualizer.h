#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Networking.h"
#include "RealTimeVisualizer.generated.h"

/**
 * Data structure representing telemetry from a single thread
 */
USTRUCT(BlueprintType)
struct FThreadTelemetry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "RealTime")
    int32 ThreadId;

    UPROPERTY(BlueprintReadOnly, Category = "RealTime")
    int32 Priority;

    UPROPERTY(BlueprintReadOnly, Category = "RealTime")
    int64 WorkCount;

    UPROPERTY(BlueprintReadOnly, Category = "RealTime")
    float ElapsedTimeMs;

    UPROPERTY(BlueprintReadOnly, Category = "RealTime")
    float WorkRate;

    FThreadTelemetry() : ThreadId(0), Priority(0), WorkCount(0), ElapsedTimeMs(0.0f), WorkRate(0.0f) {}
};

/**
 * Unreal Engine Component to visualize real-time scheduling data from the C examples.
 * It listens on a UDP port for telemetry data.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALTIMEVISUALIZER_API URealTimeVisualizer : public UActorComponent
{
    GENERATED_BODY()

public:    
    URealTimeVisualizer();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Port to listen for UDP telemetry */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RealTime|Settings")
    int32 ListenPort;

    /** Latest telemetry data received from the system */
    UPROPERTY(BlueprintReadOnly, Category = "RealTime|Data")
    TArray<FThreadTelemetry> LatestTelemetry;

    /** Current scheduling policy being visualized */
    UPROPERTY(BlueprintReadOnly, Category = "RealTime|Data")
    FString CurrentPolicy;

    /** Broadcasts when new data is received */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTelemetryReceived);
    UPROPERTY(BlueprintAssignable, Category = "RealTime|Events")
    FOnTelemetryReceived OnTelemetryReceived;

private:
    FSocket* ListenSocket;
    FUdpSocketReceiver* UDPReceiver;

    /** Callback for when data arrives on the UDP socket */
    void OnDataReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Endpoint);
};
