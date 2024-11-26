/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 100

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

static uint8_t rx_data;

/* structure to hold information about the peripheral in a standard way.
 * Some drivers in Zephyr have API-specific structures and calls that encapsulate all
 * the information needed to control the device in one structure.
 * The UART driver does not have this, so we will use the macro call DEVICE_DT_GET()
 * */
/* Get the device pointer of the UART hardware */
const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(usart2));

/* Get the device pointer of the GPIO Pin hardware */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	int ret;

	/* Verify that the GPIO device is ready */
	if (!gpio_is_ready_dt(&led))
	{
		return 0;
	}

	/* */
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		return 0;
	}

	/* Verify that the UART device is ready */
	if (!device_is_ready(uart))
	{
		return 0;
	}

	while (1)
	{

		gpio_pin_toggle_dt(&led);

		// Polling for incoming data
		ret = uart_poll_in(uart, &rx_data);
		if (ret == 0)
		{
			// Polling to send data
			uart_poll_out(uart, rx_data);

			// printk("Received data: %c\n", rx_data);
		}

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
