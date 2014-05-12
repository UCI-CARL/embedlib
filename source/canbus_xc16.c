/* -*- mode: C; tab-width: 4; -*- */
/** @file {canbus_module.c} **************************************************
 * @brief This file contains all the function definitions and private
 * variables for the CAN bus module.
 *
 * @author {Liam Bucci}
 * @date {6/11/2012}
 * @project {Carl Platform}
 * @subproject {Motor Controller}
 * @version {0.2.0}
 *
 ****************************************************************************/

// Standard C include files
#include <stdlib.h>
#include <stdbool.h>

// Microchip peripheral libraries
#include <p33Fxxxx.h>
#include <pps.h>
#include <ecan.h>
#include <dma.h>

// CarlLib files
//#include <CarlComm>
//#include <CarlMessage>


// Module include files
#include "../include/can_module.h"

/** Private Module Enums
 * Private enumerations used for setting up the CAN bus module.
 */
enum can_offset_e
{
    CAN_OFFSET_CiCTRL1    = 0x0000,
    CAN_OFFSET_CiCTRL2    = 0x0001,
    CAN_OFFSET_CiVEC      = 0x0002,
    CAN_OFFSET_CiFCTRL    = 0x0003,
    CAN_OFFSET_CiFIFO     = 0x0004,
    CAN_OFFSET_CiINTF     = 0x0005,
    CAN_OFFSET_CiINTE     = 0x0006,
    CAN_OFFSET_CiEC       = 0x0007,
    CAN_OFFSET_CiCFG1     = 0x0008,
    CAN_OFFSET_CiCFG2     = 0x0009,
    CAN_OFFSET_CiFEN1     = 0x000A,
    CAN_OFFSET_CiFMSKSEL1 = 0x000B,
    CAN_OFFSET_CiFMSKSEL2 = 0x000C,

    CAN_OFFSET_CiRXFUL1   = 0x0010,
    CAN_OFFSET_CiRXFUL2   = 0x0011,
    CAN_OFFSET_CiRXOVF1   = 0x0014,
    CAN_OFFSET_CiRXOVF2   = 0x0015,
    CAN_OFFSET_CiTR01CON  = 0x0018,
    CAN_OFFSET_CiTR23CON  = 0x0019,
    CAN_OFFSET_CiTR45CON  = 0x001A,
    CAN_OFFSET_CiTR67CON  = 0x001B,
    CAN_OFFSET_CiRXD      = 0x0020,
    CAN_OFFSET_CiTXD      = 0x0021,

