#include "FreeRTOS.h"
#include "task.h"
#include <avr/io.h>

#define FAN PORTB0
#define LIGHT PORTB1

// Methods Declaration
void vLightTask (void *pvParameters);
void vFanTask (void *pvParameters);

/*-----------------------------------------------------------*/

int main (void) {
	// PORTB0 and PORTB1 are output
	DDRB = (1 << LIGHT) | (1 << FAN);
	
	xTaskCreate(vLightTask, "Light", 256, NULL, 1, NULL);
	xTaskCreate(vFanTask, "Fan", 256, NULL, 2, NULL);
	vTaskStartScheduler();

	while(1);
	
	return 0;
}

/*-----------------------------------------------------------*/

// Method 1 implementation
void vLightTask (void* pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	while (1) {
		PORTB |= (1 << LIGHT);
		vTaskDelayUntil(&xLastWakeTime, (500 / portTICK_PERIOD_MS));	// Delay 500 ms
		
		PORTB &= ~(1 << LIGHT);
		vTaskDelayUntil(&xLastWakeTime, (500 / portTICK_PERIOD_MS));
	}
}

// Method 1 implementation
void vFanTask (void* pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	while (1) {
		PORTB |= (1 << FAN);
		vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_PERIOD_MS));	// Delay 1 s
		
		PORTB &= ~(1 << FAN);
		vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_PERIOD_MS));
	}
}