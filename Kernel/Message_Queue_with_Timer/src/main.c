/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

/* 2200 msec = 2.2 sec */
#define PRODUCER_SLEEP_TIME_MS 2200

LOG_MODULE_REGISTER(Adel_Faki, LOG_LEVEL_DBG);

/* Stack size for both the producer and consumer threads */
#define STACKSIZE		 2048
#define PRODUCER_THREAD_PRIORITY 6
#define CONSUMER_THREAD_PRIORITY 7

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* Create the expiry function for the timer */
static void TimerCallback (struct k_timer * dummy)
{
	/*Interrupt Context - System Timer ISR */
    gpio_pin_toggle_dt(&led0);

	/* let’s explore what happens if you call a kernel API that is intended
	 for the thread context inside the timer0_handler() interrupt context function */
	//k_msleep(2000);

}

/* Define the timer */
K_TIMER_DEFINE(timer0 , TimerCallback , NULL);

/* Define the data type of the message */
typedef struct {
    uint32_t x_reading;
    uint32_t y_reading;
    uint32_t z_reading;
} SensorReading;

/* Define the message queue */
K_MSGQ_DEFINE(DeviceQueue , sizeof(SensorReading) , 16 , 4);

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led0)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	/* start periodic timer that expires once every 0.5 second  */
	k_timer_start(&timer0 ,K_MSEC(500) , K_MSEC(500));

	return 0;
}

static void producer_func(void *unused1, void *unused2, void *unused3)
{
	ARG_UNUSED(unused1);
	ARG_UNUSED(unused2);
	ARG_UNUSED(unused3);

	static SensorReading acc_val = {100, 100, 100};
	int ret;

	while (1) {

		/* Write messages to the message queue */
		ret = k_msgq_put(&DeviceQueue , &acc_val , K_FOREVER);
		if(ret) {
			LOG_ERR("Return value from k_msgq_put = %d",ret);
		}

		acc_val.x_reading += 1;
		acc_val.y_reading += 1;
		acc_val.z_reading += 1;
		k_msleep(PRODUCER_SLEEP_TIME_MS);
	}
}

static void consumer_func(void *unused1, void *unused2, void *unused3)
{
	ARG_UNUSED(unused1);
	ARG_UNUSED(unused2);
	ARG_UNUSED(unused3);

	SensorReading data;
	int ret;

	while (1) {

		/* Read messages from the message queue */
		ret = k_msgq_get(&DeviceQueue , &data , K_FOREVER);
		if(ret) {
			LOG_ERR("Return value from k_msgq_get = %d",ret);
		}

		LOG_INF("Values got from the queue: %d.%d.%d\r\n", data.x_reading, data.y_reading,
			data.z_reading);
	}
}

K_THREAD_DEFINE(producer, STACKSIZE, producer_func, NULL, NULL, NULL, PRODUCER_THREAD_PRIORITY, 0,
		0);
K_THREAD_DEFINE(consumer, STACKSIZE, consumer_func, NULL, NULL, NULL, CONSUMER_THREAD_PRIORITY, 0,
		0);
