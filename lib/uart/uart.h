#pragma once

class UART
{
public:
    UART();

    void sendChar(char Tegn);
    void sendCommand(char *array);
};

/************************************************
 * "uart.h":                                     *
 * Header file for Mega2560 UART driver. *
 * Using UART 0.                                 *
 * Henning Hargaard, 16/11 2022                  *
 *************************************************/
#include <avr/io.h>
#include <stdlib.h>

void InitUART(unsigned long BaudRate, unsigned char DataBit);
void SendChar(const char Tegn);
void SendString(const char *Streng);
unsigned char CharReady();
char ReadChar();
void SendInteger(int Tal);