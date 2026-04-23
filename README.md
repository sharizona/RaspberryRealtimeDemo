# Real-Time Systems Demo - Raspberry Pi

## Overview
This project demonstrates real-time system concepts, scheduling policies, and embedded systems principles that can be deployed on Raspberry Pi or tested locally.

## Learning Objectives

1. **Define Real-Time and Embedded Systems**
2. **Understand RT vs Non-RT Challenges**
3. **Explore RT System Implementations** (Cyclic Executives, RTOS, OS Extensions)
4. **Implement and Test RT Scheduling** (SCHED_OTHER vs SCHED_FIFO)
## What is a Real-Time System?

**Real-Time System**: A system where the correctness depends not only on the logical result of computation but also on the **time** at which results are produced.

- **Hard Real-Time**: Missing a deadline causes system failure (e.g., airbag deployment, pacemaker)
- **Soft Real-Time**: Missing deadlines degrades performance but doesn't cause failure (e.g., video streaming, audio playback)
- **Firm Real-Time**: Missing occasional deadlines is tolerable, but repeated misses are unacceptable (e.g., stock trading systems)

### Key Characteristics:
- **Deterministic**: Predictable response times
- **Time Constraints**: Operations must complete within deadlines
- **Priority-based**: Critical tasks get CPU time first
- **Low Latency**: Minimal delay between event and response

## What is an Embedded System?

**Embedded System**: A computer system with dedicated functions within a larger mechanical or electrical system, typically with real-time computing constraints.

### Characteristics:
- **Purpose-specific**: Designed for specific tasks
- **Resource-constrained**: Limited CPU, memory, power
- **Reliability**: Must operate continuously without failure
- **Often Real-Time**: Many embedded systems have timing requirements

### Overlap:
- Most embedded systems have real-time requirements
- Not all real-time systems are embedded (e.g., stock trading servers)
- Raspberry Pi can function as an embedded system when deployed in dedicated applications

## Real-Time vs Non-Real-Time Systems

| Aspect | Non-Real-Time | Real-Time |
|--------|--------------|-----------|
| **Goal** | Maximize throughput | Meet deadlines |
| **Performance Metric** | Average response time | Worst-case response time |
| **Scheduling** | Fair sharing (time-slicing) | Priority-based, preemptive |
| **Predictability** | Variable, unpredictable | Deterministic, bounded |
| **Latency** | Can be high and variable | Must be low and consistent |
| **Example** | Web server, batch processing | Industrial control, medical devices |

## Challenges of Real-Time Systems

### 1. **Timing Predictability**
   - Must guarantee worst-case execution time (WCET)
   - Difficult with modern CPUs (caches, pipelines, branch prediction)

### 2. **Interrupt Latency**
   - Time from interrupt occurrence to handler execution must be bounded
   - Regular OS may have unbounded interrupt disable periods

### 3. **Priority Inversion**
   - Low-priority task holds resource needed by high-priority task
   - Solved with priority inheritance or priority ceiling protocols

### 4. **Scheduling**
   - Must prove all tasks meet deadlines (schedulability analysis)
   - Rate Monotonic Analysis (RMA), Deadline Monotonic Analysis (DMA)

### 5. **Resource Constraints**
   - Limited memory, CPU power in embedded systems
   - No virtual memory or dynamic allocation in hard real-time

### 6. **Jitter**
   - Variation in task execution timing
   - Must be minimized for control systems

### 7. **Testing and Validation**
   - Must test worst-case scenarios
   - Difficult to reproduce timing-dependent bugs

## Examples of Real-Time System Implementations

### 1. **Cyclic Executive (Time-Triggered)**
Simple, deterministic scheduling using fixed time slots.

**Example Systems:**
- Simple industrial controllers
- Automotive ECUs (Engine Control Units)
- Aircraft flight control systems

**Characteristics:**
- Fixed schedule computed offline
- No OS overhead
- Highly predictable but inflexible
- Tasks execute in predefined sequence

**Pros:** Ultra-low overhead, deterministic
**Cons:** Inflexible, hard to modify, poor CPU utilization

### 2. **Real-Time Operating Systems (RTOS)**
Specialized OS designed for real-time applications.

**Example Systems:**
- **FreeRTOS**: IoT devices, microcontrollers
- **VxWorks**: Mars Rovers, Boeing 787, medical devices
- **QNX**: Automotive infotainment, BlackBerry OS
- **Zephyr**: Industrial IoT, wearables
- **RTLinux/RTAI**: Industrial automation

**Characteristics:**
- Preemptive priority-based scheduling
- Fast context switching
- Minimal interrupt latency
- Deterministic behavior

**Pros:** Flexible, reusable, priority-driven
**Cons:** More overhead than cyclic executive

### 3. **Operating Systems with Real-Time Extensions**
General-purpose OS modified for real-time capabilities.

**Example Systems:**
- **Linux with PREEMPT_RT patch**: Industrial PCs, robotics
- **Windows with RTX**: Manufacturing automation
- **Raspberry Pi OS with RT kernel**: Educational projects, prototypes

