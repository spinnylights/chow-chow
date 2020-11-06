file(GLOB HEADER_LIST CONFIGURE_DEPENDS
     "${Chow-Chow_SOURCE_DIR}/include/chow-chow/*.hpp"
     )

function(setup exe)
        target_include_directories("${exe}" PUBLIC ../include)
        target_compile_features("${exe}" PUBLIC cxx_std_17)
        target_link_libraries("${exe}" PUBLIC libchowchow)
endfunction()

add_executable(sine
               sine.cpp
               ${HEADER_LIST}
               )
setup(sine)

add_executable(simple
               simple.cpp
               ${HEADER_LIST}
               )
setup(simple)

add_executable(gunstar
               gunstar.cpp
               ${HEADER_LIST}
               )
setup(gunstar)
