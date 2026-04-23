# Unreal Engine Real-Time Visualizer Component

This folder contains an Unreal Engine Actor Component (`RealTimeVisualizer`) that can be used to visualize the telemetry data from the C examples in this project.

## Features

- **UDP Listener**: Listens for telemetry data on a configurable UDP port (default: 8888).
- **JSON Parsing**: Automatically parses the telemetry data into a structured format.
- **Blueprint Support**: Exposes properties and events to Blueprints for easy visualization setup.

## How to use in Unreal Engine

### 1. Add Source Files
Copy the `Source` folder contents into your Unreal Engine project's `Source` directory.
- `Source/RealTimeVisualizer/Public/RealTimeVisualizer.h`
- `Source/RealTimeVisualizer/Private/RealTimeVisualizer.cpp`

### 2. Update Module Dependencies
In your project's `.Build.cs` file, add the following modules to `PublicDependencyModuleNames`:
```csharp
PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Networking", "Sockets", "Json", "JsonUtilities" });
```

### 3. Add Component to Actor
- Open an Actor Blueprint in the Unreal Editor.
- Click **Add Component** and search for **Real Time Visualizer**.
- Select the component and configure the **Listen Port** in the Details panel.

### 4. Visualize Data
Use the `OnTelemetryReceived` event in the Blueprint Event Graph to update your UI or 3D visualizations whenever new data arrives.

## Telemetry Format

The component expects UDP packets containing JSON in the following format:

```json
{
  "policy": "SCHED_FIFO",
  "threads": [
    {
      "id": 1,
      "priority": 80,
      "work_count": 1234567,
      "elapsed_ms": 5000.0,
      "work_rate": 246.91
    },
    ...
  ]
}
```

## Running with Examples

For detailed setup instructions, please refer to the [UI Visualization Setup Guide](../docs/ui_setup.md).

1. Start your Unreal Engine project and ensure the Actor with the `RealTimeVisualizer` component is in the scene.
2. Run the C examples (e.g., via Docker or natively).
3. The examples will send UDP packets to `127.0.0.1:8888`, which the Unreal component will receive and process.
