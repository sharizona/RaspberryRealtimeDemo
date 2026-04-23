# Real-Time Scheduling Analysis

This document provides an in-depth theoretical analysis of real-time scheduling policies, focus on POSIX standards and their implementation in Linux, particularly for embedded systems like the Raspberry Pi.

## 1. POSIX Scheduling Policies

The Linux kernel implements several scheduling policies as defined by POSIX.1-2001.

### 1.1 SCHED_OTHER (Default)
- **Type**: Standard time-sharing scheduler (Completely Fair Scheduler - CFS in modern Linux).
- **Behavior**: Uses dynamic priorities (niceness) and time-slicing to ensure all processes get a fair share of the CPU.
- **Real-Time Suitability**: **Poor**. It prioritizes throughput and fairness over predictability and latency.

### 1.2 SCHED_FIFO (First-In, First-Out)
- **Type**: Fixed-priority real-time scheduling.
- **Behavior**:
    - A thread with SCHED_FIFO runs until it either blocks for I/O, is preempted by a higher-priority thread, or voluntarily yields.
    - There is no time-slicing.
- **Use Case**: Critical tasks that must run to completion or until they decide to wait.

### 1.3 SCHED_RR (Round Robin)
- **Type**: Fixed-priority real-time scheduling with time-slicing.
- **Behavior**: Similar to SCHED_FIFO, but if multiple threads have the same priority, they are scheduled in a round-robin fashion within a fixed time quantum.
- **Use Case**: When multiple real-time tasks of equal importance need to share the CPU.

---

## 2. Schedulability Analysis

Schedulability analysis determines if a set of tasks can meet all their deadlines.

### 2.1 Rate Monotonic Scheduling (RMS)
RMS is a **static-priority** assignment policy where tasks with shorter periods are assigned higher priorities.

- **Assumptions**: 
    - Tasks are periodic.
    - Deadlines equal periods.
    - Tasks are independent.
- **Utilization Bound**: For $n$ tasks, the system is schedulable if:
  $$U = \sum_{i=1}^{n} \frac{C_i}{T_i} \leq n(2^{1/n} - 1)$$
  Where $C_i$ is the execution time and $T_i$ is the period.
  - For large $n$, the bound approaches $\ln(2) \approx 0.693$.

### 2.2 Earliest Deadline First (EDF)
EDF is a **dynamic-priority** policy where the task with the closest deadline gets the highest priority.

- **Utilization Bound**: A task set is schedulable if and only if:
  $$U = \sum_{i=1}^{n} \frac{C_i}{T_i} \leq 1$$
- **Pros**: Can achieve 100% CPU utilization.
- **Cons**: More complex to implement and can lead to "domino effect" failures during overloads.

---

## 3. Priority Inversion and Inheritance

### 3.1 The Problem: Priority Inversion
Priority Inversion occurs when a high-priority task ($T_H$) is blocked by a low-priority task ($T_L$) holding a resource, and a medium-priority task ($T_M$) preempts $T_L$, effectively delaying $T_H$ indefinitely.

**Historical Example**: The Mars Pathfinder mission (1997) experienced frequent system resets due to priority inversion in its VxWorks OS.

### 3.2 The Solution: Priority Inheritance Protocol (PIP)
When $T_H$ blocks on a resource held by $T_L$, $T_L$ temporarily "inherits" the priority of $T_H$. This prevents $T_M$ from preempting $T_L$, allowing $T_L$ to finish and release the resource quickly.

In POSIX, this is enabled using the `PTHREAD_PRIO_INHERIT` attribute on mutexes.

---

## 4. Practical Real-Time Linux (PREEMPT_RT)

Standard Linux is not a "hard" real-time OS because the kernel itself is not fully preemptible. The **PREEMPT_RT** patch set transforms Linux into a real-time OS by:
1. Making almost all kernel code preemptible.
2. Converting spinlocks into sleepable mutexes (with priority inheritance).
3. Moving interrupt handlers into schedulable kernel threads.

### Raspberry Pi Considerations:
- **Jitter**: Caused by CPU frequency scaling (disable for RT), background interrupts (WiFi, Bluetooth), and DMA.
- **Kernel**: Use a kernel compiled with `CONFIG_PREEMPT_RT=y` for best results.
- **Memory Locking**: Use `mlockall()` to prevent page faults, which introduce non-deterministic latencies.

---

## 5. Summary Table

| Feature | SCHED_OTHER | SCHED_FIFO / RR |
|---------|-------------|-----------------|
| **Priority** | Dynamic (Nice) | Fixed (1-99) |
| **Preemption** | Fair | Immediate |
| **Time Slicing** | Yes | No (FIFO) / Yes (RR) |
| **Determinism** | No | Yes |
| **Best For** | Desktop/Server apps | Control loops, RT data |
