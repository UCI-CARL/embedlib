/* -*- mode: C; tab-width: 4; -*- */
/** @file {kernel.c} *********************************************************
 * 
 * @brief Motor controller/driver kernel code. Run after board initialization,
 * this code ensures all modules are called as needed.
 *
 * @author     Liam Bucci
 * @date       11/19/2012
 * @carlnumber FIRM-0004
 * @version    0.3.0
 *
 * @todo Change array lists to linked lists for easier sorting, etc.
 * @todo 
 ****************************************************************************/

// Standard C include files
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Microchip peripheral libraries
#include <p33Fxxxx.h>

// Include hardware definitions
#include "../def/board.def"

// Local include file
#include "../include/scheduler.h"


/* Private Function Macros
 * These are preprocessor macros that provide some functionality besides
 * storing a value.
 */


/* Private Kernel Data Structures
 * These data structures are used internally by the kernel to keep track of
 * internal states, callback functions, etc. These are only declarations only.
 */

// Process type
typedef struct process_s
{
    void (*func)(void *params);   // The kmodule to be run when when the process is executed.
    void *params;                 // A pointer to the parameters that will be
                                  // passed to the function when it is
                                  // executed.
    int priority;       // When priority is greater than zero it determines the number of ticks until
                        // it is valid to run (i.e. priority equals zero). When it is zero or less than
                        // zero the scheduler will determine run order by how negative priority is.
} process_t;

    
/* Private Kernel Data Storage
 * These data storage variables are used internnally by the kernel to keep
 * track of internal states, callback functions, etc. These are definitions
 * only.
 */
/**
 * Array storing the schedule of pointers to processes to execute.
 */
static process_t *schedule_list[SCHEDULE_LIST_LENGTH] = {0};

/**
 * Kernel tick counter
 */
static volatile unsigned long ticks = 0;

/* Private Function Prototypes
 * These functions are private and should only be used by the kernel itself.
 */
static void prioritize(); // Sort the schedule by priority
static process_t * get_scheduled(); // Get the next scheduled process
static void update_priority(); // Decrement priority values in all scheduled processes


/* Interrupt Service Routine Prototypes
 * These are the function prototypes for the interrupt handling routines used
 * by the kernel internally.
 */
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void);


/* Function Definitions
 * This is the start of the function definition code.
 */


/**
 * This function initializes the kernel data and hardware. It sets up the timer and any data needed
 * by the scheduler.
 */
void init_scheduler(void)
{
    // Initialize Timer1 for ticks
    TMR1 = 0x0000;
    PR1 = 5000L; // 500us/tick
    T1CON = 0;   // Remain paused until kernel starts
}

    
/**
 * This function starts the scheduler running. This function never returns.
 */
void start_scheduler(void)
{
    process_t *current_process = NULL;
    
    // Start ticks
    T1CON |= (1<<15);
    
    // Enable interrupts
    IEC0bits.T1IE = 1;  // Enable Timer1 interrupt
    
    //! Start endless loop
    for( ; ; )
    {
        // Prioritize schedule
        prioritize();

        // Get next item on schedule
        current_process = get_scheduled();

        // Check if next process was valid and run it
        if( current_process != NULL )
        {// Process is valid
            // Run process
            current_process->func(current_process->params);

            // Free process
            free(current_process);
        }
            
    }   
}

/**
 * This function schedules the run function of the module pointed to by @em kmodule.
 * The scheduler is a simple FIFO queue with the distinction that positive
 * priority values mean a process is paused for that number of kernel ticks.
 * Negative priority values are higher priority. This function is atomic.
 *
 * @param[in]  func
 *             A pointer to the function which should be executed when the process reaches the top
 *             of the scheduler queue.
 *
 * @param[in]  priority
 *             An integer representing when/in what order the process should be executed. Any
 *             number greater than zero will cause the process to wait for that number of ticks
 *             before entering the scheduler queue. For example if priority = 5, then 5 ticks later
 *             it will enter the queue. Any number zero or lower represents a priority in the pool
 *             of processes to be executed, where lower numbers have higher priority.
 *
 * @param[in]  params
 *             A pointer to the parameters to pass into the scheduled function when it is executed.
 *
 * @return     An integer value representing whether adding the function to the scheduler
 *             succeeded or failed. A zero means success, a negative number means failure.
 */