    CAN_OFFSET_CiBUFPNT1  = 0x0010,
    CAN_OFFSET_CiBUFPNT2  = 0x0011,
    CAN_OFFSET_CiBUFPNT3  = 0x0012,
    CAN_OFFSET_CiBUFPNT4  = 0x0013,
    CAN_OFFSET_CiRXM0SID  = 0x0018,
    CAN_OFFSET_CiRXM0EID  = 0x0019,
    CAN_OFFSET_CiRXM1SID  = 0x001A,
    CAN_OFFSET_CiRXM1EID  = 0x001B,
    CAN_OFFSET_CiRXM2SID  = 0x001C,
    CAN_OFFSET_CiRXM2EID  = 0x001D,
    CAN_OFFSET_CiRXF0SID  = 0x0020,
    CAN_OFFSET_CiRXF0EID  = 0x0021,
    CAN_OFFSET_CiRXF1SID  = 0x0022,
    CAN_OFFSET_CiRXF1EID  = 0x0023,
    CAN_OFFSET_CiRXF2SID  = 0x0024,
    CAN_OFFSET_CiRXF2EID  = 0x0025,
    CAN_OFFSET_CiRXF3SID  = 0x0026,
    CAN_OFFSET_CiRXF3EID  = 0x0027,
    CAN_OFFSET_CiRXF4SID  = 0x0028,
    CAN_OFFSET_CiRXF4EID  = 0x0029,
    CAN_OFFSET_CiRXF5SID  = 0x002A,
    CAN_OFFSET_CiRXF5EID  = 0x002B,
    CAN_OFFSET_CiRXF6SID  = 0x002C,
    CAN_OFFSET_CiRXF6EID  = 0x002D,
    CAN_OFFSET_CiRXF7SID  = 0x002E,
    CAN_OFFSET_CiRXF7EID  = 0x002F,
    CAN_OFFSET_CiRXF8SID  = 0x0030,
    CAN_OFFSET_CiRXF8EID  = 0x0031,
    CAN_OFFSET_CiRXF9SID  = 0x0032,
    CAN_OFFSET_CiRXF9EID  = 0x0033,
    CAN_OFFSET_CiRXF10SID = 0x0034,
    CAN_OFFSET_CiRXF10EID = 0x0035,
    CAN_OFFSET_CiRXF11SID = 0x0036,
    CAN_OFFSET_CiRXF11EID = 0x0037,
    CAN_OFFSET_CiRXF12SID = 0x0038,
    CAN_OFFSET_CiRXF12EID = 0x0039,
    CAN_OFFSET_CiRXF13SID = 0x003A,
    CAN_OFFSET_CiRXF13EID = 0x003B,
    CAN_OFFSET_CiRXF14SID = 0x003C,
    CAN_OFFSET_CiRXF14EID = 0x003D,
    CAN_OFFSET_CiRXF15SID = 0x003E,
    CAN_OFFSET_CiRXF15EID = 0x003F
};


/** Private Module Data
 * This data is private to the module and cannot be accessed by outside
 * functions and modules.
 */

/** Default Mask and Filter
 * The carlcomm_mask and carlcomm_filter are the default mask and filter
 * which will accept any carlmessages that are destined for this board
 * address. Other masks and filters may be added to listen to bus traffic,
 * but this mask/filter combination should always be enabled.
 */
//! The carlcomm_mask masks out everything except the destination board
//! address from a carlmessage. The masking is done by the can hardware.
static const carlmessage_header_t carlcomm_mask = { .destination_part = {0x00, 0x1F} };

//! The carlcomm_filter filters any carlmessages that do not have our local
//! board address. The filtering is done by the can hardware.
static const carlmessage_header_t carlcomm_filter = { .destination_part = {0x00, BOARD_ADDRESS_MOTORCONTROLLER} };


/** Module Function Prototypes
 */
int can_init(can_module_t *module);
int can_run(can_module_t *module);
//void can_callback(void *module, carlmessage_t *message);
int can_open(can_module_t *module, unsigned int mode);
int can_write(can_module_t *module, const carlmessage_t *message);
int can_read(can_module_t *module, carlmessage_t *message);
int can_close(can_module_t *module);
int can_flush_tx(can_module_t *module);
int can_flush_rx(can_module_t *module);
int can_set_bit_timing(can_module_t *module, uint32_t settings);
int can_set_mask(can_module_t *module, unsigned int mask_num, carlmessage_header_t mask_val);
int can_set_filter(can_module_t *module, unsigned int filter_num, carlmessage_header_t filter_val, unsigned int mask_num);
int can_enable_filter(can_module_t *module, unsigned int filter_num);
int can_disable_filter(can_module_t *module, unsigned int filter_num);

