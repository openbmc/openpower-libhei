openpower-libhei: Hardware Error Isolation for Power Systems
============================================================

This library is a common, portable code base for isolating errors reported by
hardware registers on POWER Systems chips.

User Application Requirements and APIs
--------------------------------------
 * The method to access hardware register data will vary per user application.
  Therefore, this library declares the hardware access [user APIs], but each
   containing user application must implement the APIs for their environment.
 * This library will not contain data regarding specific chip information.
   Instead, that information will be provided by the user application in the
   form of the [Chip Data Files].
 * Tracing, or logging, methods vary per user application. Specifically, FSP and
   Hostboot utilize specialized marcos as a mechanism to save code image space.
   Therefore, the user application will need to provide a specific header file
   that defines these macros. Details are included in the [user APIs].
 * Methods to assert programming bugs vary per user application. Therefore, much
   like tracing, the user application will need to provide a specific header
   file that defines macros for assertion. Details are included in the
   [user APIs].

[user APIs]: src/hei_user_interface.hpp
[Chip Data Files]: src/chip_data/CHIP_DATA.md

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

Development Notes
-----------------

 * The Hostboot and FSP environments only support up to **C++11**. Therefore,
   this library cannot use anything newer at this time.
 * Hostboot has a very limited environment. It does not include libc or
   libstdc++. However, Hostboot has implemented select functions from those
   libraries as needed. For details, you can reference `src/include/` in the
   [Hostboot project](https://github.com/open-power/hostboot).

