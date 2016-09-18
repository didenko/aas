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

include_directories(
    "${GTEST_INCLUDES}"
    "${AERON_INCLUDE_DIR}"
    "${SBE_INCLUDE_DIR}"
)

add_custom_target(
    tests_sbe_protocol
    java
    "-Dsbe.output.dir=${CMAKE_CURRENT_SOURCE_DIR}"
    "-Dsbe.target.language=CPP"
    "-jar" "${SBE_TOOL_JAR}"
    "directory_schema.xml"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Generate SBE stubs"
)

add_dependencies(
    tests_sbe_protocol
    sbe
)

add_custom_target(
    tests_make_handler
    "${CMAKE_BINARY_DIR}/src/make_handler/make_handler"
    "directory_schema.xml"
    "./tests_messages/MakeHandler.h"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Generate SBE incoming dispatcher"
)

add_dependencies(
    tests_make_handler
    make_handler
)

add_executable(
    integration_tests
    integration_tests.cpp
)

add_dependencies(
    integration_tests
    gtest
    aeron
    sbe
    tests_sbe_protocol
    tests_make_handler
)

target_link_libraries(integration_tests ${GTEST_LIBS})
target_link_libraries(integration_tests ${AERON_LIBS})
target_link_libraries(integration_tests ${SBE_LIBS})

install(
    PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/integration_tests"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

install(
    FILES "${AERON_MEDIA_DRIVER}"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
    RENAME aeron-driver.jar
)

install(
    PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/media_driver.sh"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
