/* -*- mode: C; tab-width: 4; -*- */
/** @file {kernel.h} *********************************************************
 * 
 * @brief Motor controller/driver kernel code. Run after board initialization,
 * this code ensures all modules are called as needed.
 * 
 * @createdon{11/23/2012}
 * @createdby{Liam Bucci}
 * @carlnumber{FIRM-0004}
 *
 * @version {0.3.0}
 *
 ****************************************************************************/

#ifndef _SCHEDULER_H
#define _SCHEDULER_H


/** Setup Macros
 * These macros define configuration settings and variable sizes of various
 * kernel components.
 *
 * @todo Move this section to a .def file.
 */
// Maximum number of processes that may be scheduled at one time.
#define SCHEDULE_LIST_LENGTH 16
    


/** Public Function Prototypes
 * These are the function prototypes for public functions implemented by the
 * kernel. These functions may be used outside the kernel.
 */
void init_scheduler(void);
void start_scheduler(void) __attribute__((noreturn));
int schedule(void (*func)(void *), int priority, void *params);




#endif //_SCHEDULER_H
