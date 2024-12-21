#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "USART.h"
#include "wiring.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>

#define F_CPU 16000000UL
#define BUFFER_SIZE 10

/*-----------------------------------------------------------*/

void* vProducer(void* arg);
void* vConsumer(void* arg);

TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2;
SemaphoreHandle_t semaphore;

// Shared resources
int buffer[BUFFER_SIZE];
int count = 0; // Number of items in the buffer

/*-----------------------------------------------------------*/

int main(void)
{ 
	cli();
	USART_init();
	semaphore =  xSemaphoreCreateBinary();
	
	if (semaphore != NULL) {
		DDRC = 0xff;
		xTaskCreate (vProducer, "Producer", 256, NULL, 1, &TaskHandle_1);
		xTaskCreate (vConsumer, "Consumer", 256, NULL, 2, &TaskHandle_2);
		sei();
		vTaskStartScheduler();
	}
	else {
		USART_sendstr("Semaphore not Created Correctly !!");
	}
	
	while (1);
	return 0;
}

void* vProducer(void* arg) {
	BaseType_t xStatus;
	while (1) {

		int item = rand() % 100;
		
		// TODO [1]: Call an API to get/take the semaphore
		xStatus = xSemaphoreTake(semaphore, 100);
		if (xStatus == pdPASS) {
			// TODO [2]: Produce item (fill the buffer with the item), then increment the count
			
			// Enter critical section
			taskENTER_CRITICAL();
			
			if (count < BUFFER_SIZE) {
				buffer[count] = item;
				count++;
			}
			
			// TODO [3]: Print produced item;
			printf ("Produced: %d\n", item);

			// Exit critical section
			taskEXIT_CRITICAL();
			
			// TODO [4]: IF releasing the semaphore != pdTRUE
			xStatus = xSemaphoreGive(semaphore);
			if (xStatus != pdPASS) {
				USART_sendstr("Semaphore can not release by PRODUCER Task.\r\n");
			}
		}
	}
}

void* vConsumer(void* arg) {
	int item;
	BaseType_t xStatus;
	
	while (1) {
		// TODO [5]: Call an API to get/take the semaphore*/
		xStatus = xSemaphoreTake(semaphore, 100);
		if (xStatus == pdPASS) {

			// TODO [7]: Consume item, then increment the count
			
			// Enter critical section
			taskENTER_CRITICAL();
			
			item = buffer[count - 1];
			count--;
			
			// TODO [7]: Print consumed item;
			printf ("Consumed: %d\n", item);
			
			// Exit critical section
			taskEXIT_CRITICAL();
        
			// TODO [8]: IF releasing the semaphore != pdTRUE
			xStatus = xSemaphoreGive(semaphore);
			if (xStatus != pdPASS) {
				USART_sendstr("Semaphore can not release by PRODUCER Task.\r\n");
			}
		}
	}
}



