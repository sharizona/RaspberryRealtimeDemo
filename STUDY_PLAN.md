# Real-Time Systems Study Plan

## Overview
This study plan guides you through understanding real-time systems concepts using hands-on examples. Complete the modules in order, as each builds on previous knowledge.

**Total Time Estimate**: 6-8 hours
**Prerequisites**: Basic C programming, Linux command line
**Environment**: Docker container on Mac M4

---

## Module 1: Foundations (1 hour)

### Learning Objectives
- [ ] Define real-time systems vs embedded systems
- [ ] Understand hard, soft, and firm real-time constraints
- [ ] Identify key RT challenges (timing, latency, jitter, priority inversion)
- [ ] Enumerate RT system implementations (Cyclic Executive, RTOS, OS Extensions)

### Tasks
1. **Read README.md sections** (20 min)
   - What is a Real-Time System?
   - What is an Embedded System?
   - Real-Time vs Non-Real-Time Systems
   - Challenges of Real-Time Systems
   - Examples of Real-Time System Implementations

2. **Reflect on Real-World Examples** (10 min)
   - Find 3 examples of hard real-time systems in your environment
   - Find 3 examples of soft real-time systems
   - Why is each classified as hard vs soft?

3. **Key Terminology Quiz** (30 min)
   - Define: Deadline, Period, WCET, Jitter, Latency
   - Define: Preemption, Context Switch, Schedulability
   - Define: Priority Inversion, Priority Inheritance
   - What is Rate Monotonic Scheduling?
   - What is Earliest Deadline First?

### Resources
- README.md "Key Real-Time Concepts & Terminology" section
- Mars Pathfinder incident (Google: "Mars Pathfinder priority inversion")

---

## Module 2: Best-Effort Scheduling (1.5 hours)

### Learning Objectives
- [ ] Understand SCHED_OTHER (CFS) scheduling policy
- [ ] Observe fair CPU time distribution
- [ ] Analyze non-deterministic timing behavior
- [ ] Measure work distribution variance

### Tasks

#### 2.1 Code Reading (30 min)
```bash
# Read the source code
cat examples/01_basic_pthread/basic_pthread.c
```

**Questions to answer:**
- What scheduling policy is used?
- How many threads are created?
- What does `do_work()` simulate?
- How is "fairness" measured in the results?
- What is the coefficient of variation telling us?

#### 2.2 Run the Example (20 min)
```bash
# Inside Docker container
cd /app/examples/01_basic_pthread
make clean && make
./basic_pthread
```

**Observe and record:**
- Work counts for each thread
- Are they similar or different?
- What's the coefficient of variation percentage?
- Run it 3 times - do results change significantly?

#### 2.3 Modify and Experiment (40 min)

**Experiment 1: More threads**
- Change `NUM_THREADS` from 3 to 8
- Rebuild and run
- Does fairness change?

**Experiment 2: Different work loads**
- Change `WORK_ITERATIONS` from 1000000 to 5000000
- Does this affect fairness?

**Experiment 3: Longer duration**
- Change `RUN_DURATION_SEC` from 5 to 10
- Does longer runtime improve fairness?

**Write a summary:**
- What did you learn about SCHED_OTHER?
- When would you use this scheduling policy?
- What are its limitations?

---

## Module 3: Real-Time Scheduling (2 hours)

### Learning Objectives
- [ ] Understand SCHED_FIFO strict priority scheduling
- [ ] Observe priority-based preemption
- [ ] Identify starvation issues
- [ ] Compare deterministic vs non-deterministic behavior

### Tasks

#### 3.1 Code Reading (40 min)
```bash
cat examples/02_realtime_pthread/realtime_pthread.c
```

**Questions to answer:**
- What are the three priority levels used?
- Why does this need `sudo`/privileged mode?
- What is `mlockall()` and why is it important?
- Why does the code call `sched_yield()`?
- What would happen without `sched_yield()`?
- How is the thread priority set?

#### 3.2 Run the Example (30 min)
```bash
cd /app/examples/02_realtime_pthread
make clean && make
./realtime_pthread
```

**Observe and record:**
- High priority thread work count
- Medium priority thread work count
- Low priority thread work count
- What percentage of total work did each do?
- Is this fair? Is it supposed to be fair?

#### 3.3 Modify and Experiment (50 min)

**Experiment 1: Remove sched_yield()**
- Comment out the `sched_yield()` calls (around line 70)
- Rebuild and run
- What happens to low-priority thread?
- This demonstrates **starvation**!

**Experiment 2: Change priorities**
- Change all priorities to be equal (e.g., all 50)
- What happens to work distribution?
- How is this different from SCHED_OTHER?

**Experiment 3: Extreme priorities**
- Set: HIGH=99, MED=50, LOW=1
- How does this affect results?

**Experiment 4: More threads**
- Add a 4th thread with priority 60
- Where does it fit in the execution order?

**Write a summary:**
- When would you use SCHED_FIFO?
- What are the risks?
- Why is priority design critical?

---

