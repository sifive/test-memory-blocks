/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/**********************************************************************
 *
 * test-memory-blocks.h
 *
 * Header file for block memory test.
 *
 **********************************************************************/

#ifndef TEST_MEMORY_H
#define TEST_MEMORY_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define NUM_TESTS           2           // currently walking0 and walking1
#define NUM_MEM_BLOCKS      4           // number of memory regions to test
#define TEST_ADDRESS_STRIDE 0x100       // test every 256 bytes
#define PASS_VALUE          0x33333333  // used for success... written to t0 in simulation

// define the memory block
typedef struct  {
    uintptr_t   base_address;           // first address of memory block
    size_t      size;                   // size of memory block in bytes
} memory_block_t;

// define the "test blob"... named no better
typedef struct {
    uintptr_t      (*test_pattern[NUM_TESTS])(volatile unsigned char *);
    memory_block_t memory_block[NUM_MEM_BLOCKS];
    uint32_t       address_stride;    // address increment (1 = test all addresses, 2 = test every other)
    uint32_t       num_blocks;
    uintptr_t      first_failing_address[NUM_MEM_BLOCKS];  // non-zero if there was a failure
    uint32_t       result;  // bit encoded... 1b for fail... bit 0 for block 0, bit X for block X
} test_blob_t;

#endif
