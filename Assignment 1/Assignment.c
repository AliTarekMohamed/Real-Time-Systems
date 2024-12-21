#include <avr/io.h>
#include <util/delay.h>
#include "FreeRTOS.h"
#include "task.h"

#define PEDESTRIAN_LIGHT (1 << PORTB2)
#define VEHICLE_LIGHT_RED (1 << PORTB4)
#define VEHICLE_LIGHT_GREEN (1 << PORTB3)

TaskHandle_t xVehicleLightTaskHandle = NULL;
TaskHandle_t xPedestrianLightTaskHandle = NULL;

void vVehicleLightTask(void *pvParameters);
void vPedestrianLightTask(void *pvParameters);

/************************************************************************/

int main(void) {
	// Initialize hardware, ports, and FreeRTOS

	DDRB |= VEHICLE_LIGHT_RED | VEHICLE_LIGHT_GREEN | PEDESTRIAN_LIGHT; // Set PB2, PB3 and PB4 as outputs

	xTaskCreate(vPedestrianLightTask, "pedestrian", 256, NULL, 2, &xPedestrianLightTaskHandle);
	xTaskCreate(vVehicleLightTask, "vehicle", 256, NULL, 1, &xVehicleLightTaskHandle);
	vTaskStartScheduler();

	while (1);
	return 0;
}

/************************************************************************/

void vVehicleLightTask(void *pvParameters) {
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while (1) {
		
		// 1- Vehicle light is green for 2000ms
		// 2- Suspend the pedestrian light task
		// 3- Vehicle light is red for 1000ms
		// 4- Resume the pedestrian light task
		
		PORTB &= ~VEHICLE_LIGHT_RED;
		PORTB |= VEHICLE_LIGHT_GREEN;
		vTaskSuspend(xPedestrianLightTaskHandle);
		vTaskDelayUntil(&xLastWakeTime, (2000 / portTICK_PERIOD_MS));
		
		PORTB |= VEHICLE_LIGHT_RED;
		PORTB &= ~VEHICLE_LIGHT_GREEN;
		vTaskResume(xPedestrianLightTaskHandle);
		vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_PERIOD_MS));
	}
}

void vPedestrianLightTask(void *pvParameters) {
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while (1) {
		// 5-Toggle the pedestrian light every 250ms
		
		vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_PERIOD_MS));
		PORTB |= PEDESTRIAN_LIGHT;
		
		vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_PERIOD_MS));
		PORTB &= ~PEDESTRIAN_LIGHT;
	}
}