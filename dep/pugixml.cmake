macro(AddPugiXml FileName Md5)

    ExternalProject_Add(
            pugixml
            URL "${FileName}"
            URL_MD5 ${Md5}
            SOURCE_SUBDIR "scripts"
            INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(pugixml source_dir)
    set(PUGIXML_SOURCE_DIR ${source_dir})
    ExternalProject_Get_Property(pugixml binary_dir)
    set(PUGIXML_BINARY_DIR ${binary_dir})

    set(PUGIXML_INCLUDE_DIR "${PUGIXML_SOURCE_DIR}/src")
    set(PUGIXML_LIBRARY "${PUGIXML_BINARY_DIR}/libpugixml.a")

endmacro(AddPugiXml)
