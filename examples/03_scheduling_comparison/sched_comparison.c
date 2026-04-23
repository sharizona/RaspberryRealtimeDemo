/**
 * Scheduling Policy Comparison: SCHED_OTHER vs SCHED_FIFO
 *
 * This program demonstrates the key differences between best-effort and
 * real-time scheduling by running both policies and comparing results.
 *
 * Tests performed:
 * 1. Response time measurement (interrupt simulation)
 * 2. Priority enforcement
 * 3. Latency analysis
 * 4. Jitter measurement
 *
 * Compile: gcc -o sched_comparison sched_comparison.c -lpthread -lrt -lm
 * Run: sudo ./sched_comparison
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <sys/mman.h>
#include <math.h>

#define NUM_SAMPLES 1000
#define SLEEP_INTERVAL_US 1000  // 1ms target
#define WORK_ITERATIONS 100000

typedef struct {
    long long latencies[NUM_SAMPLES];
    int sample_count;
    int policy;
    int priority;
} latency_data_t;

// Get current time in nanoseconds
long long get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

// Simulated work
void do_work(void) {
    volatile long sum = 0;
    for (int i = 0; i < WORK_ITERATIONS; i++) {
        sum += i;
    }
}

// Calculate statistics
void calculate_stats(latency_data_t* data, double* avg, double* min, double* max,
                     double* std_dev, double* jitter) {
    *min = data->latencies[0] / 1000.0;  // Convert to microseconds
    *max = data->latencies[0] / 1000.0;
    double sum = 0;

    for (int i = 0; i < data->sample_count; i++) {
        double latency_us = data->latencies[i] / 1000.0;
        sum += latency_us;
        if (latency_us < *min) *min = latency_us;
        if (latency_us > *max) *max = latency_us;
    }

    *avg = sum / data->sample_count;

    // Calculate standard deviation
    double variance = 0;
    for (int i = 0; i < data->sample_count; i++) {
        double latency_us = data->latencies[i] / 1000.0;
        double diff = latency_us - *avg;
        variance += diff * diff;
    }
    variance /= data->sample_count;
    *std_dev = sqrt(variance);

    // Jitter = standard deviation of latencies
    *jitter = *std_dev;
}

// Latency measurement thread
void* latency_thread(void* arg) {
    latency_data_t* data = (latency_data_t*)arg;
    struct timespec sleep_time;

    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = SLEEP_INTERVAL_US * 1000;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        long long target_time = get_time_ns() + (SLEEP_INTERVAL_US * 1000LL);

        // Sleep
        nanosleep(&sleep_time, NULL);

        // Measure how late we are
        long long actual_time = get_time_ns();
        long long latency = actual_time - target_time;

        data->latencies[i] = latency;
        data->sample_count++;

        // Do some work to create load
        if (i % 10 == 0) {
            do_work();
        }
    }

    return NULL;
}

// Set thread scheduling policy
int set_thread_policy(pthread_t thread, int policy, int priority) {
    struct sched_param param;
    param.sched_priority = priority;

    if (pthread_setschedparam(thread, policy, &param) != 0) {
        return -1;
    }
    return 0;
}

// Run latency test with specified policy
void run_latency_test(int policy, int priority, const char* policy_name) {
    pthread_t thread;
    pthread_attr_t attr;
    struct sched_param param;
    latency_data_t data;

    memset(&data, 0, sizeof(data));
    data.policy = policy;
    data.priority = priority;

    printf("\n--- Testing %s (Priority: %d) ---\n", policy_name, priority);

    // Initialize thread attributes
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, policy);
    param.sched_priority = priority;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    // Create thread
    if (pthread_create(&thread, &attr, latency_thread, &data) != 0) {
        fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
        pthread_attr_destroy(&attr);
        return;
    }

    pthread_attr_destroy(&attr);

    // Wait for completion
    pthread_join(thread, NULL);

    // Calculate and print statistics
    double avg, min, max, std_dev, jitter;
    calculate_stats(&data, &avg, &min, &max, &std_dev, &jitter);

    printf("Samples collected: %d\n", data.sample_count);
    printf("Average latency: %.2f μs\n", avg);
    printf("Min latency: %.2f μs\n", min);
    printf("Max latency: %.2f μs (worst-case)\n", max);
    printf("Std deviation: %.2f μs\n", std_dev);
    printf("Jitter: %.2f μs\n", jitter);

    // Print latency distribution
    int buckets[10] = {0};
    for (int i = 0; i < data.sample_count; i++) {
        double latency_us = data.latencies[i] / 1000.0;
        int bucket = (int)(latency_us / 100.0);
        if (bucket > 9) bucket = 9;
        buckets[bucket]++;
    }

    printf("\nLatency Distribution (μs):\n");
    for (int i = 0; i < 10; i++) {
        if (buckets[i] > 0) {
            printf("  %4d-%4d: ", i * 100, (i + 1) * 100);
            int bars = (buckets[i] * 50) / data.sample_count;
            for (int j = 0; j < bars; j++) printf("█");
            printf(" %d\n", buckets[i]);
        }
    }
}

// Priority test: measure if higher priority actually gets more CPU
typedef struct {
    int priority;
    long long work_count;
    int policy;
} priority_test_data_t;

void* priority_test_thread(void* arg) {
    priority_test_data_t* data = (priority_test_data_t*)arg;
    long long end_time = get_time_ns() + 2000000000LL; // 2 seconds

    data->work_count = 0;
    while (get_time_ns() < end_time) {
        do_work();
        data->work_count++;

        // Yield occasionally to prevent starvation in SCHED_FIFO
        if (data->policy == SCHED_FIFO && data->work_count % 50 == 0) {
            sched_yield();
        }
    }

    return NULL;
}

void run_priority_test(int policy, const char* policy_name) {
    pthread_t threads[3];
    pthread_attr_t attr[3];
    priority_test_data_t data[3];
    struct sched_param param;
    int priorities[3] = {80, 50, 20}; // High, Med, Low

    printf("\n--- Priority Test: %s ---\n", policy_name);

    for (int i = 0; i < 3; i++) {
        data[i].priority = (policy == SCHED_FIFO) ? priorities[i] : 0;
        data[i].work_count = 0;
        data[i].policy = policy;

        pthread_attr_init(&attr[i]);
        pthread_attr_setschedpolicy(&attr[i], policy);
        param.sched_priority = data[i].priority;
        pthread_attr_setschedparam(&attr[i], &param);
        pthread_attr_setinheritsched(&attr[i], PTHREAD_EXPLICIT_SCHED);

        pthread_create(&threads[i], &attr[i], priority_test_thread, &data[i]);
        pthread_attr_destroy(&attr[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    long long total = data[0].work_count + data[1].work_count + data[2].work_count;
    printf("High priority (80): %lld iterations (%.1f%%)\n",
           data[0].work_count, (data[0].work_count * 100.0) / total);
    printf("Med priority  (50): %lld iterations (%.1f%%)\n",
           data[1].work_count, (data[1].work_count * 100.0) / total);
    printf("Low priority  (20): %lld iterations (%.1f%%)\n",
           data[2].work_count, (data[2].work_count * 100.0) / total);
}

int main(int argc, char* argv[]) {
    // Check privileges
    struct sched_param param;
    param.sched_priority = 1;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        fprintf(stderr, "ERROR: Insufficient privileges!\n");
        fprintf(stderr, "Please run: sudo %s\n", argv[0]);
        return 1;
    }
    param.sched_priority = 0;
    sched_setscheduler(0, SCHED_OTHER, &param);

    // Lock memory
    mlockall(MCL_CURRENT | MCL_FUTURE);

    printf("================================================\n");
    printf("Scheduling Policy Comparison\n");
    printf("================================================\n");
    printf("\nThis test compares SCHED_OTHER vs SCHED_FIFO\n");
    printf("Measuring: latency, jitter, and priority enforcement\n");

    // Test 1: Latency comparison
    printf("\n\n=== TEST 1: Latency & Jitter Analysis ===\n");
    run_latency_test(SCHED_OTHER, 0, "SCHED_OTHER");
    run_latency_test(SCHED_FIFO, 80, "SCHED_FIFO");

    // Test 2: Priority enforcement
    printf("\n\n=== TEST 2: Priority Enforcement ===\n");
    run_priority_test(SCHED_OTHER, "SCHED_OTHER");
    run_priority_test(SCHED_FIFO, "SCHED_FIFO");

    // Summary
    printf("\n\n=== SUMMARY ===\n");
    printf("\nSCHED_OTHER (Best Effort):\n");
    printf("  + Fair CPU sharing among threads\n");
    printf("  + Good for general-purpose applications\n");
    printf("  - Higher latency and jitter\n");
    printf("  - No priority guarantees\n");
    printf("  - Unpredictable timing\n");

    printf("\nSCHED_FIFO (Real-Time):\n");
    printf("  + Low, bounded latency\n");
    printf("  + Minimal jitter\n");
    printf("  + Strict priority enforcement\n");
    printf("  + Deterministic behavior\n");
    printf("  - Can starve low-priority tasks\n");
    printf("  - Requires root privileges\n");
    printf("  - Must be carefully designed\n");

    printf("\nWhen to use SCHED_FIFO:\n");
    printf("  - Time-critical control systems\n");
    printf("  - Audio/video processing with deadlines\n");
    printf("  - Sensor data acquisition with timing requirements\n");
    printf("  - Any task where missing deadlines has consequences\n");

    munlockall();
    return 0;
}
