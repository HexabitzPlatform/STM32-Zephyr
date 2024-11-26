/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

#define RECEIVE_BUFF_SIZE 10
#define RECEIVE_TIMEOUT 100
#define SLEEP_TIME_MS 1000

/* Define the Tx/Rx buffers */
static uint8_t TxBuffer[] = {"STM32G0B1RE-NUCLEO Board , Adel Faki \r\n"};
static uint8_t RxBuffer[RECEIVE_BUFF_SIZE] = {0};

/* Get the device pointer of the UART hardware */
const struct device *uart= DEVICE_DT_GET(DT_NODELABEL(usart1));

static void UARTAsynchronousCallback(const struct device *dev, struct uart_event *evt, void *user_data);


int main(void)
{
  int ret;

  /* Verify that the UART device is ready */
	if (!device_is_ready(uart)){
		return 0 ;
	}

  /* Register the UART callback function */
  ret = uart_callback_set(uart, UARTAsynchronousCallback, NULL);
  	if (ret) {
		return 1;
	}

  /* Send the data over UART */
  ret = uart_tx(uart, TxBuffer, sizeof(TxBuffer) - 1 , SYS_FOREVER_US);
  	if (ret) {
      printk("Failed to send UART data\n");
		return 1;
	}

  /* Start receiving */
  /*ret = uart_rx_enable(uart, RxBuffer, sizeof RxBuffer, RECEIVE_TIMEOUT);
  	if (ret) {
      printk("Failed to enable UART data\n");
		return 1;
	}*/

  while (1)
  {
    k_msleep(SLEEP_TIME_MS);
  }
  return 0;
}

/* Define the callback function for UART */
static void UARTAsynchronousCallback(const struct device *dev, struct uart_event *evt, void *user_data)
{
  switch (evt->type)
  {
  /* This event occurs when the UART transmission is completed */  
  case UART_TX_DONE:
    printk("Transmission completed\n");
    break;
  
  /* This event occurs when an ongoing UART transmission is aborted
   *  it means that an ongoing process of sending data through the UART peripheral
   *  has been interrupted or halted before it could be completed. */
  case UART_TX_ABORTED:
    printk("Transmission aborted\n");
    break;

  /* This event occurs when data has been received and it is ready for processing
   * [evt->data.rx.offset]:
   * The offset is the starting position within the buffer where the received data begins. */
  case UART_RX_RDY:
    printk("Received data: %.*s\n", evt->data.rx.len, &RxBuffer[evt->data.rx.offset]);
    break;

  /* This event occurs when the UART driver requests a new receive buffer 
   * Provide a new buffer to the UART driver to continue receiving data. */
  case UART_RX_BUF_REQUEST:
    uart_rx_buf_rsp(dev, RxBuffer, sizeof(RxBuffer));
    break;

  /* This event occurs when the UART driver releases a previously provided receive buffer 
   * Use Case: You can reuse or deallocate the buffer once it is no longer needed. */
  case UART_RX_BUF_RELEASED:
    printk("Receive buffer released\n");
    break;

  /* This event occurs when UART reception is disabled. 
   * Use Case: Handle any cleanup required when UART reception is no longer needed. */
  case UART_RX_DISABLED:
    printk("Reception disabled\n");
    break;

  /* This event occurs when UART reception is stopped, typically due to an error.
   * Handle the error, reset the UART, or take corrective action. */
  case UART_RX_STOPPED:
    printk("Reception stopped\n");
    break;

  default:
    printk("Unhandled UART event: %d\n", evt->type);
  }

}

