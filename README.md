openpower-libhei: Hardware Error Isolation for Power Systems
============================================================

This library is a common, portable code base for isolating errors reported by
hardware registers. It can be used in a wide range of user applications:

 * Checkstop/TI error analysis on OpenBMC
 * Checkstop/TI error analysis on FSP
 * Recoverable error analysis on Hostboot/HBRT (aka PRD)
 * Cronus fircheck tool
 * SmartDUMP tool
 * etc.

User Application Requirements and APIs
--------------------------------------

 * Access to the hardware registers will vary user applications. Therefore, this
   library declares the [user APIs](src/hei_user_interface.hpp), but each
   containing user application must implement the APIs.
 * This library will not contain data regarding specific chip information.
   Instead, that information will be provided by the user application in the
   form of the [Chip Data Files](src/chip_data/CHIP_DATA.md).
 * Tracing, or logging, methods vary per application. Specifically, FSP and
   Hostboot/HBRT will define marcos such that the format string is replaced with
   a hash in order save code image space. Therefore, the user application will
   need to provide a specific header that defines these macros. Details are
   included in the [user APIs](src/hei_user_interface.hpp).

Core API
--------

Details TBD.

Integration
-----------

The library is intended to be integrated into containing user applications as a
set of source files (either imported, or as a git subtree/submodule).

Details TBD.

Environment configuration
-------------------------

The Chip Data Files will contain rules for clearing and masking register bits,
where both require modifying hardware registers. However, not all user
applications are allowed to write hardware (e.g. OpenBMC or FSP). Those
applications can define `__HEI_READ_ONLY` to ensure any hardware write support
is disabled.

Development Requirements
------------------------

 * Hostboot/HBRT and FSP environments only support up to **C++11**. Therefore,
   this library cannot use anything newer at this time.
 * Hostboot/HBRT implements their own version of the Standard Template Library,
   but not all of it because of size constraints. So most of the common
   structures like std::vector or std::map are supported, but less common things
   like std::deque are not supported at this time.
 * Hostboot/HBRT and FSP environments will replace C-strings with hashes in
   order to minimize code image size. Therefore, C-strings are required as
   opposed to use of std::string, iostream, etc.

