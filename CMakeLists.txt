cmake_minimum_required(VERSION 3.16..3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(Chow-Chow VERSION 0.1
                  DESCRIPTION "A powerful FM synth"
                  LANGUAGES CXX
                  )

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
