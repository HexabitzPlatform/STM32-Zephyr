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
#define SLEEP_TIME_MS   100

/* The devicetree node identifier for the "led0" alias */
#define LED0_NODE DT_ALIAS(led0)

/* Get the device pointers of the LED through gpio_dt_spec */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* Get the device pointer of the UART hardware */
static const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(usart2));

/* Intrrupt Callback Function */
static void UARTInterruptCallback(const struct device *dev, void *user_data) {
	uint8_t RxData;
	
	/* uart_irq_update(dev):
	 * This function updates the internal state of the UART driver and returns a non-zero value
	 * if there is any change in the interrupt status (such as a new data reception or transmission event).
	 * 
	 * uart_irq_is_pending(dev):
	 * This function checks if there is a pending interrupt for the UART device.
	 * */
	if(uart_irq_update(dev) && uart_irq_is_pending(dev)) {

		/* checks if there is data available to read from the UART receive buffer */
		if(uart_irq_rx_ready(dev)) {
			/* Read the data from FIFO  */
			uart_fifo_read(dev, &RxData, 1);
			
			/* Echo (write) the data */
			uart_fifo_fill(dev , &RxData , 1);
		}

		/* checks if the UART transmit buffer is ready to send more data */
		if(uart_irq_tx_ready(dev)) {
		// Handle TX interrupt if needed
		}

	}

}

int main(void)
{
	int ret;

	/* Verify that the UART device is ready */
	if (!device_is_ready(uart)) {
    return 0;
    }

    /* Verify that the LED devices are ready */
	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

    /* Configure the GPIOs of the LEDs */
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	/* Set the UART interrupt callback handler */
	uart_irq_callback_set(uart , UARTInterruptCallback);

	/* Enable RX interrupts */
	uart_irq_rx_enable(uart);

	/* Enable TX interrupts */
	uart_irq_tx_enable(uart);

	while (1) {
		gpio_pin_toggle_dt(&led);

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
