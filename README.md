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
