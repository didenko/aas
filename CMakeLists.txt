#    Copyright 2016 Vlad Didenko
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

cmake_minimum_required(VERSION 3.0.2)
project(aas VERSION 0.16.0917)

set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type choice" FORCE)
endif (NOT CMAKE_BUILD_TYPE)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    # The pugixml and nltpl directory structures require SOURCE_SUBDIR feature
    # of the ExternalProject_Add module, which feature is not yet released.
    # See https://gitlab.kitware.com/cmake/cmake/issues/15297 for the pull request
    include(dep/ExternalProject_patch/ExternalProject.cmake)
else ()
    include(ExternalProject)
endif ()

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin")

include(dep/gtest.cmake)
AddGtest("${CMAKE_SOURCE_DIR}/dep/gtest-1.8.0.tar.gz" 91500c3d80ac1273fa56d405ec88311d)

include(dep/sbe.cmake)
set(SBE_VERSION "1.5.1")
AddSBE("${CMAKE_SOURCE_DIR}/dep/sbe-${SBE_VERSION}.tar.gz" 8dfe73c1a501fd7915b0b33a038b32f5)

include(dep/aeron.cmake)
set(AERON_VERSION "1.0.1")
AddAeron("${CMAKE_SOURCE_DIR}/dep/aeron-${AERON_VERSION}.tar.gz" d63dfba9ca00aa8b6669fc8d4b85c846)

include(dep/pugixml.cmake)
AddPugiXml("${CMAKE_SOURCE_DIR}/dep/pugixml-1.7.tar.gz" 17e6a3996de2942629dce65db1a701c5)

include(dep/nltpl.cmake)
AddNLTpl("${CMAKE_SOURCE_DIR}/dep/nltpl-0.16.0912.tgz" c9af2cd08301f5d6bcd4b5c7e2d25b23)

add_subdirectory(src/make_handler)
add_subdirectory(tests)

add_custom_target(
        "RunInstall"
        "$(MAKE)" "install"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Install"
)

enable_testing()

add_test(
        NAME aeron_and_sbe
        COMMAND integration_tests
)
