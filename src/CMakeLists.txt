file(GLOB HEADER_LIST CONFIGURE_DEPENDS
     "${Chow-Chow_SOURCE_DIR}/include/chow-chow/*.hpp"
     )

add_library(libchowchow
            ${HEADER_LIST}
            "${Chow-Chow_SOURCE_DIR}/extern/magic_enum/include/magic_enum.hpp"
            setting.cpp
            frequency.cpp
            operator.cpp
            operators.cpp
            wav_file.cpp
            phase_acc.cpp
            )

set_property(TARGET libchowchow PROPERTY CXX_STANDARD 17)

target_include_directories(libchowchow PUBLIC ../include)
target_include_directories(libchowchow PUBLIC ../extern/magic_enum/include)

target_compile_features(libchowchow PUBLIC cxx_std_17)
