#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "USART.h"

#define F_CPU 16000000UL
#define LED PORTD5

TimerHandle_t Timer;
void MyTimer (TimerHandle_t Timer);
void Task1 (void* pvParameters);

/*-----------------------------------------------------------------------*/

int main (void) {
	
	USART_init();
	DDRD = 0xFF;
	
	// ToDo [1]: Create a auto-reload timer with periodic ticks = 100.
	// ToDo [2]: Start the timer.
	// ToDo [3]: Create a task which controls the frequency of the timer.
	// ToDo [4]: Start the scheduler.
	
	Timer = xTimerCreate("Timer", 100, pdTRUE, NULL, MyTimer);
	xTimerStart(Timer, 100);
	
	xTaskCreate(Task1, "Control", 256, NULL, 1, NULL);
	vTaskStartScheduler();
	
	return 0;
}

/*-----------------------------------------------------------------------*/

void Task1 (void* pvParameters)
{
	char in;
	uint16_t period = 100; // Initial timer period in ticks
	
	while (1) {
		// 1. Receive a char from the user, then display it on terminal window. 
		USART_sendstr("Enter a character: ");
		in = (char) USART_receive();
		USART_send(in);
		USART_sendstr("\r\n");
		
		// 2. Implement the mentioned conditions.
		switch (in) {
			case 'd':
				period *= 2;
				xTimerChangePeriod(Timer, period, 100);
				break;
			
			case 'i':
				period /= 2;
				xTimerChangePeriod(Timer, period, 100);
				break;
			
			case 'r':
				period = 100;
				xTimerChangePeriod(Timer, period, 100);
				break;
			
			case 's':
				xTimerStop(Timer, 100);
				break;
			
			default:
				USART_sendstr("Invalid Input\r\n");
				break;
		}
	}
}

void MyTimer (TimerHandle_t Timer)
{
	// ToDo [5]: Toggle the led.
	PORTD ^= (1 << LED);
}