**Characteristics:**
- Adds real-time features to standard OS
- Allows running non-RT and RT tasks together
- More features than pure RTOS (networking, filesystems)

**Pros:** Rich feature set, familiar environment
**Cons:** Less deterministic than pure RTOS, higher overhead

**Use Cases:**
- Raspberry Pi projects requiring soft real-time
- Mixed systems (RT control + non-RT UI/networking)
- Development and prototyping

## Key Real-Time Concepts & Terminology

- **Deadline**: Time by which task must complete
- **Period**: Time between successive task activations
- **WCET**: Worst-Case Execution Time
- **Jitter**: Variation in execution timing
- **Latency**: Delay between event and response
- **Preemption**: Ability to interrupt lower-priority task
- **Context Switch**: Saving/restoring task state
- **Schedulability**: Whether all tasks can meet deadlines
- **Utilization**: Percentage of CPU time used by tasks
- **Rate Monotonic Scheduling (RMS)**: Static priority by period (shorter period = higher priority)
- **Earliest Deadline First (EDF)**: Dynamic priority by deadline
- **Priority Inversion**: High-priority task blocked by low-priority
- **Priority Inheritance**: Low-priority task temporarily inherits high priority

## Project Structure

| Path | Description |
|------|-------------|
| `examples/01_basic_pthread/` | Best Effort scheduling |
| `examples/02_realtime_pthread/` | SCHED_FIFO example |
| `examples/03_scheduling_comparison/` | Compare policies |
| `examples/04_priority_inversion/` | Demonstrate and solve priority inversion |
| `docs/scheduling_analysis.md` | Theoretical background |
| `docs/ui_setup.md` | Step-by-step UI guide |
| `README.md` | Project documentation |
| `run-docker.sh` | Interactive script to run examples in Docker |
| `Dockerfile` & `docker-compose.yml` | Container configuration for real-time environment |

## Getting Started

### Option 1: Using Docker (Recommended for Full RT Support)

Docker provides a Linux environment with full POSIX real-time scheduling support, even on macOS/Windows. **Compiling and executing happen inside the container to ensure compatibility.**

#### Method A: Interactive Menu Script (Easiest)

```bash
# Run the interactive menu
./run-docker.sh

# Follow the menu:
# 1) Build and start container
# 2) Enter running container (interactive shell)
# 3-6) Run individual examples (automatically compiles and executes inside)
# 7) Stop container
# 8) Clean up
```

#### Method B: Manual Docker Commands

```bash
# Step 1: Build and start the container
docker-compose up -d --build

# Step 2: Run examples directly (automatically compiles inside)
docker-compose exec realtime-demo bash -c "cd examples/01_basic_pthread && make clean && make && ./basic_pthread"

# Step 3: Or enter the container to work manually
docker-compose exec realtime-demo /bin/bash

# Inside container:
cd /app/examples/01_basic_pthread
make clean && make run
```

**Why Docker?**
- ✅ Full Linux kernel with POSIX RT scheduling
- ✅ Works on macOS, Windows, and Linux
- ✅ Privileged mode for SCHED_FIFO support
- ✅ Isolated environment, no system conflicts
- ✅ Easy to reset and reproduce results

### Option 2: Native Linux/Raspberry Pi

```bash
# Install build tools
sudo apt-get install build-essential

# For Raspberry Pi: Install RT kernel (optional for best results)
sudo apt-get install linux-image-rt-arm64

# Build and run
cd examples/01_basic_pthread
make
./basic_pthread  # Example 1 doesn't need sudo

# Real-time examples need sudo
cd examples/02_realtime_pthread
make
sudo ./realtime_pthread
```

### Option 3: macOS/Windows (Limited RT Support)

```bash
# Build examples (only basic_pthread will work correctly)
cd examples/01_basic_pthread
make
./basic_pthread

# Note: SCHED_FIFO examples won't work on macOS
# Use Docker for full functionality
```

## Testing Environments Comparison

| Environment | SCHED_OTHER | SCHED_FIFO | Priority Inheritance | Recommended For |
|-------------|-------------|------------|---------------------|-----------------|
| **Docker** | ✅ Full | ✅ Full | ✅ Full | Development & Learning |
| **Raspberry Pi** | ✅ Full | ✅ Full | ✅ Full | Deployment & Hardware |
| **Linux Native** | ✅ Full | ✅ Full | ✅ Full | Development |
| **macOS** | ✅ Basic | ❌ Limited | ❌ No | Code Testing Only |
| **Windows** | ⚠️ Basic | ❌ No | ❌ No | Use Docker |

## Next Steps

1. Study the example code in `examples/`
2. Run benchmarks comparing scheduling policies
3. Follow the [UI Visualization Setup Guide](docs/ui_setup.md) to run the Unreal Engine demo
4. Read `docs/scheduling_analysis.md` for in-depth theory
