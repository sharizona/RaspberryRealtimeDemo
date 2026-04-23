#include "RealTimeVisualizer.h"
#include "Common/UdpSocketBuilder.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

URealTimeVisualizer::URealTimeVisualizer()
{
    PrimaryComponentTick.bCanEverTick = true;
    ListenPort = 8888;
    ListenSocket = nullptr;
    UDPReceiver = nullptr;
}

void URealTimeVisualizer::BeginPlay()
{
    Super::BeginPlay();

    FIPv4Endpoint Endpoint(FIPv4Address::Any, ListenPort);
    
    ListenSocket = FUdpSocketBuilder(TEXT("TelemetryListenSocket"))
        .AsNonBlocking()
        .BoundToEndpoint(Endpoint)
        .Build();

    if (ListenSocket)
    {
        UDPReceiver = new FUdpSocketReceiver(ListenSocket, FTimespan::FromMilliseconds(100), TEXT("TelemetryUDPReceiver"));
        UDPReceiver->OnDataReceived().BindUObject(this, &URealTimeVisualizer::OnDataReceived);
        UDPReceiver->Start();
        
        UE_LOG(LogTemp, Log, TEXT("RealTimeVisualizer: Listening on port %d"), ListenPort);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RealTimeVisualizer: Failed to create socket on port %d"), ListenPort);
    }
}

void URealTimeVisualizer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UDPReceiver)
    {
        UDPReceiver->Stop();
        delete UDPReceiver;
        UDPReceiver = nullptr;
    }

    if (ListenSocket)
    {
        ListenSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
        ListenSocket = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void URealTimeVisualizer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URealTimeVisualizer::OnDataReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Endpoint)
{
    FString ReceivedData = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(Data->GetData())));
    
    // Minimal JSON parsing (UE style)
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ReceivedData);

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        FString Policy = JsonObject->GetStringField(TEXT("policy"));
        CurrentPolicy = Policy;

        const TArray<TSharedPtr<FJsonValue>>* ThreadsArray;
        if (JsonObject->TryGetArrayField(TEXT("threads"), ThreadsArray))
        {
            TArray<FThreadTelemetry> NewTelemetry;
            for (const auto& ThreadValue : *ThreadsArray)
            {
                TSharedPtr<FJsonObject> ThreadObj = ThreadValue->AsObject();
                if (ThreadObj.IsValid())
                {
                    FThreadTelemetry Entry;
                    Entry.ThreadId = ThreadObj->GetIntegerField(TEXT("id"));
                    Entry.Priority = ThreadObj->GetIntegerField(TEXT("priority"));
                    Entry.WorkCount = ThreadObj->GetNumberField(TEXT("work_count"));
                    Entry.ElapsedTimeMs = ThreadObj->GetNumberField(TEXT("elapsed_ms"));
                    Entry.WorkRate = ThreadObj->GetNumberField(TEXT("work_rate"));
                    NewTelemetry.Add(Entry);
                }
            }
            
            // Update on Game Thread
            AsyncTask(ENamedThreads::GameThread, [this, NewTelemetry]()
            {
                LatestTelemetry = NewTelemetry;
                OnTelemetryReceived.Broadcast();
            });
        }
    }
}
