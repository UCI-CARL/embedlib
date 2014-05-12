/* -*- mode: C; tab-width: 4; -*- */
/** @file {dyndma.h} *********************************************************
 * @brief The DynDMA library provides functions to dynamically allocate and
 * free DMA memory.
 *
 * @author {Liam Bucci}
 * @date {6/10/2012}
 * @project {Embedded Library}
 * @subproject {DynDMA}
 * @version {0.0.1}
 *
 ****************************************************************************/

#include <stdlib.h>

/** Function Prototypes
 * These are the prototypes for the functions contained in the library.
 */
void * dmalloc( size_t size );
void dfree( void * block );
