/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>



// Define stack size used by each thread
#define THREAD0_STACKSIZE      512
#define THREAD1_STACKSIZE      512
#define WORQ_THREAD_STACK_SIZE 512

/* Set the priorities of the threads */
#define THREAD0_PRIORITY 2 
#define THREAD1_PRIORITY 3
#define WORKQ_PRIORITY   4 /* lowest priority since I want this thread to execute offloaded (non-urgent) work */

/* Define stack area used by workqueue thread */ 
static K_THREAD_STACK_DEFINE(my_stack_area , WORQ_THREAD_STACK_SIZE);

/* Define queue structure */ 
static struct k_work_q offload_work_q = {0};

/* Define a global varible */
volatile int counter = 0;

/* Define function to emulate non-urgent work */
 void emulate_work(void)
{
	if (counter > 10000)
	counter = 0;

	 counter++;
}

/* Create work_info structure and offload function */
struct k_work work;


void offload_function(struct k_work *work_tem)
{
	emulate_work();
}

void thread0(void)
{
	/* Start the workqueue, */
	k_work_queue_start(&offload_work_q, my_stack_area, K_THREAD_STACK_SIZEOF(my_stack_area),WORKQ_PRIORITY, NULL);
    /* initialize the work item and connect it to its handler function */
	k_work_init(&work, offload_function);

	while (1) {
		/* Submit the work item to the workqueue instead of calling emulate_work() directly */
		k_work_submit_to_queue(&offload_work_q , &work);

		k_msleep(5);

	}
}


void thread1(void)
{
	while (1) {

		printk("thread1 yielding ,counter value =  %d \n", counter);
		k_msleep(5);

	}
}

/* Define entry function for thread1 */
K_THREAD_DEFINE(thread0_id, THREAD0_STACKSIZE, thread0, NULL, NULL, NULL, THREAD0_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread1_id, THREAD1_STACKSIZE, thread1, NULL, NULL, NULL, THREAD1_PRIORITY, 0, 0);
