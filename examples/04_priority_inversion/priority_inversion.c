/**
 * Priority Inversion Demonstration and Solution
 *
 * Priority Inversion: A real-time system bug where a high-priority task
 * is blocked by a low-priority task, while a medium-priority task runs.
 *
 * Classic Example: Mars Pathfinder (1997)
 * - High-priority task blocked by low-priority task holding mutex
 * - Medium-priority task preempted low-priority task
 * - High-priority task starved, system watchdog reset
 *
 * Solution: Priority Inheritance Protocol
 * - When low-priority task holds resource needed by high-priority task
 * - Low-priority task temporarily inherits high priority
 * - Prevents medium-priority task from preempting
 *
 * Compile: gcc -o priority_inversion priority_inversion.c -lpthread -lrt
 * Run: sudo ./priority_inversion
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sched.h>

#define HIGH_PRIORITY 80
#define MED_PRIORITY 50
#define LOW_PRIORITY 20
#define WORK_ITERATIONS 50000000

pthread_mutex_t resource_mutex;
int inversion_detected = 0;

// Get current time in milliseconds
long long get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

void do_work(const char* task_name) {
    volatile long sum = 0;
    for (int i = 0; i < WORK_ITERATIONS; i++) {
        sum += i;
    }
}

// High-priority task: needs the shared resource
void* high_priority_task(void* arg) {
    int use_inheritance = *(int*)arg;
    long long start, end;

    printf("[HIGH] Started at %lld ms\n", get_time_ms());

    // Give low-priority task time to acquire mutex
    usleep(100000); // 100ms

    printf("[HIGH] Attempting to acquire resource at %lld ms\n", get_time_ms());
    start = get_time_ms();

    pthread_mutex_lock(&resource_mutex);

    end = get_time_ms();
    long long wait_time = end - start;

    printf("[HIGH] Acquired resource at %lld ms (waited %lld ms)\n", end, wait_time);

    if (wait_time > 500 && !use_inheritance) {
        printf("[HIGH] ⚠️  PRIORITY INVERSION DETECTED! Waited %lld ms\n", wait_time);
        inversion_detected = 1;
    } else if (use_inheritance) {
        printf("[HIGH] ✓ Priority inheritance prevented long delay\n");
    }

    // Use resource
    do_work("HIGH");
    printf("[HIGH] Using resource at %lld ms\n", get_time_ms());

    pthread_mutex_unlock(&resource_mutex);
    printf("[HIGH] Released resource at %lld ms\n", get_time_ms());
    printf("[HIGH] Completed at %lld ms\n", get_time_ms());

    return NULL;
}

// Medium-priority task: CPU-intensive, doesn't need resource
void* medium_priority_task(void* arg) {
    printf("[MED]  Started at %lld ms\n", get_time_ms());

    // Give low-priority task time to acquire mutex
    usleep(50000); // 50ms

    // Do intensive work (this is the problem in priority inversion)
    printf("[MED]  Doing intensive work at %lld ms\n", get_time_ms());
    for (int i = 0; i < 10; i++) {
        do_work("MED");
    }

    printf("[MED]  Completed at %lld ms\n", get_time_ms());
    return NULL;
}

// Low-priority task: holds the shared resource
void* low_priority_task(void* arg) {
    printf("[LOW]  Started at %lld ms\n", get_time_ms());

    // Acquire resource immediately
    pthread_mutex_lock(&resource_mutex);
    printf("[LOW]  Acquired resource at %lld ms\n", get_time_ms());

    // Do work while holding resource
    printf("[LOW]  Working with resource at %lld ms\n", get_time_ms());
    for (int i = 0; i < 5; i++) {
        do_work("LOW");
        usleep(10000); // Give other threads chance to start
    }

    printf("[LOW]  Releasing resource at %lld ms\n", get_time_ms());
    pthread_mutex_unlock(&resource_mutex);
    printf("[LOW]  Completed at %lld ms\n", get_time_ms());

    return NULL;
}

void run_scenario(int use_priority_inheritance, const char* scenario_name) {
    pthread_t high_thread, med_thread, low_thread;
    pthread_attr_t attr_high, attr_med, attr_low;
    struct sched_param param;
    pthread_mutexattr_t mutex_attr;

    printf("\n================================================\n");
    printf("%s\n", scenario_name);
    printf("================================================\n\n");

    // Initialize mutex with or without priority inheritance
    pthread_mutexattr_init(&mutex_attr);
    if (use_priority_inheritance) {
        pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_INHERIT);
        printf("✓ Priority Inheritance Protocol ENABLED\n\n");
    } else {
        pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_NONE);
        printf("✗ Priority Inheritance Protocol DISABLED\n\n");
    }
    pthread_mutex_init(&resource_mutex, &mutex_attr);

    // Create low-priority thread
    pthread_attr_init(&attr_low);
    pthread_attr_setschedpolicy(&attr_low, SCHED_FIFO);
    param.sched_priority = LOW_PRIORITY;
    pthread_attr_setschedparam(&attr_low, &param);
    pthread_attr_setinheritsched(&attr_low, PTHREAD_EXPLICIT_SCHED);

    // Create medium-priority thread
    pthread_attr_init(&attr_med);
    pthread_attr_setschedpolicy(&attr_med, SCHED_FIFO);
    param.sched_priority = MED_PRIORITY;
    pthread_attr_setschedparam(&attr_med, &param);
    pthread_attr_setinheritsched(&attr_med, PTHREAD_EXPLICIT_SCHED);

    // Create high-priority thread
    pthread_attr_init(&attr_high);
    pthread_attr_setschedpolicy(&attr_high, SCHED_FIFO);
    param.sched_priority = HIGH_PRIORITY;
    pthread_attr_setschedparam(&attr_high, &param);
    pthread_attr_setinheritsched(&attr_high, PTHREAD_EXPLICIT_SCHED);

    inversion_detected = 0;

    // Start threads in order: LOW, MED, HIGH
    pthread_create(&low_thread, &attr_low, low_priority_task, &use_priority_inheritance);
    usleep(10000);
    pthread_create(&med_thread, &attr_med, medium_priority_task, &use_priority_inheritance);
    usleep(10000);
    pthread_create(&high_thread, &attr_high, high_priority_task, &use_priority_inheritance);

    // Wait for completion
    pthread_join(high_thread, NULL);
    pthread_join(med_thread, NULL);
    pthread_join(low_thread, NULL);

    // Cleanup
    pthread_attr_destroy(&attr_high);
    pthread_attr_destroy(&attr_med);
    pthread_attr_destroy(&attr_low);
    pthread_mutex_destroy(&resource_mutex);
    pthread_mutexattr_destroy(&mutex_attr);

    printf("\n--- Scenario Complete ---\n");
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

    printf("================================================\n");
    printf("Priority Inversion Demonstration\n");
    printf("================================================\n\n");

    printf("This demo shows:\n");
    printf("1. Priority inversion problem (without solution)\n");
    printf("2. Priority inheritance solution\n\n");

    printf("Thread Priorities:\n");
    printf("  HIGH: %d - Needs shared resource\n", HIGH_PRIORITY);
    printf("  MED:  %d - CPU-intensive, no resource needed\n", MED_PRIORITY);
    printf("  LOW:  %d - Holds shared resource\n\n", LOW_PRIORITY);

    printf("Expected Without Priority Inheritance:\n");
    printf("  1. LOW acquires mutex\n");
    printf("  2. MED preempts LOW (higher priority)\n");
    printf("  3. HIGH waits for mutex (blocked by LOW)\n");
    printf("  4. MED runs extensively (inversion!)\n");
    printf("  5. Finally LOW finishes and HIGH can proceed\n");
    printf("  Result: HIGH priority task delayed by MED priority task!\n\n");

    printf("Expected With Priority Inheritance:\n");
    printf("  1. LOW acquires mutex\n");
    printf("  2. HIGH tries to acquire mutex\n");
    printf("  3. LOW inherits HIGH priority temporarily\n");
    printf("  4. MED cannot preempt (LOW now has HIGH priority)\n");
    printf("  5. LOW completes quickly and HIGH proceeds\n");
    printf("  Result: Inversion prevented!\n\n");

    // Scenario 1: Without priority inheritance (shows the problem)
    run_scenario(0, "SCENARIO 1: Priority Inversion Problem");

    sleep(1);

    // Scenario 2: With priority inheritance (shows the solution)
    run_scenario(1, "SCENARIO 2: Priority Inheritance Solution");

    // Summary
    printf("\n================================================\n");
    printf("SUMMARY\n");
    printf("================================================\n\n");

    printf("Priority Inversion:\n");
    printf("  - High-priority task blocked by low-priority task\n");
    printf("  - Medium-priority task runs, delaying high-priority task\n");
    printf("  - Famous bug: Mars Pathfinder (1997)\n\n");

    printf("Priority Inheritance Protocol:\n");
    printf("  - Low-priority task temporarily inherits high priority\n");
    printf("  - Prevents medium-priority task from interfering\n");
    printf("  - Ensures high-priority task doesn't wait long\n\n");

    printf("Real-World Solutions:\n");
    printf("  1. Priority Inheritance (demonstrated here)\n");
    printf("  2. Priority Ceiling Protocol\n");
    printf("  3. Avoid blocking high-priority tasks\n");
    printf("  4. Use lock-free data structures\n\n");

    printf("Use pthread_mutexattr_setprotocol() with:\n");
    printf("  - PTHREAD_PRIO_INHERIT: for priority inheritance\n");
    printf("  - PTHREAD_PRIO_PROTECT: for priority ceiling\n");

    return 0;
}
