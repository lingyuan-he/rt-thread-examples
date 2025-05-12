#include <stdint.h>
#include <stdio.h>
#include <rtthread.h>
#include <rthw.h>
#include <interrupt.h>

#define THREAD_PRIORITY         20
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        5
#define IPI_INT_ID              3

static struct rt_thread thread1;
static struct rt_thread thread2;
static ALIGN(RT_ALIGN_SIZE) rt_uint8_t thread1_stack[THREAD_STACK_SIZE];
static ALIGN(RT_ALIGN_SIZE) rt_uint8_t thread2_stack[THREAD_STACK_SIZE];

/* Inter-processor interrupt handler. */
static void ipi_handler(int vector, void *param)
{
    rt_base_t level = rt_cpus_lock();
    rt_kprintf("Core %u received IPI\n",  rt_hw_cpu_id());
    rt_cpus_unlock(level);
}

/* Sends an IPU to the next core. */
static void send_ipi(void)
{
    unsigned int current_core = rt_hw_cpu_id();
    /* On the last core, loops back to core 0. */
    unsigned int target_core = (current_core + 1) % RT_CPUS_NR;
   
    rt_kprintf("Core %u sending IPI to Core %u\n", current_core, target_core);
    rt_hw_ipi_send(IPI_INT_ID, 1 << target_core);
}

static void thread_entry(void *parameter)
{
    rt_base_t level = rt_cpus_lock();
    rt_hw_ipi_handler_install(IPI_INT_ID, ipi_handler);
    rt_hw_interrupt_umask(IPI_INT_ID);
    rt_cpus_unlock(level);

    while (1) {
        /* Repeating: sends IPI to the other core and then sleeps. */
        send_ipi();
        rt_thread_mdelay(2000);
    }
}

int main(void)
{
    rt_err_t result;

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

    /* Delay between threads to interleave the IPIs. */
    rt_thread_mdelay(1000);

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

    return 0;
}
