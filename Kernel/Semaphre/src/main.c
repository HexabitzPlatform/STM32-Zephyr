/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>


#define PRODUCER_STACKSIZE 512
#define CONSUMER_STACKSIZE 512

/* Set the priority of the producer and consumer thread */
#define PRODUCER_PRIORITY        5 
#define CONSUMER_PRIORITY        4

/* Define semaphore to monitor instances of available resource */
K_SEM_DEFINE(instance_monitor_sem, 10, 10);

/* Initialize the available instances of this resource */
volatile int available_instance_count = 10;

// Function for getting access of resource
void get_access(void)
{
	/* Take semaphore before access to the resource */
	k_sem_take(&instance_monitor_sem, K_FOREVER);

	/* Decrement available resource */
	available_instance_count--;
    printk("Resource taken and available_instance_count = %d\n",
	  available_instance_count);

}

// Function for releasing access of resource
void release_access(void)
{
	/* Increment available resource */
	available_instance_count++;
    printk("Resource given and available_instance_count = %d\n",
      available_instance_count);

	/* Give semaphore after finishing access to resource */
	k_sem_give(&instance_monitor_sem); 
}

/* Producer thread relinquishing access to instance */
void thread0(void)
{
	while (1) {
		release_access();
		/* Assume the resource instance access is released at this point */ 
		k_msleep(500);
	}
}

/* Consumer thread obtaining access to instance */
void thread1(void)
{
	while (1) {
		get_access();
		/* Assume the resource instance access is released at this point */
		k_msleep(10);
	}
}

// Define and initialize threads
K_THREAD_DEFINE(producer_id, PRODUCER_STACKSIZE, thread0, NULL, NULL, NULL, PRODUCER_PRIORITY, 0,
		0);
K_THREAD_DEFINE(consumer_id, CONSUMER_STACKSIZE, thread1, NULL, NULL, NULL, CONSUMER_PRIORITY, 0,
		0);