/** Module Constructor */
can_module_t * new_can_module()
{
    can_module_t *new_module = malloc( sizeof(can_module_t) );

    new_module->run_period = 3;
    new_module->can_base_address = NULL;
    new_module->tx_dma_base_address = NULL;
    new_module->rx_dma_base_address = NULL;
    new_module->bit_timing = 0x0000;
    new_module->open_mode = 0x0001;
    new_module->can_buffer = NULL;
    new_module->tx_locks = 0x00;
    new_module->tx_ready = 0x00;
    new_module->rx_lock = false;
    new_module->init = &can_init;
    new_module->run = &can_run;
    new_module->open = &can_open;
    new_module->write = &can_write;
    new_module->read = &can_read;
    new_module->close = &can_close;
    new_module->flush_tx = &can_flush_tx;
    new_module->flush_rx = &can_flush_rx;
    new_module->set_bit_timing = &can_set_bit_timing;
    new_module->set_mask = &can_set_mask;
    new_module->set_filter = &can_set_filter;
    new_module->enable_filter = &can_enable_filter;
    new_module->disable_filter = &can_disable_filter;

    return new_module;
}

/** Module Destructor */
void delete_can_module(can_module_t *module)
{
    // Clean up module
    module->close(module);
    
    free(module);
}

int can_init(can_module_t *module)
{
    //! @TODO Allow for partial init (only RX or only TX).
    //! @TODO Move PPS settings to kernel.

    // Set up PPS
    PPSUnLock;
    PPSOutput(PPS_C1TX,PPS_RP9);
    PPSInput(PPS_C1RX,PPS_RP8);
    PPSLock;

    // Check for valid can_base_address
    if( module->can_base_address == NULL )
    {
        return -1;
    }

    // Check for valid tx_dma_base_address
    if( module->tx_dma_base_address == NULL )
    {
        return -1;
    }

    // Check for valid rx_dma_base_address
    if( module->rx_dma_base_address == NULL )
    {
        return -1;
    }

    // Check for valid can_buffer
    if( module->can_buffer == NULL )
    {
        return -1;
    }
    
    // Switch to configuration mode
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) = (*(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) & 0xF8FF) | (CAN_MODE_CONFIG<<8);
    while( (*(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) & 0x00E0) != (CAN_MODE_CONFIG)<<5 );
    module->open_mode = CAN_MODE_CONFIG;
    
    // Set up bit timing
    *(module->can_base_address + CAN_OFFSET_CiCFG1_OFFSET) = (module->bit_timing&(SYNC_JUMP_MASK | BAUD_PRE_MASK));
    *(module->can_base_address + CAN_OFFSET_CiCFG2_OFFSET) = ((module->bit_timing&ENABLE_WAKE)>>5) | (module->bit_timing&PHASE_SEG2_MASK)>>6 | ((module->bit_timing&ENABLE_PHASE_SEG2_PROG)>>10) | ((module->bit_timing&TRIPLE_SAMPLE)>>12) | (module->bit_timing&(PHASE_SEG1_MASK | PROP_SEG_MASK))>>8;
    
    // Switch to filter window
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) |= 0x0001;
    
    // Disable all filters by default
    *(module->can_base_address + CAN_OFFSET_CiFEN1_OFFSET) = 0x0000;
    
    // Set up filter buffer pointers
    *(module->can_base_address + CAN_OFFSET_CiBUFPNT1_OFFSET) = 0xFFFF;
    *(module->can_base_address + CAN_OFFSET_CiBUFPNT2_OFFSET) = 0xFFFF;
    *(module->can_base_address + CAN_OFFSET_CiBUFPNT3_OFFSET) = 0xFFFF;
    *(module->can_base_address + CAN_OFFSET_CiBUFPNT4_OFFSET) = 0xFFFF;
    
    // Switch to buffer window
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
    
    // Set up TX buffers
    // TR0: Highest Priority
    // TR1: Highest Priority
    // TR2: High Priority
    // TR3: High Priority
    // TR4: Low Priority
    // TR5: Low Priority
    // TR6: Lowest Priority
    // TR7: Lowest Priority
    *(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET) = (1<<15) | (0b11<<8) | (1<<7) | (0b11);
    *(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET) = (1<<15) | (0b10<<8) | (1<<7) | (0b10);
    *(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET) = (1<<15) | (0b01<<8) | (1<<7) | (0b01);
    *(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET) = (1<<15) | (0b00<<8) | (1<<7) | (0b00);
    
    // Set up RX FIFO buffer
    // Start buffer: 8
    // Number of buffers: 24
    *(module->can_base_address + CAN_OFFSET_CiFCTRL_OFFSET) = (0xA008);
    
    // Set up CAN interrupts
    //! @TODO Write error handling functions.
    //*(module->can_base_address + CAN_OFFSET_CiINTE_OFFSET) |= (0x0003);
    
    //! @TODO Change CAN mode to disable
    
    // Set up DMA channel for TX
    *(module->tx_dma_base_address + DMAxCON_OFFSET) = 0x2020;
    *(module->tx_dma_base_address + DMAxREQ_OFFSET) = 0x0046;
    *(module->tx_dma_base_address + DMAxSTA_OFFSET) = (unsigned int)module->can_buffer - (unsigned int)&_DMA_BASE;
    *(module->tx_dma_base_address + DMAxPAD_OFFSET) = (volatile unsigned int)(module->can_base_address + CAN_OFFSET_CiTXD_OFFSET);
    *(module->tx_dma_base_address + DMAxCNT_OFFSET) = 0x0007;
    
    // Set up DMA channel for RX
    *(module->rx_dma_base_address + DMAxCON_OFFSET) = 0x0020;
    *(module->rx_dma_base_address + DMAxREQ_OFFSET) = 0x0022;
    *(module->rx_dma_base_address + DMAxSTA_OFFSET) = (unsigned int)module->can_buffer - (unsigned int)&_DMA_BASE;
    *(module->rx_dma_base_address + DMAxPAD_OFFSET) = (volatile unsigned int)(module->can_base_address + CAN_OFFSET_CiRXD_OFFSET);
    *(module->rx_dma_base_address + DMAxCNT_OFFSET) = 0x0007;

    return 0;
}

