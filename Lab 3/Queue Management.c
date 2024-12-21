#define F_CPU 16000000UL
#include "FreeRTOS.h"
#include "task.h"
#include "USART.h"
#include "queue.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

///////////////////////////////////////////////////////////////
/////////////////////////Requirements//////////////////////////
///////////////////////////////////////////////////////////////

/* ToDo [1]: The queue is created to hold a maximum of 5 long values. Create A Suitable Queue. */

/* ToDo [2]: Create two instances of the task that will write to the queue. The parameter is used to pass the value that the task should write to the queue,
so one task will continuously write 100 to the queue while the other task will continuously write 200 to the queue. Both tasks are created at priority 1. */

/* ToDo [3]: Create the task that will read from the queue. The task is created with priority 2, so above the priority of the sender tasks. */

/* ToDo [4]: Insert back the element in the Queue without waiting for available space. */

/* ToDo [5]: Receive the next element in the Queue and wait for a xTicksToWait for arriving at least one element. */

/*-----------------------------------------------------------*/

// Methods Declaration
static void vSenderTask (void *pvParameters);
static void vReceiverTask (void *pvParameters);

// Declare a global queue
QueueHandle_t xQueue;


int main (void) {
	USART_init();
	
	// ToDo [1] */
	xQueue = xQueueCreate(5, sizeof(long));
	
	if (xQueue != NULL) {

		/* ToDo [2] */
		xTaskCreate(vSenderTask, "Send to Queue", 256, "100", 1, NULL);
		xTaskCreate(vSenderTask, "Send to Queue", 256, "200", 1, NULL);
		
		/* ToDo [3] */
		xTaskCreate(vReceiverTask, "Receive from Queue", 256, NULL, 2, NULL);
		
		vTaskStartScheduler();
	}
	else {
		USART_sendstr ("The queue could not be created");
	}
	
	while (1);
	
	return 0;
}

/*-----------------------------------------------------------*/

static void vSenderTask (void* pvParameters) {
	BaseType_t xStatus;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	long lValueToSend = (long) pvParameters;
	
	USART_sendstr("Sender\r\n");
	
	while (1) {
		
		// ToDo [4] */
		xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);
		
		if (xStatus != pdPASS) {
			USART_sendstr("Could not send to the queue.\r\n");
		}
		
		vTaskDelayUntil(&xLastWakeTime, (100 / portTICK_PERIOD_MS));
	}
}

/*-----------------------------------------------------------*/

static void vReceiverTask (void *pvParameters) {
	long lReceivedValue;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(99UL);

	while (1) {
		
		// As this task unblocks immediately that data is written to the queue this call should always find the queue empty.
		if (uxQueueMessagesWaiting (xQueue) != 0) {
			USART_sendstr("Queue should be empty!\r\n");
		}

		/* ToDo [5] */
		xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);
		
		if (xStatus == pdPASS) {
			USART_sendstr ("Received = ");
			USART_sendstr (lReceivedValue);
			USART_sendstr ("\r\n");
		}
		else {
			USART_sendstr("Could not receive from the queue.\r\n" );
		}
	}
}
