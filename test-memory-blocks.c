/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/**********************************************************************
 *
 * test-memory-blocks.c
 *
 * Generic memory test supporting multiple byte tests, memory blocks
 * and memory stride.
 *
 **********************************************************************/
#include <stdio.h>
#include "test-memory-blocks.h"

/* defines */
#undef USE_PRINT        // for simulation keep undefined

/* prototypes */
void output_results (void);
uint32_t walking_1_byte_test (volatile unsigned char *address);
uint32_t walking_0_byte_test (volatile unsigned char *address);

/* globals */
test_blob_t tb = {.test_pattern = {(void*) walking_1_byte_test,
                                   (void*) walking_0_byte_test},
                  .address_stride = TEST_ADDRESS_STRIDE,  // address increment 
                  .memory_block = {0x20008000, 0x8000,    // block 1 - set appropriately for your design
                                   0x30000000, 0x8000,    // block 2 - set appropriately for your design
                                   0x87001000, 0x2000,    // block 3 - set appropriately for your design
                                   0x87180000, 0x1000},   // block 4 - set appropriately for your design
                  .num_blocks = NUM_MEM_BLOCKS            // number of defined memory blocks
                 };

/**********************************************************************
 *
 * Function:    main(), the memory test executive
 *
 * Description: Test memory by iterating through the test blob
 *              which defines the tests to run and the blocks to test.
 *
 * Returns:     test_result
 *
 **********************************************************************/
int main (void) {

    int i, j, test_result=0;
    uintptr_t address;

    for (i=0; i<tb.num_blocks; i++) {
        for (j=0; j<NUM_TESTS; j++) {
            for (address = tb.memory_block[i].base_address;
                 address < tb.memory_block[i].base_address+tb.memory_block[i].size && !test_result;
                 address += tb.address_stride) {
                test_result=tb.test_pattern[j]((unsigned char*)address);
                if (test_result!=0) {
                    /* we had a test failure */
                    /* capture the failing address for this block */
                    tb.first_failing_address[i] = address;
                    /* update the overall return to indicate that this memory block failed */
                    tb.result = tb.result | (1 << i);
                }  /* end if () */
            }  /* end for(address) */
        }  /* end for(j) */

        if (test_result) {
            /* reset the local result for the next block */
            test_result = 0;
        }
    }  /* end for(i) */

    output_results();

    return (tb.result);
}  /* end main() */


/**********************************************************************
 *
 * Function:    walking_1_byte_test()
 *
 * Description: Test a byte of memory with a walking 1's test
 *
 *             NOTE:  This test corrupts the address under test.
 *
 * Returns:     0 if the test succeeds.
 *              A non-zero result is the first pattern that failed.
 *
 **********************************************************************/
uint32_t walking_1_byte_test (volatile unsigned char *address)
{
    register unsigned char pattern;  // give the fp a break

    /*
     * Perform a walking 1's test at the given byte address
     * The pattern is {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80}
     */
    for (pattern = 1; pattern != 0; pattern <<= 1) {
        /*
         * Write the test pattern
         */
        *address = pattern;

        /*
         * Read it back and test if it matches the expected
         */
        if (*address != pattern) {
            return ((int)pattern);
        }
    }

    return (0);

}   /* walking_1_byte_test () */


/**********************************************************************
 *
 * Function:    walking_0_byte_test()
 *
 * Description: Test a byte of memory with a walking 0's test
 *
 *              NOTE:  This test corrupts address under test.
 *
 * Returns:     0 if the test succeeds.
 *              A non-zero result is the first pattern that failed.
 *
 **********************************************************************/
uint32_t walking_0_byte_test (volatile unsigned char *address)
{
    register unsigned char pattern, test_pattern;  // give the fp a break

    /*
     * Perform a walking 0's test at the given byte address
     * The pattern is {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F}
     */
    for (pattern = 0x1; pattern != 0; pattern <<= 1) {

        /*
         * Complement the walking 1's pattern to generate the walking 0's pattern
         */
        test_pattern = ~pattern;

        /*
         * Write the test pattern
         */
        *address = test_pattern;

        /*
         * Read it back and test if it matches the expected
         */
        if (*address != test_pattern) {
            return ((int)test_pattern);
        }

    }

    return (0);

}   /* walking_0_byte_test () */


/**********************************************************************
 *
 * Function:    output_results()
 *
 * Description: Dump the test results - either to stdio or temporary
 *              registers.  This is compiled at -O0 to ensure the writes
 *              to t1 - t4 are honored.
 *
 * Returns:     void
 *
 **********************************************************************/
__attribute__ ((optimize(0))) void  output_results(void) {
#ifdef USE_PRINT
    int i;
    printf ("Test results:\n");
    if (!tb.result) {
        printf ("\tTest passed - memory checked ok\n");
    }
    else {
        printf ("\tOverall bit encoded result = 0x%x\n", tb.result);
        printf ("\tFailing addresses:\n");
        for (i=0; i<tb.num_blocks; i++) {
            printf ("\t\tblock = %d  address = 0x%012x\n", i, tb.first_failing_address[i]);
        }
    }
#else
    int temp;
    if (tb.result) {
        temp = tb.result;
    }
    else {
        temp = PASS_VALUE;
    }
    /* the test result is placed in t0 */
    __asm__ volatile ("mv t0, %0" : "=r" (temp));

    /* the first failing addresses are written to the remaining "t" registers */
    /* there are only room for 6 (t1 - t6) */
    /* but... this test only has 4 regions so we write to t1-t4 */
    temp = tb.first_failing_address[0];
    __asm__ volatile ("mv t1, %0" : "=r" (temp));
    temp = tb.first_failing_address[1];
    __asm__ volatile ("mv t2, %0" : "=r" (temp));
    temp = tb.first_failing_address[2];
    __asm__ volatile ("mv t3, %0" : "=r" (temp));
    temp = tb.first_failing_address[3];
    __asm__ volatile ("mv t4, %0" : "=r" (temp));
#endif

}


