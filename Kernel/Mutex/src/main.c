/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>


#define THREAD0_STACKSIZE 512
#define THREAD1_STACKSIZE 512

/* Set the priority of the two threads to have equal priority*/
#define THREAD0_PRIORITY        4 
#define THREAD1_PRIORITY        4

/* Define the  counter */
int32_t increment_count = 0;

/* Define mutex to protect access to shared code section */
K_MUTEX_DEFINE(test);

// Shared code run by both threads
void shared_code_section(void)
{
	/* Lock the mutex */
	k_mutex_lock(&test , K_FOREVER);

	/* Increment count and decrement count changed */
	increment_count ++;

	/* Unlock the mutex */
	k_mutex_unlock(&test);

	if(increment_count > 2000)
	   increment_count =0;

}

/* Functions for thread0 and thread1 with a shared code section */
void thread0(void)
{
	printk("Thread 0 started\n");
	while (1) {
		shared_code_section();

		/* Print counter values if they do */
	    printk("Thread 0 counter = %d \n" , increment_count);
	}
}

void thread1(void)
{
	printk("Thread 1 started\n");
	while (1) {
		shared_code_section();

		/* Print counter values if they do */
	    printk("Thread 1 counter = %d \n" , increment_count);

	}
}

// Define and initialize threads
K_THREAD_DEFINE(thread0_id, THREAD0_STACKSIZE, thread0, NULL, NULL, NULL, THREAD0_PRIORITY, 0,
		0);

K_THREAD_DEFINE(thread1_id, THREAD1_STACKSIZE, thread1, NULL, NULL, NULL, THREAD1_PRIORITY, 0,
		0);
