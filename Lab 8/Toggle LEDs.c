#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "USART.h"
#define F_CPU 16000000UL

TimerHandle_t Timer;
QueueHandle_t ledQueue;

void vLed1(void* pvParameters);
void vLed2(void* pvParameters);
void taskSchedulerCallback(TimerHandle_t xTimer);

/************************************************************************/

int main (void) {
	DDRB = 0xFF;
	
	ledQueue = xQueueCreate(10, sizeof(int));

	Timer = xTimerCreate("Invoke function", pdMS_TO_TICKS(1000), pdTRUE, NULL, taskSchedulerCallback);

	xTimerStart(Timer, 0);
	xTaskCreate(vLed1, "Led1", 256, NULL, 1, NULL);
	xTaskCreate(vLed2, "Led2", 256, NULL, 1, NULL);
	vTaskStartScheduler();
	
	while(1);
}

/************************************************************************/

void vLed1(void* pvParameters){
	int randomNumber;	
	
	while (1) {
		if (xQueueReceive(ledQueue, &randomNumber, 100)){
			if (randomNumber == 0){
				PORTB &= ~(1 << PORTB0);
			}
			else {
				PORTB |= (1 << PORTB0);
			}
		}
	}
}

void vLed2(void* pvParameters){
	int randomNumber;
	
	while(1){
		if (xQueueReceive(ledQueue, &randomNumber, 100)){
			if (randomNumber == 0){
				PORTB &= ~(1 << PORTB1);
			}
			else {
				PORTB |= (1 << PORTB1);
			}
		}
	}
}

void taskSchedulerCallback(TimerHandle_t Timer) {
	int led1State = rand() % 2;
	int led2State = rand() % 2;

	xQueueSend(ledQueue, &led1State, 0);
	xQueueSend(ledQueue, &led2State, 0);
}