macro(AddSBE FileName Md5)
    ExternalProject_Add(
        sbe
        URL "${FileName}"
        URL_MD5 ${Md5}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ./gradlew COMMAND ./cppbuild/cppbuild
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE ON
    )

    ExternalProject_Get_Property(sbe source_dir)
    set(SBE_SOURCE_DIR ${source_dir})

    ExternalProject_Get_Property(sbe binary_dir)
    set(SBE_BINARY_DIR ${binary_dir})

    set(SBE_INCLUDE_DIR "${SBE_SOURCE_DIR}/sbe-tool/src/main/cpp")
    set(SBE_LIBRARY_DIR "${SBE_BINARY_DIR}/cppbuild/Release/lib")

    set(
        SBE_LIBS
        ${SBE_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}sbe${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    set(SBE_TOOL_JAR "${SBE_BINARY_DIR}/sbe-all/build/libs/sbe-all-${SBE_VERSION}.jar")

endmacro(AddSBE)
