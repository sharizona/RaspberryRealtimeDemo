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