int can_run(can_module_t *module)
{
    return module->flush_tx(module);
}

int can_open(can_module_t *module, unsigned int mode)
{
    //! @TODO Move open_mode below setting the CAN mode
    // Set open_mode
    module->open_mode = mode;
    
    // Enable TX DMA channel
    if( (module->open_mode & WRITE) == WRITE )
    {
        *(module->tx_dma_base_address + DMAxCON_OFFSET) |= 0x8000;
    }
    
    // Enable RX DMA channel
    if( (module->open_mode & READ) == READ )
    {
        *(module->rx_dma_base_address + DMAxCON_OFFSET) |= 0x8000;
    }

    // Set default mask
    if( module->set_mask(module, 0, carlcomm_mask) == -1 )
    {// Setting default mask failed
        return -1;
    }

    // Set default filter
    if( module->set_filter(module, 0, carlcomm_filter, 0) == -1 )
    {// Setting default filter failed
        return -1;
    }

    // Enable default filter
    if( module->enable_filter(module, 0) == -1 )
    {// Enabling default filter failed
        return -1;
    }
    
    // Set CAN mode
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) = (*(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) & 0xF8FF) | ((module->open_mode & 0x0007)<<8);
    while( (*(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) & 0x00E0) != (module->open_mode & 0x0007)<<5 );

    return 0;
}

