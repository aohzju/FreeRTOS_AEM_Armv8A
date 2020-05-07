
/* Standard includes. */
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );
static void prvTask3( void *pvParameters );

/*-----------------------------------------------------------*/
/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainTask3_PRIORITY					( tskIDLE_PRIORITY + 3 )


void demo1( void )
{
	/* Create the queue. */
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

	if( xQueue != NULL )
	{
		/* Start the two tasks as described in the comments at the top of this
		file. */
		xTaskCreate( prvQueueReceiveTask,				/* The function that implements the task. */
					"Rx", 								/* The text name assigned to the task - for debug only as it is not used by the kernel. */
					configMINIMAL_STACK_SIZE, 			/* The size of the stack to allocate to the task. */
					NULL, 								/* The parameter passed to the task - not used in this case. */
					mainQUEUE_RECEIVE_TASK_PRIORITY, 	/* The priority assigned to the task. */
					NULL );								/* The task handle is not required, so NULL is passed. */

		xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

		xTaskCreate(prvTask3, "Task3", configMINIMAL_STACK_SIZE, NULL, mainTask3_PRIORITY, NULL);

		/* Start the tasks and timer running. */
		vTaskStartScheduler();
	}

}

static void prvQueueReceiveTask( void *pvParameters )
{
	uint32_t ulReceivedValue;
	const uint32_t ulExpectedValue = 100UL;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		printf("Message received: %u\n", ulReceivedValue);
	}
}

static void prvQueueSendTask( void *pvParameters )
{
	TickType_t xNextWakeTime;
	uint32_t ulValueToSend;
	static int toggle = 0;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		ulValueToSend = toggle? 0:100;
		if(toggle)
			toggle = 0;
		else
			toggle = 1;
		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, &ulValueToSend, 0U );
	}
}

static void prvTask3( void *pvParameters )
{
	( void ) pvParameters;
	for(;;){
		printf("Task 3 running. Tick Count: %lu\n", xTaskGetTickCount());
		vTaskDelay(20);
	}
}

