file(GLOB HEADER_LIST CONFIGURE_DEPENDS
     "${Chow-Chow_SOURCE_DIR}/include/chow-chow/*.hpp"
     )

add_executable(unit
               main.cpp
               sample.cpp
               sine.cpp
               frequency.cpp
               phase.cpp
               amp.cpp
               wav_file.cpp
               operators.cpp
               ${HEADER_LIST}
               )

target_include_directories(unit PUBLIC ../include)
target_compile_features(unit PUBLIC cxx_std_17)
target_link_libraries(unit PRIVATE doctest)
target_link_libraries(unit PUBLIC libchowchow)
add_test(NAME unittest COMMAND unit)
