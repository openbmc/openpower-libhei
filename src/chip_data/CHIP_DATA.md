# Chip Data Files

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

As in Linux, a file extention is not required, but it is recommended to use the
extention `.cdb` (chip data binary).

## Requirements

 * These files must be consumable by different host archetictures. So the data
   fields in must be stored in network byte order (big-endian).

## File Format

### 1) File metadata

At a minimum, each Chip Data File must define the following:

| Size |         Desc        |               Value/Example                |
|:----:|:-------------------:|:------------------------------------------:|
|    8 | magic number        | 0x4348495044415441 (ascii for "CHIPDATA")  |
|    1 | file version        | Version 1 => 0x01                          |
|    4 | chip model/level    | Unique ID defined by data files owner [^1] |
|    1 | # of Reg Type Lists | 0-255                                      |

[^1]: The user application should be able to use this data to determine if this
is the correct data file to use.

### 2) Register Type List metadata

**Note:** The number of Register Type Lists is found in the File metadata.

The Chip Data Files will support registers of different types and sizes. All
registers of the same type and size will be contained within the same list. The
list metadata is as follows:

| Size |          Desc          |               Value/Example               |
|:----:|:----------------------:|:-----------------------------------------:|
|    2 | hardware access type   | Unique ID defined by data file owner [^2] |
|    1 | address byte size      | 1-255 (0 is invalid)                      |
|    1 | data byte size         | 1-255 (0 is invalid)                      |
|    2 | # of Registers in list | 0-65535                                   |

[^2]: The user application should be able to use this data to determine how to
access the register.

#### 2.1) Register metadata

**Note:** The number of Registers in a list is found in the Register Type List
metadata.


| Size |          Desc          |               Value/Example               |
|:----:|:----------------------:|:-----------------------------------------:|
|    2 | register ID            | Unique ID defined by data file owner [^3] |
|    1 | # of Address Instances | 1-255 (0 is invalid)                      |

[^3]: This will be used as part of the error signature for each reported error.
This value must be unique across all Registers defined in all Register Type
Lists.

**TBD:** By default, registers are assumed read/write accessible. However, we
will need some mechanism to indicate a register is read-only or write-only.
**TBD:** Do we need some mechanism to indicate the bit ordering?
**TBD:** Need to add mechanism to mask and/or clear bits in a register.
**TBD:** There are scenarios where we want to isolate down to something like a
`c_err_rpt` register. In which case, we can clear/mask bits in that register,
but then we would then need to clear/mask bits in the containing FIR.
**TBD:** There are cases where a bit may be a side-effect of another bit and we
will want to clear and/or mask the side-effect. I am thinking this is too
complicated for the Chip Data Files and should be handled on the RAS side of
analysis. That should seem possible because the analyzer will have a list of
all active attention. So we should be able to pick the necessary bits in the
signature list mask/clear them. If that is the case, we should be able to apply
the same tactic to the previous TBD above.

##### 2.1.1) Address Instance metadata

Some registers will have multiple instances with the same definition for each
instance. For example, there may be an error register for each core unit within
a processor chip. Instead of copying the same bit definition for each instance
of the register, only one bit definition will be used

| Size |    Desc    |         Value/Example         |
|:----:|:----------:|:-----------------------------:|
|    1 | instance # | defined by hardware spec [^4] |
|    x | address    | defined by hardware spec      |

[^4]: This will be used as part of the error signature for each reported error.

