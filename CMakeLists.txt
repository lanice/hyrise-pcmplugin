# Usually, you should not have to touch the following part ##########################################
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)                                                     #
                                                                                                    #
# Include CMakeLists.txt of Hyrise (all the build magic is contained there)                         #
add_subdirectory(hyrise/)                                                                           #
                                                                                                    #
# Add hyrise sources to be able to include headers                                                  #
include_directories(                                                                                #
    ${PROJECT_SOURCE_DIR}/hyrise/src/lib/                                                           #
    ${PROJECT_SOURCE_DIR}/hyrise/third_party/sql-parser/src                                         #
    ${TBB_INCLUDE_DIR}                                                                              #
    ${Boost_INCLUDE_DIRS}                                                                           #
)                                                                                                   #
                                                                                                    #
# Global flags and include directories                                                              #
add_compile_options(-std=c++17 -pthread -Wall -Wextra -pedantic -Werror -Wno-unused-parameter       #
-Wno-dollar-in-identifier-extension -Wno-unknown-pragmas)                                           #
                                                                                                    #
#####################################################################################################

# Build pcm
add_library(
    pcm

    STATIC

    pcm/msr.cpp
    pcm/cpucounters.cpp
    pcm/pci.cpp
    pcm/client_bw.cpp
    pcm/utils.cpp
    pcm/pcm.cpp
    pcm/pcm-numa.cpp
    pcm/pcm-power.cpp
    pcm/pcm-sensor.cpp
    pcm/pcm-msr.cpp
    pcm/pcm-memory.cpp
    pcm/pcm-tsx.cpp
    pcm/pcm-pcie.cpp
    pcm/pcm-core.cpp
    pcm/pcm-iio.cpp
    pcm/pcm-lspci.cpp
)

target_compile_options(
    pcm
    PRIVATE
    -O3 -std=c++11 -fPIC -DHAVE_STD_REGEX -DNDEBUG
)

target_include_directories(
    pcm
    PUBLIC
    pcm
)

### Modify this line to add your own plugin
add_plugin(NAME PcmPlugin SRCS pcm_plugin.cpp pcm_plugin.hpp DEPS hyriseConsole LIBS pcm)