int schedule(void (*func)(void *), int priority, void *params)
{
    unsigned int iterator;
    
    // Disable interrupts
    __asm__ volatile ("disi #0x3FFF");
    
    // Check for valid function pointer
    if( func == NULL )
    {// Module is invalid
        // Reenable interrupts
        __asm__ volatile ("disi #0x0000");

        // Return unsuccessfully
        return false;
    }

    // Iterate through schedule and find next empty slot
    for( iterator = 0; iterator < SCHEDULE_LIST_LENGTH; ++iterator )
    {
        // Check for empty slot
        if( schedule_list[iterator] == NULL )
        {// Current slot is empty
            // Allocate new process and copy data into it
            schedule_list[iterator] = malloc(sizeof(process_t));
            schedule_list[iterator]->func = func;
            schedule_list[iterator]->params = params;
            schedule_list[iterator]->priority = priority;

            // Reenable interrupts
            __asm__ volatile ("disi #0x0000");
            
            // Return successfully
            return true;
        }
    }

    // Reenable interrupts
    __asm__ volatile ("disi #0x0000");
    
    // No empty slots found, return unsuccessfully
    //! @todo Add debug notice here
    return false;
}


void prioritize()
{
    unsigned int iterator;
    process_t *tmp_ptr;
    bool swap = false;
    
    // Disable interrupts
    __asm__ volatile ("disi #0x3FFF");

    // Check for empty schedule or only one item in schedule
    if( schedule_list[0] == NULL || schedule_list[1] == NULL )
    {// Schedule is empty
        // Reenable interrupts
        __asm__ volatile ("disi #0x0000");

        return;
    }

    // Implement reverse bubble sort, sort by iterating through the list in reverse order
    // Continue sorting as long as a swap has occurred this round
    do
    {
        // Initialize swap to false for this round
        swap = false;
        
        // Iterate through entire list from back to front
        for( iterator = SCHEDULE_LIST_LENGTH-1; iterator > 0; --iterator )
        {
            // Check that the current item is not NULL
            if( schedule_list[iterator] != NULL )
            {// Current item is not NULL
                // Check the current item against the next (closer to the front)
                if( schedule_list[iterator]->priority < schedule_list[iterator-1]->priority )
                {// Current item has higher priority than the next item
                    // Swap current item and next item
                    tmp_ptr = schedule_list[iterator];
                    schedule_list[iterator] = schedule_list[iterator-1];
                    schedule_list[iterator-1] = tmp_ptr;
                    swap = true;
                }
            }
        }
    } while(swap);

    // Reenable interrupts
    __asm__ volatile ("disi #0x0000");

    // Return
    return;
}


/**
 * Get the next scheduled process and shift the schedule array left. This
 * function is atomic.
 *
 * @return      A pointer to the next process in the schedule queue.
 */
process_t * get_scheduled()
{
    unsigned int iterator;
    process_t *next_process = NULL;
    
    // Disable interrupts
    __asm__ volatile ("disi #0x3FFF");

    // Check if schedule is empty
    if( schedule_list[0] == NULL )
    {// Schedule is empty
        // Reenable interrupts
        __asm__ volatile ("disi #0x0000");

        // Return NULL
        return NULL;
    }
    
    // Check if next process is ready to run
    if( schedule_list[0]->priority <= 0 )
    {// Store next process pointer and shift schedule left
        next_process = schedule_list[0];
        for( iterator = 0; schedule_list[iterator] != NULL && iterator < SCHEDULE_LIST_LENGTH-1; ++iterator )
        {
            // Copy the next item into the current item
            schedule_list[iterator] = schedule_list[iterator+1];
        }
        // Set last item to NULL (in case it was the last item in array)
        schedule_list[iterator] = NULL;

        // Reenable interrupts
        __asm__ volatile ("disi #0x0000");

        // Return next process
        return next_process;
    }

    // Reenable interrupts
    __asm__ volatile ("disi #0x0000");

    // Return NULL
    return NULL;
}


/**
 * Update the priority levels of all scheduled processes. Simply decrements all
 * processes priority values by one. This function is atomic.
 */
void update_priority()
{
    unsigned int iterator;
    
    // Disable interrupts
    __asm__ volatile ("disi #0x3FFF");

    // Iterate through schedule and decrement priorities
    for( iterator = 0; schedule_list[iterator] != NULL && iterator < SCHEDULE_LIST_LENGTH; ++iterator )
    {
        schedule_list[iterator]->priority--;
    }

    // Reenable interrupts
    __asm__ volatile ("disi #0x0000");

    // Return
    return;
}


/** Timer1 ISR
 * This is the Timer1 ISR. This ISR is used as the kernels tick counter.
 */
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
    // Increment kernel ticks
    //! @todo Make this atomic!
    ticks++;

    // Update schedule priorities
    update_priority();
    
    // Reset interrupt flag
    IFS0bits.T1IF=0;
}

