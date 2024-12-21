#include "FreeRTOS.h"
#include "task.h"
#include "USART.h"
#include "queue.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define F_CPU 16000000UL
#define BUTTON (1 << PORTB2);

QueueHandle_t xQueue;
static void vTerminal (void *pvParameters);
static void vCalcAverage (void *pvParameters);

/************************************************************************/

int main (void)
{
	USART_init();

	xQueue = xQueueCreate(10, sizeof(char));
	
	xTaskCreate(vTerminal, "Terminal", 256, NULL, 1, NULL);
	xTaskCreate(vCalcAverage, "calcAvg", 256, NULL, 1, NULL);
	vTaskStartScheduler();
	
	while (1);
	return 0;
}

/*-----------------------------------------------------------*/

static void vTerminal (void *pvParameters)
{
	char number;
	BaseType_t xStatus;

	while(1) {
		USART_sendstr("Enter Number : ");
		number = USART_receive();
		USART_sendstr("\r\n");
		xStatus = xQueueSendToFront(xQueue, &number, 10);
	}
}

/*-----------------------------------------------------------*/

static void vCalcAverage (void *pvParameters)
{
	BaseType_t xStatus;
	char lReceivedValue;
	int average = 0, counter = 0;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 99UL );

	while (1)
	{		
		xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);

		if (xStatus == pdPASS){
			
			if (lReceivedValue != '0' && counter < 10){
				average += (int)lReceivedValue;;
				counter++;
			}
			
			if (lReceivedValue == '0' || counter == 10){
				average /= counter;

				USART_sendstr("Average = ");
				USART_send(average);
				USART_sendstr("\r\n");
				counter = 0;
				average = 0;
			}
		}
	}
}