cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_COVERAGE "Build coverage" OFF)

set(
        HUNTER_CACHE_SERVERS
        "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
        CACHE STRING "Default cache server"
)

include("tools/gate/cmake/HunterGate.cmake")

HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.23.214.tar.gz"
        SHA1 "e14bc153a7f16d6a5eeec845fb0283c8fad8c358"
)

project(ProducerConsumer)
string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

hunter_add_package(gumbo)
find_package(gumbo CONFIG REQUIRED)
hunter_add_package(Boost COMPONENTS program_options thread log filesystem)
find_package(Boost CONFIG REQUIRED program_options thread log log_setup filesystem)
hunter_add_package(OpenSSL)
find_package(OpenSSL REQUIRED)
find_package(Threads)


add_executable(
        ProducerConsumer
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/main.cpp
)

target_include_directories(
        ProducerConsumer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
        ProducerConsumer
        ${CMAKE_THREAD_LIBS_INIT}
        gumbo::gumbo
        OpenSSL::SSL
        Boost::program_options
        Boost::filesystem
        Boost::log
        Boost::thread
        Boost::log_setup
)