int can_write(can_module_t *module, const carlmessage_t *message)
{
    volatile unsigned int *write_ptr;
    unsigned int write_buffer_num;
    
    if( module == NULL)
    {
        return -1;
    }

    if( message == NULL )
    {
        return module->tx_ready;
    }

    if( (module->open_mode&WRITE) != WRITE )
    {
        return -1;
    }

    // If buffer is full, flush and continue
    if( module->tx_ready == (0xFF>>(8-CAN_TX_BUFFER_LENGTH)) )
    {
        if( module->flush_tx(module) == -1 )
        {
            return -1;
        }
    }

    // Find next empty, unlocked buffer that is not currently being sent
    if( !(module->tx_ready&0x01) && !(module->tx_locks&0x01) && !(*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x0008) )
    {// Buffer TR0 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x01;

        // Set write_ptr
        write_ptr = module->can_buffer;

        // Store write_buffer_num for later
        write_buffer_num = 0;
    }
    else if( !(module->tx_ready&0x02) && !(module->tx_locks&0x02) && !(*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x0800) )
    {// Buffer TR1 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x02;

        // Set write_ptr
        write_ptr = module->can_buffer + 8;

        // Store write_buffer_num for later
        write_buffer_num = 1;
    }
    else if( !(module->tx_ready&0x04) && !(module->tx_locks&0x04) && !(*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x0008) )
    {// Buffer TR2 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x04;

        // Set write_ptr
        write_ptr = module->can_buffer + 16;

        // Store write_buffer_num for later
        write_buffer_num = 2;
    }
    else if( !(module->tx_ready&0x08) && !(module->tx_locks&0x08) && !(*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x0800) )
    {// Buffer TR3 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x08;

        // Set write_ptr
        write_ptr = module->can_buffer + 24;

        // Store write_buffer_num for later
        write_buffer_num = 3;
    }
    else if( !(module->tx_ready&0x10) && !(module->tx_locks&0x10) && !(*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x0008) )
    {// Buffer TR4 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x10;

        // Set write_ptr
        write_ptr = module->can_buffer + 32;

        // Store write_buffer_num for later
        write_buffer_num = 4;
    }
    else if( !(module->tx_ready&0x20) && !(module->tx_locks&0x20) && !(*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x0800) )
    {// Buffer TR5 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x10;

        // Set write_ptr
        write_ptr = module->can_buffer + 40;

        // Store write_buffer_num for later
        write_buffer_num = 5;
    }
    else if( !(module->tx_ready&0x40) && !(module->tx_locks&0x40) && !(*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x0008) )
    {// Buffer TR6 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x10;

        // Set write_ptr
        write_ptr = module->can_buffer + 48;

        // Store write_buffer_num for later
        write_buffer_num = 6;
    }
    else if( !(module->tx_ready&0x80) && !(module->tx_locks&0x80) && !(*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x0800) )
    {// Buffer TR7 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x10;

        // Set write_ptr
        write_ptr = module->can_buffer + 56;

        // Store write_buffer_num for later
        write_buffer_num = 7;
    }
    else
    {// No buffers available to write
        return -1;
    }
    
        
    // Write carlmessage to buffer

    // Always an extended id (ide = 1)
    *write_ptr = (message->header.id.standard_id<<2) | (1<<1) | (1);
    *(write_ptr+1) = (message->header.id.extended_id>>6);
    *(write_ptr+2) = (message->header.id.extended_id<<10) | (message->dlc);
    if( message->rtr )
    {// Set RTR flag
        *(write_ptr+2) |= (1<<9);
    }
    else
    {// Clear RTR flag
        *(write_ptr+2) &= ~(1<<9);
    }

    // Copy data to buffer
    *(write_ptr+3) = (message->data[1]<<8) | (message->data[0]);
    *(write_ptr+4) = (message->data[3]<<8) | (message->data[2]);
    *(write_ptr+5) = (message->data[5]<<8) | (message->data[4]);
    *(write_ptr+6) = (message->data[7]<<8) | (message->data[6]);

    // Set tx_ready bit
    module->tx_ready |= 1<<(write_buffer_num);

    // Release buffer lock
    module->tx_locks &= ~(1<<(write_buffer_num));

    return write_buffer_num;
}


