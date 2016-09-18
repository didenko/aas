# Example project for Aeron and SBE dependencies

## About the project

1. Automatically generates de-serializing handler 
0. CMake-generated build
0. Demonstrates build-time SBE stubs generation
0. Demonstrates generating media driver script for client tests (run manually, *nix systems only)
0. Dependency `cmake` files do not contain configuration information
0. Command-line build script is *nix-centered (no Windows script provided)
0. Includes the upcoming `ExternalProject` extension 
0. Apache License 2.0

## Prerequisites

Same prerequisites as for standalone Aeron, SBE, GoogleTest builds. Get recent tool chains (tested on OSX 10.11.6 and Ubuntu 16.04.1), JDK 8, CMake 3+.

## Building

From a command line: run the `./build.sh` script to build and `./test.sh` to run the test. Use `./clean.sh` to remove build-related directories. For in-Clion builds and tests, build the `Build All` target, and the install. Run the `./bin/media_driver.sh` to start the built media driver and the `aeron_and_sbe` target for the example test.

See the short [YouTube video](https://youtu.be/02Fi2rCjCr8) for an example build arrangement.

## Build structure

Dependencies are vendored in the `dep` directory, each brought in through an external project macro in accompanying `cmake` scripts. The root `CMakeLists.txt` calls all the macros and then includes directories for the `make_handler` and `integration_tests` executables.

The `make_handler` executable uses an inlined template and SBE protocol XML definition to generate a C++ incoming messages handler for an Aeron subscription. When called, the handler wraps incoming messages in proper SBE structures and calls message-type-specific intake handlers defined elsewhere.
 
The build of `integration_tests` runs the `make_handler` process to generate the handler. It also runs the `sbe-tool` which generates proper message codes alongside the handler. 

The example intake functions in `integration_tests_intakes.h` header are specializations of the template which is not implemented itself. That way the compilation intentionally fails if a message type intake function is not implemented.

-----
