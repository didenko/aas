macro(AddAeron FileName Md5)
    ExternalProject_Add(
        aeron
        URL "${FileName}"
        URL_MD5 ${Md5}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ./gradlew COMMAND ./cppbuild/cppbuild
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE ON
    )

    ExternalProject_Get_Property(aeron source_dir)
    set(AERON_SOURCE_DIR ${source_dir})

    ExternalProject_Get_Property(aeron binary_dir)
    set(AERON_BINARY_DIR ${binary_dir})

    set(AERON_INCLUDE_DIR "${AERON_SOURCE_DIR}/aeron-client/src/main/cpp")
    set(AERON_LIBRARY_DIR "${AERON_BINARY_DIR}/cppbuild/Release/lib")

    set(
        AERON_LIBS
        ${AERON_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}aeron_client${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${AERON_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}aeron_client_test${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    set(AERON_MEDIA_DRIVER "${AERON_BINARY_DIR}/aeron-driver/build/libs/aeron-driver-${AERON_VERSION}-all.jar")

endmacro(AddAeron)
