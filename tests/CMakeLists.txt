add_executable(unit
               main.cpp
               frequency.cpp
               wav_file.cpp
               operator.cpp
               operators.cpp
               phase_acc.cpp
               ${HEADER_LIST}
               )

target_include_directories(unit PUBLIC ../include)
target_compile_features(unit PUBLIC cxx_std_17)
target_link_libraries(unit PRIVATE doctest)
target_link_libraries(unit PUBLIC libchowchow)
add_test(NAME unittest COMMAND unit)

setup_simple_bin(perf)
setup_simple_bin(pure_sine)
