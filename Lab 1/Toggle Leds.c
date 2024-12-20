#include "FreeRTOS.h"
#include "task.h"
#include <avr/io.h>

#define LED1 PORTB0
#define LED2 PORTB1

// Declaration of two methods
void led1(void* pvParameters);
void led2(void* pvParameters);

/*-----------------------------------------------------------*/

int main(void)
{
	// PORTB0 and PORTB1 are output
	DDRB = (1 << LED1) | (1 << LED2);
	
	xTaskCreate(led1, "Led 1", 256, NULL, 1, NULL);
	xTaskCreate(led2, "Led 2", 256, NULL, 2, NULL);
	vTaskStartScheduler();

	while(1);

	return 0;
}

/*-----------------------------------------------------------*/

// Method 1 implementation
void led1(void* pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	while (1) {
		// Led 1 ON
		PORTB |= (1 << LED1);
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);	// Delay 100 ms
		
		// Led 1 OFF
		PORTB &= ~(1 << LED1);
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

// Method 2 implementation
void led2(void* pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	while (1) {
		// Led 2 ON
		PORTB |= (1 << LED2);
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
		
		// Led 2 OFF
		PORTB &= ~(1 << LED2);
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}