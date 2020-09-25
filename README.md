# test-memory-blocks 

A destructive memory test that tests memory blocks on a byte-sized basis.

The parameters of the test are defined in a test_blob_t structure, tb.

The tb structure is initialized in test-memory.c.  It defines the following:
 * test patterns (input)
 * memory blocks to test (input)
 * address stride (input)
 * number of blocks to test (input)
 * first failing address (output) 
 * test result (output)

## Test Patterns
There are two test patterns:  walking0 and walking1. 
New test patterns can be added by changing NUM_TESTS and adding the new
test function to tb.test_pattern[]. 

## Memory Blocks 
A memory block is defined by a base address and size in bytes. 
The number of memory blocks is set by NUM_MEM_BLOCKS. 

## Address Stride
To move through a memory block quicker, an address_stride is supported. For instance:
 address_stride = 1 will test every byte of the memory block
 address_stride = 2 will test every other byte of the memory block 
 address_stride = 0x1000 will test every 4KB of the memory block
 
## First Failing Address
If the test of a memory block fails, the first failing address is captured in this array.
When a failure is detected, the testing of that block stops. 

## Test Result
The test result is 0 for success and non-zero for fail.  For test failures, the 
result is bit encoded where:
 bit0 represents the success of block 0 (0b for pass, 1b for fail)
 bit1 represents the success of block 1 (0b for pass, 1b for fail)
 ...
For example if a 4 region test has a failure only for block 3, then the
result is set to 0x8.  The test result is the return value from main(). 

## Controlling Test Output
A USE_PRINT preprocessor conditional controls whether results are printed using printf()
or redirected to internal registers for simulation purposes.  If USE_PRINT is not defined
then the end of the simulation trace log can be inspected for results.  The test result
value is returned from main() and this will show a PASS or FAIL result as a normal test. 
When the test fails in simulation, the test result is also displayed.

In addition t0 will be written with tb.result if there was a failure and PASS_VALUE if
the test passed.  The first 4 first_failing_address values are stored to
t1, t2, t3 and t4.

## Test Deployment
In a RTL environment, the files can be incorporated into your tarball workspace and 
executed like a standard example delivered with the tarball.  Follow these steps:
1) Copy all files into a new folder called sifive_coreip/freedom-e-sdk/software/test-memory-blocks
2) Build the test from the sifive_coreip/freedom-e-sdk level:
   make PROGRAM=test-memory-blocks TARGET=design-rtl CONFIGURATION=release software
3) Execute the test at the sifive_coreip level:
   make test-memory-blocks.vcs.out 

In a Freedom Studio environment for testing on the ARTY FPGA, the source files can be added to 
a new project based on your IP Deliverable.

