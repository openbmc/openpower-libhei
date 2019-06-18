Chip Data Files
===============

The Chip Data Files define everything we need to know about a chip type. Their
purpose is to keep libhei agnostic to specific chip information. Required
information includes:
 * All hardware addresses needed for error isolation.
 * A definition of how errors propagate from register to register.
 * A list of top level registers to use as a starting point for isolation.
 * Error types reported by each register bit.
 * A list of additional registers to capture for each register bit (for debug).
 * Rules defining how to clear and mask register bits (not available if
   `__HEI_READ_ONLY` is defined).

Requirements
------------

 * These files must be consumable by different host archetictures. So the data
   fields in must be stored in network byte order (big-endian).

File Format
-----------

### Header

 4 bytes - Magic number of 0x4348495044415441 (ascii for "CHIPDATA").
 1 byte  - File version number. Currently, only version 1 is supported.
 3 bytes - unused

### Register Type Lists

TBD

### Registers

TBD

### Isolation Rules

TBD

