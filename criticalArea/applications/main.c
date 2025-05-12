#include <stdint.h>
#include <stdio.h>
#include <rtthread.h>
#include <rthw.h>

#define THREAD_HIGH_PRIORITY    10
#define THREAD_LOW_PRIORITY     20
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static struct rt_thread thread1;
static struct rt_thread thread2;
static struct rt_thread thread3;
static ALIGN(RT_ALIGN_SIZE) rt_uint8_t thread1_stack[THREAD_STACK_SIZE];
static ALIGN(RT_ALIGN_SIZE) rt_uint8_t thread2_stack[THREAD_STACK_SIZE];
static ALIGN(RT_ALIGN_SIZE) rt_uint8_t thread3_stack[THREAD_STACK_SIZE];

rt_spinlock_t lock;

static void low_priority_entry(void *parameter)
{
    unsigned int num = (unsigned int) parameter;
    rt_uint32_t count = 0;
    unsigned long i, res;

    while (1)  {
        if (count % 2 == 0) {
            rt_enter_critical();
            rt_kprintf("Low priority Thread%u is running on core %u, in "
                       "critical area\n", num, rt_hw_cpu_id());
            /*
             * Even tick update has paused in the critical area, cannot use
             * delay; simulate using the CPU by some arithmetics. During
             * this period, higher priority task cannot preempt on this core.
             */
            for (i = 0; i < (1 << 29); i++) {
                res = i * i + res;
            }
        } else {
            rt_kprintf("Low priority Thread%u is running on core %u, leaving "
                       "critical area\n", num, rt_hw_cpu_id());
            rt_exit_critical();
            rt_thread_mdelay(3000);
        }
        count++;
    }
}

static void high_priority_entry(void *parameter)
{
    unsigned int num = (unsigned int) parameter;
    while (1) {
        rt_spin_lock(&lock);
        rt_kprintf("High priority Thread%u is running on core %u\n",
                   num, rt_hw_cpu_id());
        rt_spin_unlock(&lock);
        rt_thread_mdelay(1000);
        rt_thread_yield();
    }
}

int main(void)
{
    rt_err_t result;

    rt_spin_lock_init(&lock);

    /*
     * Thread 1 is bound to core 0 and periodically locks the core despite
     * being lower priority.
     */
    result = rt_thread_init(&thread1,
                            "Thread1",
                            low_priority_entry,
                            (void *) 1,
                            &thread1_stack[0],
                            sizeof(thread1_stack),
                            THREAD_LOW_PRIORITY,
                            THREAD_TIMESLICE);
    if (result != RT_EOK) {
        rt_kprintf("Failed to create thread 1: %d", result);
        return result;
    }
    result = rt_thread_control(&thread1, RT_THREAD_CTRL_BIND_CPU, (void *) 0);
    if (result != RT_EOK) {
        rt_kprintf("Failed to bind thread 1: %d", result);
        return result;
    }
    rt_thread_startup(&thread1);

    /*
     * Thread 2 is higher priority and is bound to core 0. It wakes more frequent
     * than thread 1.
     */
    result = rt_thread_init(&thread2,
                            "Thread2",
                            high_priority_entry,
                            (void *) 2,
                            &thread2_stack[0],
                            sizeof(thread2_stack),
                            THREAD_HIGH_PRIORITY,
                            THREAD_TIMESLICE);
    if (result != RT_EOK) {
        rt_kprintf("Failed to create thread 2: %d", result);
        return result;
    }
    result = rt_thread_control(&thread2, RT_THREAD_CTRL_BIND_CPU, (void *) 0);
    if (result != RT_EOK) {
        rt_kprintf("Failed to bind thread 2: %d", result);
        return result;
    }
    rt_thread_startup(&thread2);

    /*
     * Thread 3 is also higher priority, but can run on any core.
     */
    result = rt_thread_init(&thread3,
                            "Thread3",
                            high_priority_entry,
                            (void *) 3,
                            &thread3_stack[0],
                            sizeof(thread3_stack),
                            THREAD_HIGH_PRIORITY,
                            THREAD_TIMESLICE);
    if (result != RT_EOK) {
        rt_kprintf("Failed to create thread 3: %d", result);
        return result;
    }
    rt_thread_startup(&thread3);

    return 0;
}
