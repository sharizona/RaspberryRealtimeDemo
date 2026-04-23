# UI Visualization Setup Guide

This guide explains how to run the Unreal Engine UI part to visualize the real-time scheduling data from the C examples.

## 1. Unreal Engine Project Setup

### Prerequisites
- Unreal Engine 4.26 or newer (UE5 recommended).
- A C++ Unreal Engine project (required for the C++ component).

### Installation Steps
1. **Copy Source Files**: 
   Copy the contents of `unreal_component/Source/RealTimeVisualizer/` into your Unreal project's `Source/[YourProjectName]/` directory.
   - `Public/RealTimeVisualizer.h`
   - `Private/RealTimeVisualizer.cpp`

2. **Update Module Dependencies**:
   Open your project's `.Build.cs` file (e.g., `MyProject.Build.cs`) and add the necessary modules to `PublicDependencyModuleNames`:
   ```csharp
   PublicDependencyModuleNames.AddRange(new string[] { 
       "Core", "CoreUObject", "Engine", "InputCore", 
       "Networking", "Sockets", "Json", "JsonUtilities" 
   });
   ```

3. **Compile**:
   Compile your Unreal Engine project in your IDE (Visual Studio, Rider, or XCode) or by clicking **Compile** in the Unreal Editor.

## 2. Setting Up the Visualization in Unreal

1. **Add the Component**:
   - Create or open an Actor Blueprint (e.g., `BP_RealTimeManager`).
   - Click **+ Add Component** and search for **Real Time Visualizer**.
   - Select it and look at the **Details** panel. The default **Listen Port** is `8888`.

2. **Create the UI**:
   - Create a **Widget Blueprint** to display the thread data.
   - In your Actor Blueprint, use the `OnTelemetryReceived` event from the `RealTimeVisualizer` component.
   - Loop through the `LatestTelemetry` array to update your UI elements (text blocks, progress bars, or charts).

3. **Add to Scene**:
   - Drag your `BP_RealTimeManager` Actor into the level.

## 3. Connecting Docker to Unreal Engine

When running the C examples inside Docker, the default `127.0.0.1` refers to the container itself. To reach the Unreal Engine editor running on your host machine, you need to adjust the telemetry settings.

### Mac/Windows (Docker Desktop)
1. Open `examples/telemetry.h`.
2. Change `VISUALIZER_IP` from `"127.0.0.1"` to `"host.docker.internal"`.
   ```c)
   #define VISUALIZER_IP "host.docker.internal"
   ```
3. Re-run the examples inside Docker (they will re-compile automatically).

### Linux (Native Docker)
1. Use your host's local IP address (e.g., `192.168.x.x`).
2. Update `examples/telemetry.h` with this IP.

## 4. Running the Demo

1. **Play in Unreal**:
   Click **Play** in the Unreal Editor. Check the `Output Log` to confirm the component is listening:
   `LogTemp: RealTimeVisualizer: Listening on port 8888`

2. **Run C Examples**:
   Execute the examples via Docker:
   ```bash
   ./run-docker.sh
   # Select an example (3-6)
   ```

3. **Observe**:
   As the C example completes its run (e.g., after 5 seconds), it sends the results via UDP. Your Unreal UI should update immediately with the latest work counts, priorities, and scheduling policy.
