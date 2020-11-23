file(GLOB HEADER_LIST CONFIGURE_DEPENDS
     "${Chow-Chow_SOURCE_DIR}/include/chow-chow/*.hpp"
     )

function(setup exe)
        add_executable("${exe}"
                       "${exe}.cpp"
                       ${HEADER_LIST}
                       )
        target_include_directories("${exe}" PUBLIC ../include)
        target_compile_features("${exe}" PUBLIC cxx_std_17)
        target_link_libraries("${exe}" PUBLIC libchowchow)
endfunction()

setup(gunstar)

setup(sine)

setup(two_op)