## Module 4: Scheduling Comparison (1.5 hours)

### Learning Objectives
- [ ] Measure latency and jitter differences
- [ ] Understand worst-case execution time importance
- [ ] Compare priority enforcement between policies
- [ ] Analyze latency distributions

### Tasks

#### 4.1 Code Reading (40 min)
```bash
cat examples/03_scheduling_comparison/sched_comparison.c
```

**Questions to answer:**
- What two tests are performed?
- How is latency measured?
- What is jitter and how is it calculated?
- What does the latency distribution show?
- Why are there two priority tests?

#### 4.2 Run the Example (30 min)
```bash
cd /app/examples/03_scheduling_comparison
make clean && make
./sched_comparison
```

**Record Results:**

| Metric | SCHED_OTHER | SCHED_FIFO |
|--------|-------------|------------|
| Average Latency | | |
| Min Latency | | |
| Max Latency (WCET) | | |
| Jitter (Std Dev) | | |
| High Priority % | | |
| Med Priority % | | |
| Low Priority % | | |

#### 4.3 Analysis (20 min)

**Questions:**
- Which policy has lower maximum latency?
- Which policy has lower jitter?
- For a hard real-time system, which matters more: average or max latency?
- Why is SCHED_FIFO more predictable?
- What's the trade-off for this predictability?

**Write a summary:**
- When would low jitter be critical? (Think: audio, control systems)
- What applications can tolerate high jitter?
- Design a system that needs SCHED_FIFO and explain why

---

## Module 5: Priority Inversion (2 hours)

### Learning Objectives
- [ ] Understand the priority inversion problem
- [ ] See Mars Pathfinder bug recreation
- [ ] Learn priority inheritance protocol
- [ ] Solve priority inversion with PTHREAD_PRIO_INHERIT

### Tasks

#### 5.1 Background Research (30 min)

**Read about Mars Pathfinder:**
- Google: "Mars Pathfinder priority inversion 1997"
- Read about the incident
- Understand what went wrong
- How was it fixed remotely?

**Questions:**
- What were the three task priorities?
- Which task held the mutex?
- Which task was starved?
- What was the consequence?

#### 5.2 Code Reading (40 min)
```bash
cat examples/04_priority_inversion/priority_inversion.c
```

**Questions to answer:**
- What are the three thread priorities?
- What resource do they compete for?
- Which thread needs the resource?
- Which thread holds the resource?
- Which thread causes the problem?
- How does `PTHREAD_PRIO_INHERIT` solve this?
- What happens to LOW priority when HIGH blocks on its mutex?

#### 5.3 Run Both Scenarios (30 min)

```bash
cd /app/examples/04_priority_inversion
make clean && make
./priority_inversion
```

**Observe Scenario 1 (without priority inheritance):**
- When does HIGH thread try to acquire mutex?
- How long does HIGH wait?
- What is MED thread doing during this time?
- Is priority inversion detected?

**Observe Scenario 2 (with priority inheritance):**
- When does LOW inherit HIGH priority?
- Can MED preempt LOW now?
- How long does HIGH wait this time?
- Compare wait times between scenarios

#### 5.4 Experiments (20 min)

**Experiment 1: Remove MED thread**
- Comment out medium thread creation
- Does priority inversion still occur?
- Why or why not?

**Experiment 2: Change work amounts**
- Increase `WORK_ITERATIONS` for MED thread
- Does the inversion problem get worse?

**Write a summary:**
- Explain priority inversion in your own words
- Why is it dangerous in real-time systems?
- Name two solutions besides priority inheritance
- When would you use priority ceiling instead?

---

## Module 6: Integration & Design (1-2 hours)

### Learning Objectives
- [ ] Design a complete real-time system
- [ ] Apply scheduling theory
- [ ] Make architecture decisions
- [ ] Document design rationale

### Project: Design a Real-Time System

Pick one scenario and design a complete solution:

#### Scenario A: Industrial Robot Controller
**Requirements:**
- Safety monitoring: Check every 10ms (hard deadline)
- Motor control: Update every 20ms (hard deadline)
- Vision processing: 30 FPS (soft deadline)
- Network logging: Best effort

**Design Questions:**
- Which scheduling policy for each task?
- What priorities? (use Rate Monotonic Scheduling)
- Are there shared resources? How to protect?
- What's the CPU utilization?
- Is the system schedulable?

#### Scenario B: Medical Device (Patient Monitor)
**Requirements:**
- Heart rate sensor: Sample every 100ms (hard deadline)
- Alarm system: Respond within 50ms (hard deadline)
- Display update: 10 FPS (soft deadline)
- Data logging: Best effort

**Design Questions:**
- Task priorities?
- Scheduling policy?
- How to handle priority inversion on shared data?
- What if alarm and sensor conflict?

#### Scenario C: Autonomous Drone
**Requirements:**
- IMU reading: 100Hz (hard deadline)
- Stabilization control: 50Hz (hard deadline)
- Obstacle detection: 20Hz (firm deadline)
- GPS logging: 1Hz (soft deadline)

