#include <stdint.h>
#include <stdio.h>
#include <rtthread.h>
#include <rthw.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static struct rt_thread thread1;
static struct rt_thread thread2;
static struct rt_thread thread3;
static ALIGN(RT_ALIGN_SIZE) rt_uint8_t thread1_stack[THREAD_STACK_SIZE];
static ALIGN(RT_ALIGN_SIZE) rt_uint8_t thread2_stack[THREAD_STACK_SIZE];
static ALIGN(RT_ALIGN_SIZE) rt_uint8_t thread3_stack[THREAD_STACK_SIZE];

rt_spinlock_t lock;

static void thread_entry(void *parameter)
{
    rt_uint32_t count = 0;
    unsigned int num = (unsigned int) parameter;

    while (1)
    {
        rt_spin_lock(&lock);
        rt_kprintf("Thread%u is running on core %d, count: %u\n",
                   num, rt_hw_cpu_id(), count++);
        rt_spin_unlock(&lock);
        rt_thread_mdelay(1000);
    }
}

int main(void)
{
    rt_err_t result;

    rt_spin_lock_init(&lock);

    /* Thread 1 is bound to core 0. */
    result = rt_thread_init(&thread1,
                            "Thread1",
                            thread_entry,
                            (void *) 1,
                            &thread1_stack[0],
                            sizeof(thread1_stack),
                            THREAD_PRIORITY,
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

    /* Thread 2 is bound to core 1. */
    result = rt_thread_init(&thread2,
                            "Thread2",
                            thread_entry,
                            (void *) 2,
                            &thread2_stack[0],
                            sizeof(thread2_stack),
                            THREAD_PRIORITY,
                            THREAD_TIMESLICE);
    if (result != RT_EOK) {
        rt_kprintf("Failed to create thread 2: %d", result);
        return result;
    }
    result = rt_thread_control(&thread2, RT_THREAD_CTRL_BIND_CPU, (void *) 1);
    if (result != RT_EOK) {
        rt_kprintf("Failed to bind thread 2: %d", result);
        return result;
    }
    rt_thread_startup(&thread2);

    /*
     * Thread 3 can run on any core. While mostly it will run on core 0 due to
     * the core is not very busy, over time it will occationally run on core 1.
     */
    result = rt_thread_init(&thread3,
                            "Thread3",
                            thread_entry,
                            (void *) 3,
                            &thread3_stack[0],
                            sizeof(thread3_stack),
                            THREAD_PRIORITY,
                            THREAD_TIMESLICE);
    if (result != RT_EOK) {
        rt_kprintf("Failed to create thread 3: %d", result);
        return result;
    }
    rt_thread_startup(&thread3);

    return 0;
}