int can_read(can_module_t *module, carlmessage_t *message)
{
    unsigned int read_buffer_num;
    volatile unsigned int *read_ptr = NULL;

    // Check for valid module pointer
    if( module == NULL )
    {
        return -1;
    }

    // Check for valid message pointer
    if( message == NULL )
    {
        //! @TODO Return number of messages in buffer
        return 0;
    }

    // Check for correct open_mode
    if( ((module->open_mode&READ) != READ) || ((module->open_mode&0x0007) == CAN_MODE_DISABLE) || ((module->open_mode&0x0007) == CAN_MODE_CONFIG) )
    {
        return -1;
    }

    // Check for available rx_lock
    if( module->rx_lock )
    {
        return -1;
    }

    // Get rx_lock
    module->rx_lock = true;

    // Check for message in next read buffer
    read_buffer_num = (*(module->can_base_address + CAN_OFFSET_CiFIFO_OFFSET)&0x003F);
    if( read_buffer_num < 16 )
    {// Use CAN_OFFSET_CiRXFUL1
        if( *(module->can_base_address + CAN_OFFSET_CiRXFUL1_OFFSET)&(1<<read_buffer_num) )
        {// A new message exists
            // Set read_ptr to start of can buffer + read_buffer_num*8
            read_ptr = module->can_buffer + read_buffer_num*8;
        }
    }
    else if( read_buffer_num < 32 )
    {// Use CAN_OFFSET_CiRXFUL2
        if( *(module->can_base_address + CAN_OFFSET_CiRXFUL2_OFFSET)&(1<<(read_buffer_num-16)) )
        {// A new message exists
            // Set read_ptr to start of can buffer + read_buffer_num*8
            read_ptr = module->can_buffer + read_buffer_num*8;
        }
    }
    else
    {// Unknown value of read_buffer_num!
        // Release lock and return error
        module->rx_lock = false;
        return -1;
    }

    // If read_ptr is not NULL a new message exists
    if( read_ptr != NULL )
    {// A new message exists and read_ptr points to it

        // Zero message header (since some garbage bits remain)
        message->header.uint32 = 0x00000000;
        
        // Copy buffer into message
        
        // Should always be an extended id
        message->header.id.standard_id = (*read_ptr)>>2;
        message->header.id.extended_id = (*(read_ptr+1))<<6 | (*(read_ptr+2))>>10;
        if( (*(read_ptr+2))&0x0200 )
        {// Set rtr flag
            message->rtr = true;
        }
        else
        {// Clear rtr flag
            message->rtr = false;
        }
        
        message->dlc = (*(read_ptr+2))&0x000F;
        message->data[0] = (*(read_ptr+3))&0x00FF;
        message->data[1] = ((*(read_ptr+3))&0xFF00)>>8;
        message->data[2] = (*(read_ptr+4))&0x00FF;
        message->data[3] = ((*(read_ptr+4))&0xFF00)>>8;
        message->data[4] = (*(read_ptr+5))&0x00FF;
        message->data[5] = ((*(read_ptr+5))&0xFF00)>>8;
        message->data[6] = (*(read_ptr+6))&0x00FF;
        message->data[7] = ((*(read_ptr+6))&0xFF00)>>8;

        if( read_buffer_num < 16 )
        {// Use CAN_OFFSET_CiRXFUL1
            // Clear RXFUL bit
            *(module->can_base_address + CAN_OFFSET_CiRXFUL1_OFFSET) &= ~(1<<read_buffer_num);
        }
        else if( read_buffer_num < 32 )
        {// Use CAN_OFFSET_CiRXFUL2
            // Clear RXFUL bit
            *(module->can_base_address + CAN_OFFSET_CiRXFUL2_OFFSET) &= ~(1<<(read_buffer_num-16));
        }
        else
        {// Shouldn't ever happen!
            module->rx_lock = false;
            return -1;
        }
        
        // Release lock
        module->rx_lock = false;

        // Return the filter number that accepted the message
        return *(read_ptr+7)>>8;
    }
    else
    {// No new messages received

        // Release rx_lock
        module->rx_lock = false;
        
        // No messages received, so return a value not corresponding to a filter number
        return 32;
    }
}

