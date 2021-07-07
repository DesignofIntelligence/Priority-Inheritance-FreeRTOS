/*
    FreeRTOS V6.1.0 - Copyright (C) 2010 Real Time Engineers Ltd.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/* Library includes. */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Stellaris library includes. */
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_sysctl.h"

/* Dimensions the buffer into which messages destined for stdout are placed. */
#define mainMAX_MSG_LEN	( 80 )
#define HIGHESTPRIORITYTASK 3

/* The task to be created.  Two instances of this task are created. */
static void prvPrintTask( void *pvParameters );
static void periodic( void *pvParameters );

/* The function that uses a mutex to control access to standard out. */
static void prvNewPrintString( const portCHAR *pcString );

/*-----------------------------------------------------------*/

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
mutex type semaphore that is used to ensure mutual exclusive access to stdout. */
xSemaphoreHandle xMutex;
xTaskHandle xTaskHigh;
xTaskHandle xTaskLow;
int main( void )
{
  xMutex = xSemaphoreCreateMutex();
	if( xMutex != NULL )
	{
		xTaskCreate( prvPrintTask, "Low", 240, "Low  \n", 1, xTaskLow );
		xTaskCreate( periodic, "Medium", 240, "Medium \n", 2, NULL );
		xTaskCreate( prvPrintTask, "High", 240, "High \n", HIGHESTPRIORITYTASK, xTaskHigh );
		vTaskStartScheduler();
	}
}
static void prvNewPrintString( const portCHAR *pcString )
{
static char cBuffer[ mainMAX_MSG_LEN ];
int i,j;	
	
	xSemaphoreTake( xMutex, portMAX_DELAY );
	printf(pcString);
	{
		sprintf( cBuffer, "%s", pcString );
		for (i=1;i<1000000;i++)
		{
			j++;
		}
		printf( cBuffer );
	}

	xSemaphoreGive( xMutex );
}

static void prvPrintTask( void *pvParameters )
{
	char *pcStringToPrint;
	int i,j;
	unsigned portBASE_TYPE uxPriority;
	static unsigned portBASE_TYPE uxPriorityHigh; 
	uxPriorityHigh= HIGHESTPRIORITYTASK;
	pcStringToPrint = ( char * ) pvParameters;
	for( ;; )
	{
		printf("Entered the task of priority ");
		printf(pcStringToPrint);

		// increase the priority if thats the low task


		uxPriority = uxTaskPriorityGet(NULL);

		if (uxPriority != uxPriorityHigh) //if current task priority isnt equal to highest task priority
		{
			// we are in the low priority task
			vTaskPrioritySet(NULL,HIGHESTPRIORITYTASK); //set the priority to the priority of the highest task.

		}
		// Enter critical section (mutex lock)
		prvNewPrintString( pcStringToPrint );
		// Exit lock
		// decrease the priority again
		vTaskPrioritySet(NULL, uxPriority);
		
		vTaskDelay( 255 );


	}
}
static void periodic( void *pvParameters )
{
	for(;;)
	{
		printf("Medium \n");
		vTaskDelay( 10 );
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining - and
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will only be called if a task overflows its stack.  Note
	that stack overflow checking does slow down the context switch
	implementation and will only be performed if configCHECK_FOR_STACK_OVERFLOW
	is set to either 1 or 2 in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* This example does not use the idle hook to perform any processing.  The
	idle hook will only be called if configUSE_IDLE_HOOK is set to 1 in 
	FreeRTOSConfig.h. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This example does not use the tick hook to perform any processing.   The
	tick hook will only be called if configUSE_TICK_HOOK is set to 1 in
	FreeRTOSConfig.h. */
}


