macro(AddNLTpl FileName Md5)

    ExternalProject_Add(
            nltpl
            URL "${FileName}"
            URL_MD5 ${Md5}
            SOURCE_SUBDIR "NLTemplate"
            INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(nltpl source_dir)
    set(NLTPL_INCLUDE_DIR "${source_dir}/NLTemplate")
    ExternalProject_Get_Property(nltpl binary_dir)
    set(NLTPL_LIBRARY "${binary_dir}/libnltpl.a")

endmacro(AddNLTpl)