int can_close(can_module_t *module)
{
    int return_val = 0;
    // Flush all remaining messages
    if( module->flush_tx(module) == -1 )
    {
        return_val = -1;
    }
    
    // Set open_mode to CAN_MODE_DISABLE | !READ | !WRITE
    module->open_mode &= ~(READ | WRITE | 0x0007);
    module->open_mode |= CAN_MODE_DISABLE;
    
    // Disable TX DMA channel
    *(module->tx_dma_base_address + DMAxCON_OFFSET) &= 0x7FFF;
    
    // Disable RX DMA channel
    *(module->rx_dma_base_address + DMAxCON_OFFSET) &= 0x7FFF;
    
    // Set CAN mode
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) = (*(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) & 0xF8FF) | ((module->open_mode & 0x0007)<<8);
    while( (*(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) & 0x00E0) != (module->open_mode & 0x0007)<<5 );

    return return_val;
}

int can_flush_tx(can_module_t *module)
{
    //! @TODO Add errno type error reporting scheme.
    int return_value = 0;
    
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

#if (CAN_TX_BUFFER_LENGTH > 0)
    // Check status of TR0
    if( (*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x0008) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x0040) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x0020) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x0010) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x01) && !(module->tx_locks&0x01) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET) |= 0x0008;

            // Clear tx_ready
            module->tx_ready &= ~(0x01);
        }
    }
#endif
    
#if (CAN_TX_BUFFER_LENGTH > 1)
    // Check status of TR1
    if( (*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x0800) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x4000) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x2000) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET)&0x1000) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x02) && !(module->tx_locks&0x02) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CAN_OFFSET_CiTR01CON_OFFSET) |= 0x0800;

            // Clear tx_ready
            module->tx_ready &= ~(0x02);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 2)
    // Check status of TR2
    if( (*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x0008) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x0040) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x0020) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x0010) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x04) && !(module->tx_locks&0x04) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET) |= 0x0008;

            // Clear tx_ready
            module->tx_ready &= ~(0x04);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 3)
    // Check status of TR3
    if( (*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x0800) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x4000) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x2000) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET)&0x1000) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x08) && !(module->tx_locks&0x08) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CAN_OFFSET_CiTR23CON_OFFSET) |= 0x0800;

            // Clear tx_ready
            module->tx_ready &= ~(0x08);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 4)
    // Check status of TR4
    if( (*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x0008) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x0040) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x0020) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x0010) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x10) && !(module->tx_locks&0x10) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET) |= 0x0008;

            // Clear tx_ready
            module->tx_ready &= ~(0x10);
        }
    }
#endif
    
#if (CAN_TX_BUFFER_LENGTH > 5)
    // Check status of TR5
    if( (*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x0800) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x4000) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x2000) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET)&0x1000) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x20) && !(module->tx_locks&0x20) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CAN_OFFSET_CiTR45CON_OFFSET) |= 0x0800;

            // Clear tx_ready
            module->tx_ready &= ~(0x20);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 6)
    // Check status of TR6
    if( (*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x0008) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x0040) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x0020) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x0010) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x40) && !(module->tx_locks&0x40) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET) |= 0x0008;

            // Clear tx_ready
            module->tx_ready &= ~(0x40);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 7)
    // Check status of TR7
    if( (*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x0800) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x4000) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x2000) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET)&0x1000) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x80) && !(module->tx_locks&0x80) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CAN_OFFSET_CiTR67CON_OFFSET) |= 0x0800;

            // Clear tx_ready
            module->tx_ready &= ~(0x80);
        }
    }
#endif

    return return_value;
}

int can_flush_rx(can_module_t *module)
{
    // The hardware FIFO takes care of the RX buffer
    
    return 0;
}

int can_set_bit_timing(can_module_t *module, uint32_t bit_timing)
{
    if( module == NULL )
    {
        return -1;
    }

    module->bit_timing = bit_timing;
    
    return 0;
}

