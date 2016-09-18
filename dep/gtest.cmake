macro(AddGtest FileName Md5)
    ExternalProject_Add(
        gtest
        URL "${FileName}"
        URL_MD5 ${Md5}
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(gtest source_dir)
    set(GTEST_SOURCE_DIR ${source_dir})

    ExternalProject_Get_Property(gtest binary_dir)
    set(GTEST_BINARY_DIR ${binary_dir})

    set(
        GTEST_LIBS
        ${GTEST_BINARY_DIR}/${CMAKE_CFG_INTDIR}/googlemock/gtest/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${GTEST_BINARY_DIR}/${CMAKE_CFG_INTDIR}/googlemock/gtest/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    set(
        GTEST_INCLUDES
        "${GTEST_SOURCE_DIR}/googletest/include"
        "${GTEST_SOURCE_DIR}/googletest/"
    )
endmacro(AddGtest)
