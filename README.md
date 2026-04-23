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