int can_set_mask(can_module_t *module, unsigned int mask_num, carlmessage_header_t mask_val)
{
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

    // Only modify masks if module is not readable or writable
    //! @TODO Add module lock so the mask modification is atomic.
    if( module->open_mode&READ == READ || module->open_mode&WRITE == WRITE )
    {
        return -1;
    }

    // Check for valid mask number
    if( mask_num > 2 )
    {
        return -1;
    }

    //! The module should normally be in buffer window
    // Set filter window
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) |= 0x0001;

    switch(mask_num)
    {
    case 0:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXM0SID_OFFSET) = (mask_val.id.standard_id)<<5 | (1<<3) | (mask_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXM0EID_OFFSET) = (mask_val.id.extended_id&0x0FFFF);
        break;
    case 1:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXM1SID_OFFSET) = (mask_val.id.standard_id)<<5 | (1<<3) | (mask_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXM1EID_OFFSET) = (mask_val.id.extended_id&0x0FFFF);
        break;
    case 2:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXM2SID_OFFSET) = (mask_val.id.standard_id)<<5 | (1<<3) | (mask_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXM2EID_OFFSET) = (mask_val.id.extended_id&0x0FFFF);
        break;
    default:
        // Return to buffer window
        *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
        return -1;
    }

    // Return to buffer window
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
    
    return 0;
}

int can_set_filter(can_module_t *module, unsigned int filter_num, carlmessage_header_t filter_val, unsigned int mask_num)
{
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

    // Only modify filters if module is not readable or writable
    //! @TODO Add module lock so the filter modification is atomic.
    if( module->open_mode&READ == READ || module->open_mode&WRITE == WRITE )
    {
        return -1;
    }

    // Check for valid filter and mask numbers
    if( filter_num > 15 || mask_num > 2 )
    {
        return -1;
    }

    //! The module should normally be in buffer window
    // Set filter window
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) |= 0x0001;

    switch(filter_num)
    {
    case 0:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF0SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF0EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num);
        break;
    case 1:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF1SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF1EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<2);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<2);
        break;
    case 2:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF2SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF2EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<4);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<4);
        break;
    case 3:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF3SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF3EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<6);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<6);
        break;
    case 4:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF4SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF4EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<8);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<8);
        break;
    case 5:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF5SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF5EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<10);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<10);
        break;
    case 6:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF6SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF6EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<12);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<12);
        break;
    case 7:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF7SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF7EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<14);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<14);
        break;
    case 8:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF8SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF8EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num);
        break;
    case 9:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF9SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF9EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<2);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<2);
        break;
    case 10:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF10SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF10EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<4);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<4);
        break;
    case 11:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF11SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF11EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<6);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<6);
        break;
    case 12:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF12SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF12EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<8);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<8);
        break;
    case 13:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF13SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF13EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<10);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<10);
        break;
    case 14:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF14SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF14EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<12);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<12);
        break;
    case 15:
        // Always use extended id
        *(module->can_base_address + CAN_OFFSET_CiRXF15SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CAN_OFFSET_CiRXF15EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<14);
        *(module->can_base_address + CAN_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<14);
        break;
    default:
        // Switch to buffer window
        *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
        return -1;
    }

    // Switch to buffer window
    *(module->can_base_address + CAN_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
    
    return 0;
}

int can_enable_filter(can_module_t *module, unsigned int filter_num)
{
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

    // Check for valid filter number
    if( filter_num > 15 )
    {
        return -1;
    }

    *(module->can_base_address + CAN_OFFSET_CiFEN1_OFFSET) |= (1<<filter_num);

    return 0;
}

int can_disable_filter(can_module_t *module, unsigned int filter_num)
{
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

    // Check for valid filter number
    if( filter_num > 15 )
    {
        return -1;
    }

    *(module->can_base_address + CAN_OFFSET_CiFEN1_OFFSET) &= ~(1<<filter_num);

    return 0;
}
