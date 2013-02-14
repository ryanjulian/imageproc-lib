/**
 * Copyright (c) 2011-2013, Regents of the University of California
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the University of California, Berkeley nor the names
 *   of its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * High Level Wireless Communications Driver
 *
 * by Humphrey Hu
 *
 * v.0.5
 */

#ifndef __RADIO_H
#define __RADIO_H

#include "mac_packet.h"

// Radio interrupt flags
typedef enum {
    RADIO_TX_SUCCESS,   // Transmission success
    RADIO_TX_FAILURE,   // Transmission failure
    RADIO_RX_START,     // Beginning of reception
    RADIO_RX_SUCCESS,   // Reception success
    RADIO_RX_FAILURE,   // Reception failure
    RADIO_HW_FAILURE,   // Hardware failure
} RadioIrqSrc;

// Represent the current radio state and operation
typedef enum {
    STATE_SLEEP = 0,        // Radio powered down (currently not implemented)
    STATE_OFF,              // Transceiver off
    STATE_IDLE,             // Transceiver off, PLL on
    STATE_TRANSITIONING,    // Radio in state transition
    STATE_RX_IDLE,          // Radio in RX mode, listening
    STATE_RX_BUSY,          // Radio in RX mode, receiving
    STATE_TX_IDLE,          // Radio in TX mode, idling
    STATE_TX_BUSY,          // Radio in TX mode, transmitting or preparing
} RadioState;

typedef struct {
    unsigned int address;   // (2)
    unsigned int pan_id;    // (2)
    unsigned char channel;  // (1)
} RadioAddress;             // Total: (5)

typedef struct {
    RadioAddress address;           // (5)
    unsigned char soft_retries;     // (1)
    unsigned char hard_retries;     // (1)
    unsigned char watchdog_running; // (1)
    unsigned long watchdog_timeout; // (4)
} RadioConfiguration;               // Total: (12)

typedef struct {
    RadioState state;               // (2)
    unsigned int packets_sent;      // (2)
    unsigned int packets_received;  // (2)
    unsigned char sequence_number;  // (1)
    unsigned char retry_number;     // (1)
    unsigned char last_rssi;        // (1)
    unsigned char last_ed;          // (1)
    unsigned long last_calibration; // (4)
    unsigned long last_progress;    // (4)
} RadioStatus;                      // Total: (18)

// Setup and initialization
void radioInit(unsigned int tx_queue_length, unsigned int rx_queue_length,
    unsigned char cs);

// Configuration methods
void radioConfigure(RadioConfiguration *conf);
void radioSetAddress(RadioAddress *address);
void radioSetSrcAddr(unsigned int src_addr);
void radioSetSrcPanID(unsigned int src_pan_id);
void radioSetChannel(unsigned char channel);
void radioSetSoftRetries(unsigned char retries);
void radioSetHardRetries(unsigned char retries);

void radioGetConfiguration(RadioConfiguration *conf);
void radioGetStatus(RadioStatus *status);

void radioSetWatchdogState(unsigned char state);
void radioEnableWatchdog(void);
void radioDisableWatchdog(void);
void radioSetWatchdogTime(unsigned int time);

// Queue interface
unsigned int radioEnqueueTxPacket(MacPacket packet);
MacPacket radioDequeueRxPacket(void);

unsigned char radioSendData (unsigned int dest_addr, unsigned char status,
                             unsigned char type, unsigned int datalen,
                             unsigned char* dataptr, unsigned char fast_fail);

unsigned int radioTxQueueEmpty(void);
unsigned int radioTxQueueFull(void);
unsigned int radioGetTxQueueSize(void);
unsigned int radioRxQueueEmpty(void);
unsigned int radioRxQueueFull(void);
unsigned int radioGetRxQueueSize(void);
// Clear all packets off of queue
void radioFlushQueues(void);

// Processes queues and internals
// Should be called regularly
void radioProcess(void);

// Object access/creation
// Request a packet + payload from the preinitialized pool
MacPacket radioRequestPacket(unsigned int data_size);
// Return a packet + payload to the preinitialized pool
unsigned int radioReturnPacket(MacPacket packet);

MacPacket __attribute__ ((deprecated)) radioCreatePacket(unsigned int data_size);
void __attribute__ ((deprecated)) radioDeletePacket(MacPacket packet);

#endif // __RADIO_H
