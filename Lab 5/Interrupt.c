#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "USART.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define TrackSize 5

/*-----------------------------------------------------------*/

static void vGetNextTrack();
static void vReceiverTask(void *pvParameters );

QueueHandle_t xQueue;
volatile unsigned long lastInterrupt;
volatile int playListIndex = 0;
char const *playList[] = {"Track1", "Track2", "Track3", "Track4", "Track5"};
	
/*-----------------------------------------------------------*/

int main(void)
{
	USART_init();
	xQueue = xQueueCreate(5, sizeof(char *));
	
	// ToDo [1]: Disable global interrupt
	cli();
	
	// ToDo [2]: assign vGetNextTrack method to external Int0
	attachInterrupt(0, vGetNextTrack, 1);
	
	if( xQueue != NULL )
	{
		xTaskCreate(vReceiverTask, "Receiver", 256, NULL, 2, NULL );
		sei();
		vTaskStartScheduler();
	}
	else
		USART_sendstr("The queue could not be created");

	while (1);
	return 0;
	
}

/*-----------------------------------------------------------*/

static void vGetNextTrack()
{
	char *lValueToSend = NULL;
	BaseType_t xStatus;
  
	if(xTaskGetTickCountFromISR() - lastInterrupt > 500) // we set a 10ms no-interrupts window
	{
		// ToDo [3]: Set lValueToSend to the right track
		lValueToSend = playList[playListIndex];
		
		// ToDo [4]: Send lValueToSend to Queue using appropriate methods 'suffix by FromISR'
		xStatus = xQueueSendToBackFromISR(xQueue, &lValueToSend, 0);
	 
		if (xStatus != pdPASS)
			USART_sendstr("Could not send to the queue.");
		
		lastInterrupt = xTaskGetTickCountFromISR();
		
		playListIndex++;
		if(playListIndex >= TrackSize)
		playListIndex = 0;
	}
}

static void vReceiverTask (void *pvParameters)
{
    char *lReceivedValue;
    BaseType_t xStatus;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xTicksToWait = pdMS_TO_TICKS(99UL);
   	
	while (1) {
		if (uxQueueMessagesWaiting (xQueue) != 0)
			USART_sendstr("Queue should have been empty!\r\n");
			
		xStatus = xQueueReceive (xQueue, &lReceivedValue, xTicksToWait);
		if (xStatus == pdPASS) {
			USART_sendstr("Received= system runs ");
			USART_sendstr(lReceivedValue);
			USART_sendstr( "\r\n");
		}
		else
			USART_sendstr("Could not receive from the queue.\r\n");
		
		vTaskDelayUntil (&xLastWakeTime, (1000 / portTICK_PERIOD_MS));
	}
}
