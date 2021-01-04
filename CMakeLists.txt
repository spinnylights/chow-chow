cmake_minimum_required(VERSION 3.16..3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(Chow-Chow VERSION 0.1
                  DESCRIPTION "A flexible FM synth"
                  LANGUAGES CXX
                  )

file(GLOB HEADER_LIST CONFIGURE_DEPENDS
     "${Chow-Chow_SOURCE_DIR}/include/chow-chow/*.hpp"
     )

function(setup_simple_bin exe)
        add_executable("${exe}"
                       "${exe}.cpp"
                       ${HEADER_LIST}
                       )

        target_include_directories("${exe}" PUBLIC
                                   "${Chow-Chow_SOURCE_DIR}/include"
                                   )

        target_compile_features("${exe}" PUBLIC cxx_std_17)
        target_link_libraries("${exe}" PUBLIC libchowchow)
endfunction()

add_subdirectory(src)
add_subdirectory(examples)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if (BUILD_TESTING)
        add_subdirectory(extern/doctest)
        # doctest's include stuff doesn't seem to work, so
        include_directories(extern/doctest/doctest)

        add_subdirectory(tests)
    endif()
endif()
