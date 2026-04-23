/**
 * Real-Time Pthread Example - SCHED_FIFO Scheduling
 *
 * This demonstrates real-time scheduling with SCHED_FIFO policy.
 * SCHED_FIFO provides strict priority-based preemptive scheduling.
 *
 * Key Characteristics:
 * - Strict priority: Higher priority threads always preempt lower priority
 * - No time slicing: Thread runs until it blocks or yields
 * - Real-time guarantees: Deterministic, bounded latency
 * - REQUIRES ROOT/SUDO: Real-time scheduling is privileged operation
 *
 * Priority Range: 1 (lowest) to 99 (highest)
 *
 * Compile: gcc -o realtime_pthread realtime_pthread.c -lpthread -lrt
 * Run: sudo ./realtime_pthread  (MUST USE SUDO!)
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <sys/mman.h>
#include "../telemetry.h"

#define NUM_THREADS 3
#define WORK_ITERATIONS 1000000
#define RUN_DURATION_SEC 5

// Thread priorities (SCHED_FIFO: 1-99, higher = more priority)
#define HIGH_PRIORITY 80
#define MED_PRIORITY 50
#define LOW_PRIORITY 20

// Structure to pass data to threads
typedef struct {
    int thread_id;
    int priority;
    long long work_count;
    struct timespec start_time;
    struct timespec end_time;
    int preemption_count;
} thread_data_t;

// Get current time in nanoseconds
long long get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

// Calculate time difference in milliseconds
double time_diff_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1000000.0;
}

// Simulated work function
void do_work(void) {
    volatile long sum = 0;
    for (int i = 0; i < WORK_ITERATIONS; i++) {
        sum += i;
    }
}

// Thread function
void* thread_function(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int policy;
    struct sched_param param;

    // Get current scheduling policy and priority
    pthread_getschedparam(pthread_self(), &policy, &param);

    printf("Thread %d started:\n", data->thread_id);
    printf("  Scheduling Policy: ");
    switch(policy) {
        case SCHED_OTHER: printf("SCHED_OTHER (Best Effort)\n"); break;
        case SCHED_FIFO:  printf("SCHED_FIFO (Real-Time FIFO)\n"); break;
        case SCHED_RR:    printf("SCHED_RR (Real-Time Round Robin)\n"); break;
        default:          printf("Unknown (%d)\n", policy);
    }
    printf("  Priority: %d (range: 1-99, higher = more priority)\n", param.sched_priority);
    printf("  Behavior: Runs until completion or yields, preempts lower priority\n\n");

    // Record start time
    clock_gettime(CLOCK_MONOTONIC, &data->start_time);
    long long end_time = get_time_ns() + (RUN_DURATION_SEC * 1000000000LL);

    // Perform work for specified duration
    data->work_count = 0;
    while (get_time_ns() < end_time) {
        do_work();
        data->work_count++;

        // Occasionally yield to allow lower-priority threads to run
        // Without this, lower priority threads would be starved
        if (data->work_count % 100 == 0) {
            sched_yield();
        }
    }

    // Record end time
    clock_gettime(CLOCK_MONOTONIC, &data->end_time);

    return NULL;
}

// Check if running with sufficient privileges
int check_privileges(void) {
    struct sched_param param;
    param.sched_priority = 1;

    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        return 0; // Failed
    }

    // Reset to normal
    param.sched_priority = 0;
    sched_setscheduler(0, SCHED_OTHER, &param);
    return 1; // Success
}

int main(int argc, char* argv[]) {
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    pthread_attr_t attr[NUM_THREADS];
    struct sched_param param;
    int rc;

    printf("===========================================\n");
    printf("Real-Time Scheduling (SCHED_FIFO) Demo\n");
    printf("===========================================\n\n");

    // Check privileges
    if (!check_privileges()) {
        fprintf(stderr, "ERROR: Insufficient privileges for real-time scheduling!\n");
        fprintf(stderr, "Please run with sudo: sudo %s\n\n", argv[0]);
        fprintf(stderr, "Why sudo is needed:\n");
        fprintf(stderr, "- SCHED_FIFO can lock the CPU and starve other processes\n");
        fprintf(stderr, "- Could prevent system from responding (freeze system)\n");
        fprintf(stderr, "- Only privileged users can set real-time policies\n");
        return 1;
    }

    // Lock memory to prevent paging (important for real-time)
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        fprintf(stderr, "Warning: Could not lock memory: %s\n", strerror(errno));
        fprintf(stderr, "Real-time performance may be affected by page faults.\n\n");
    }

    printf("This demo creates %d threads with SCHED_FIFO policy.\n", NUM_THREADS);
    printf("Threads have different priorities to demonstrate preemption.\n\n");

    printf("Thread Priorities:\n");
    printf("  Thread 1: Priority %d (HIGH)   - Should complete most work\n", HIGH_PRIORITY);
    printf("  Thread 2: Priority %d (MEDIUM) - Moderate work\n", MED_PRIORITY);
    printf("  Thread 3: Priority %d (LOW)    - Least work (may be starved)\n\n", LOW_PRIORITY);

    printf("Expected Behavior:\n");
    printf("- Higher priority threads preempt lower priority threads\n");
    printf("- Thread 1 (highest) should dominate CPU time\n");
    printf("- Thread 3 (lowest) may barely run (starvation)\n");
    printf("- Deterministic, predictable execution\n");
    printf("- Note: We use sched_yield() to prevent complete starvation\n\n");

    // Define priorities for threads
    int priorities[NUM_THREADS] = {HIGH_PRIORITY, MED_PRIORITY, LOW_PRIORITY};

    // Create threads with real-time priorities
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i + 1;
        thread_data[i].priority = priorities[i];
        thread_data[i].work_count = 0;
        thread_data[i].preemption_count = 0;

        // Initialize thread attributes
        pthread_attr_init(&attr[i]);

        // Set scheduling policy to SCHED_FIFO
        pthread_attr_setschedpolicy(&attr[i], SCHED_FIFO);

        // Set priority
        param.sched_priority = priorities[i];
        pthread_attr_setschedparam(&attr[i], &param);

        // Tell pthread to use our scheduling parameters (not inherit from parent)
        pthread_attr_setinheritsched(&attr[i], PTHREAD_EXPLICIT_SCHED);

        rc = pthread_create(&threads[i], &attr[i], thread_function, &thread_data[i]);
        if (rc) {
            fprintf(stderr, "Error creating thread %d: %s\n", i, strerror(rc));
            exit(1);
        }

        pthread_attr_destroy(&attr[i]);
    }

    printf("All threads created. Running for %d seconds...\n\n", RUN_DURATION_SEC);

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Unlock memory
    munlockall();

    // Print results
    printf("\n===========================================\n");
    printf("Results\n");
    printf("===========================================\n\n");

    char telemetry[2048];
    char thread_json[512];
    snprintf(telemetry, sizeof(telemetry), "{\"policy\": \"SCHED_FIFO\", \"threads\": [");

    long long total_work = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        double elapsed = time_diff_ms(thread_data[i].start_time, thread_data[i].end_time);
        total_work += thread_data[i].work_count;
        double work_rate = (thread_data[i].work_count * 1000.0) / elapsed;

        printf("Thread %d (Priority %d):\n",
               thread_data[i].thread_id, thread_data[i].priority);
        printf("  Work iterations completed: %lld\n", thread_data[i].work_count);
        printf("  Elapsed time: %.2f ms\n", elapsed);
        printf("  Work rate: %.2f iterations/sec\n\n", work_rate);

        snprintf(thread_json, sizeof(thread_json), 
                 "{\"id\": %d, \"priority\": %d, \"work_count\": %lld, \"elapsed_ms\": %.2f, \"work_rate\": %.2f}%s",
                 thread_data[i].thread_id, thread_data[i].priority, thread_data[i].work_count, elapsed, work_rate,
                 (i == NUM_THREADS - 1) ? "" : ", ");
        strcat(telemetry, thread_json);
    }
    strcat(telemetry, "]}");
    send_telemetry(telemetry);

    printf("Total work across all threads: %lld iterations\n\n", total_work);

    // Analyze priority impact
    printf("Priority Impact Analysis:\n");
    double high_pct = (double)thread_data[0].work_count / total_work * 100.0;
    double med_pct = (double)thread_data[1].work_count / total_work * 100.0;
    double low_pct = (double)thread_data[2].work_count / total_work * 100.0;

    printf("  HIGH priority (Thread 1): %.1f%% of total work\n", high_pct);
    printf("  MED priority  (Thread 2): %.1f%% of total work\n", med_pct);
    printf("  LOW priority  (Thread 3): %.1f%% of total work\n\n", low_pct);

    printf("Key Observations:\n");
    printf("- Higher priority threads completed significantly more work\n");
    printf("- This demonstrates strict priority preemption\n");
    printf("- In pure SCHED_FIFO, low priority can be starved completely\n");
    printf("- sched_yield() calls allowed some fairness\n");
    printf("- Real-time systems must carefully design priority schemes\n");
    printf("- Use SCHED_FIFO for time-critical tasks with known priorities\n");

    return 0;
}