**Design Questions:**
- Calculate periods and priorities
- Which tasks can miss deadlines?
- Schedulability analysis?
- Resource protection strategy?

### Deliverable
Write a 1-2 page design document including:
1. Task list with periods and deadlines
2. Priority assignment (justify using RMS or other)
3. Scheduling policy choice
4. Resource sharing strategy
5. Schedulability analysis (utilization calculation)
6. Failure mode analysis

---

## Module 7: Advanced Topics (Optional, 2-3 hours)

### 7.1 Rate Monotonic Analysis
- [ ] Learn RMS theorem (utilization bound)
- [ ] Calculate schedulability for your Module 6 design
- [ ] Understand why shorter period = higher priority

**Resources:**
- Research: "Rate Monotonic Scheduling" Liu & Layland 1973
- Calculate: U = Σ(Ci/Ti) ≤ n(2^(1/n) - 1)

### 7.2 Earliest Deadline First
- [ ] Understand dynamic priority scheduling
- [ ] Compare EDF vs RMS
- [ ] When is EDF optimal?

### 7.3 Real-Time Linux Patch
- [ ] Research PREEMPT_RT patch
- [ ] Understand how it improves Linux RT performance
- [ ] Compare to pure RTOS like FreeRTOS

### 7.4 Raspberry Pi Deployment
- [ ] Install Raspberry Pi OS
- [ ] Compile examples on Pi
- [ ] Optional: Install RT kernel patch
- [ ] Measure actual latencies on hardware

---

## Assessment Checklist

### Concepts Mastery
- [ ] Can explain hard vs soft real-time with examples
- [ ] Can describe SCHED_OTHER vs SCHED_FIFO differences
- [ ] Understand priority-based preemptive scheduling
- [ ] Can identify and solve priority inversion
- [ ] Know when to use each scheduling policy

### Practical Skills
- [ ] Can write pthread code with SCHED_OTHER
- [ ] Can write pthread code with SCHED_FIFO
- [ ] Can set thread priorities correctly
- [ ] Can use mutex with priority inheritance
- [ ] Can measure latency and jitter

### Design Skills
- [ ] Can assign priorities using Rate Monotonic
- [ ] Can calculate CPU utilization
- [ ] Can perform schedulability analysis
- [ ] Can choose appropriate scheduling policy
- [ ] Can design resource sharing strategy

### Code Modifications
- [ ] Successfully modified thread count
- [ ] Successfully changed priorities
- [ ] Successfully demonstrated starvation
- [ ] Successfully showed priority inversion
- [ ] Successfully tested priority inheritance

---

## Recommended Study Schedule

### Week 1: Foundation
- **Day 1**: Module 1 (Foundations)
- **Day 2**: Module 2 (Best-Effort Scheduling)
- **Day 3**: Module 3 (Real-Time Scheduling)

### Week 2: Advanced
- **Day 4**: Module 4 (Scheduling Comparison)
- **Day 5**: Module 5 (Priority Inversion)
- **Day 6**: Module 6 (Integration & Design)

### Week 3: Optional
- **Day 7+**: Module 7 (Advanced Topics)

---

## Resources

### Books
- "Real-Time Systems" by Jane W.S. Liu
- "Real-Time Concepts for Embedded Systems" by Qing Li & Caroline Yao
- "Embedded Systems: Real-Time Operating Systems" by Jim Cooling

### Online
- POSIX Real-Time Programming: man pages (`man 7 sched`)
- Linux Kernel Documentation: scheduler
- FreeRTOS documentation
- PREEMPT_RT patch documentation

### Papers
- Liu & Layland (1973): "Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment"
- Mars Pathfinder Priority Inversion Analysis

### Tools
- `chrt` - manipulate real-time attributes
- `cyclictest` - measure RT latencies
- `stress-ng` - stress test RT behavior

---

## Next Steps After Completion

1. **Deploy to Raspberry Pi**
   - Get actual hardware
   - Test on real embedded system
   - Measure real-world latencies

2. **Build a Real Project**
   - Sensor data acquisition system
   - Motor control system
   - Audio processing pipeline

3. **Explore RTOS**
   - Try FreeRTOS on microcontroller
   - Compare to Linux RT
   - Understand trade-offs

4. **Contribute**
   - Add more examples to this repo
   - Document your findings
   - Share with others learning RT systems

---

## Study Tips

- **Hands-on is key**: Don't just read - run, modify, break things!
- **Take notes**: Document your observations
- **Ask "why"**: Understand the reasoning behind design choices
- **Compare**: Always contrast RT vs non-RT behavior
- **Real examples**: Think of systems around you that use these concepts
- **Incremental**: Master each module before moving on
- **Experiment**: The best learning comes from trying things

---

## Getting Help

- Read code comments thoroughly
- Check README.md for detailed explanations
- Google specific error messages
- Review POSIX pthread documentation
- Study the Mars Pathfinder case study
- Trace through code with a debugger

Good luck with your real-time systems journey! 🚀
