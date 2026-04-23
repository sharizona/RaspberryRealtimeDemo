/**
 * Basic Pthread Example - Best Effort Scheduling (SCHED_OTHER)
 *
 * This demonstrates standard pthread behavior with the default SCHED_OTHER policy.
 * SCHED_OTHER uses time-sharing (CFS - Completely Fair Scheduler in Linux).
 *
 * Key Characteristics:
 * - Fair CPU time distribution among threads
 * - No real-time guarantees
 * - Priority values (nice) affect CPU time allocation, not strict priority
 * - Suitable for non-time-critical applications
 *
 * Compile: gcc -o basic_pthread basic_pthread.c -lpthread -lrt
 * Run: ./basic_pthread
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "../telemetry.h"

#define NUM_THREADS 3
#define WORK_ITERATIONS 1000000
#define RUN_DURATION_SEC 5

// Structure to pass data to threads
typedef struct {
    int thread_id;
    int priority;
    long long work_count;
    struct timespec start_time;
    struct timespec end_time;
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
        case SCHED_FIFO:  printf("SCHED_FIFO (Real-Time)\n"); break;
        case SCHED_RR:    printf("SCHED_RR (Real-Time Round Robin)\n"); break;
        default:          printf("Unknown (%d)\n", policy);
    }
    printf("  Priority: %d\n", param.sched_priority);
    printf("  Nice value effect: Lower nice = more CPU time (not strict priority)\n\n");

    // Record start time
    clock_gettime(CLOCK_MONOTONIC, &data->start_time);
    long long end_time = get_time_ns() + (RUN_DURATION_SEC * 1000000000LL);

    // Perform work for specified duration
    data->work_count = 0;
    while (get_time_ns() < end_time) {
        do_work();
        data->work_count++;
    }

    // Record end time
    clock_gettime(CLOCK_MONOTONIC, &data->end_time);

    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    int rc;

    printf("===========================================\n");
    printf("Best Effort Scheduling (SCHED_OTHER) Demo\n");
    printf("===========================================\n\n");

    printf("This demo creates %d threads with SCHED_OTHER policy.\n", NUM_THREADS);
    printf("All threads will run for %d seconds and compete for CPU time.\n\n", RUN_DURATION_SEC);

    printf("Expected Behavior:\n");
    printf("- Threads share CPU time fairly (CFS scheduler)\n");
    printf("- Work counts should be similar across threads\n");
    printf("- No real-time guarantees or strict priorities\n");
    printf("- Timing is non-deterministic\n\n");

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i + 1;
        thread_data[i].priority = 0; // SCHED_OTHER ignores priority
        thread_data[i].work_count = 0;

        rc = pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);
        if (rc) {
            fprintf(stderr, "Error creating thread %d: %s\n", i, strerror(rc));
            exit(1);
        }
    }

    printf("All threads created. Running for %d seconds...\n\n", RUN_DURATION_SEC);

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print results
    printf("\n===========================================\n");
    printf("Results\n");
    printf("===========================================\n\n");

    char telemetry[2048];
    char thread_json[512];
    snprintf(telemetry, sizeof(telemetry), "{\"policy\": \"SCHED_OTHER\", \"threads\": [");

    long long total_work = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        double elapsed = time_diff_ms(thread_data[i].start_time, thread_data[i].end_time);
        total_work += thread_data[i].work_count;
        double work_rate = (thread_data[i].work_count * 1000.0) / elapsed;

        printf("Thread %d:\n", thread_data[i].thread_id);
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

    // Calculate fairness
    double avg_work = (double)total_work / NUM_THREADS;
    printf("Average work per thread: %.0f iterations\n", avg_work);
    printf("Total work across all threads: %lld iterations\n\n", total_work);

    // Calculate variance to show fairness
    double variance = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        double diff = thread_data[i].work_count - avg_work;
        variance += diff * diff;
    }
    variance /= NUM_THREADS;
    double std_dev = sqrt(variance);
    double cv = (std_dev / avg_work) * 100.0; // Coefficient of variation

    printf("Work Distribution Analysis:\n");
    printf("  Standard deviation: %.2f\n", std_dev);
    printf("  Coefficient of variation: %.2f%%\n", cv);
    printf("  Interpretation: Lower CV = more fair distribution\n\n");

    printf("Key Observations:\n");
    printf("- SCHED_OTHER provides fair CPU time sharing\n");
    printf("- Work counts are relatively balanced (low variance)\n");
    printf("- No thread has guaranteed priority over others\n");
    printf("- Suitable for non-real-time applications\n");

    return 0;
}
