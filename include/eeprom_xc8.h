/**************************
 * eeprom.h
 * Created On: 11/5/10
 * Created By: Liam Bucci
 * Project Name: EEPROM
 * Project Type: Embedded Library
 * Project Description: Embedded library for PIC MCUs which sets
 * and configures chip specific EEPROM settings as well as providing
 * functionality for reading and writing of bytes to EEPROM memory.
 * 
 * NOTES: 
 * 
 * TODO: 
 * 
 *************************/

#ifndef _EEPROM_H
#define _EEPROM_H


void eeprom_init(void);
char eeprom_read(unsigned int address, unsigned char *byte);
char eeprom_write(unsigned int address, unsigned char byte);

#endif //_EEPROM_H

