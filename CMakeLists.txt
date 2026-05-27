cmake_minimum_required(VERSION 3.16)
project(TileMaxxing CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Locate ONNX Runtime ───────────────────────────────────────────────────────
# Try the official CMake config first (works if ORT was installed via a package
# that ships onnxruntimeConfig.cmake, e.g. a manual install or vcpkg).
find_package(onnxruntime CONFIG QUIET)

if(NOT onnxruntime_FOUND)
    # Fallback: search common locations.
    # Override by passing -DORT_ROOT=/your/path to cmake.
    if(NOT ORT_ROOT)
        foreach(candidate
                "/opt/homebrew/opt/onnxruntime"      # Homebrew Apple Silicon
                "/usr/local/opt/onnxruntime"          # Homebrew Intel
                "/usr/local"                          # manual install
                "/usr")
            if(EXISTS "${candidate}/include/onnxruntime_cxx_api.h")
                set(ORT_ROOT "${candidate}")
                break()
            endif()
        endforeach()
    endif()

    if(NOT ORT_ROOT)
        message(FATAL_ERROR
            "ONNX Runtime not found. Install it (e.g. brew install onnxruntime) "
            "or pass -DORT_ROOT=/path/to/onnxruntime to cmake.")
    endif()

    message(STATUS "Using ONNX Runtime from: ${ORT_ROOT}")

    add_library(onnxruntime::onnxruntime SHARED IMPORTED)
    set_target_properties(onnxruntime::onnxruntime PROPERTIES
        IMPORTED_LOCATION             "${ORT_ROOT}/lib/libonnxruntime.dylib"
        INTERFACE_INCLUDE_DIRECTORIES "${ORT_ROOT}/include")
endif()

# ── Shared game sources (no entry point) ─────────────────────────────────────
set(GAME_SOURCES
    src/board.cpp
    src/mcts.cpp
    src/heuristic.cpp
    src/network.cpp
    src/solver.cpp
    src/display.cpp
    src/data_capture.cpp)

# ── data_capture executable ───────────────────────────────────────────────────
add_executable(data_capture ${GAME_SOURCES} src/data_capture_main.cpp)
target_include_directories(data_capture PRIVATE src)
target_link_libraries(data_capture PRIVATE onnxruntime::onnxruntime)

# ── tilemax executable (main.cpp) ─────────────────────────────────────────────
add_executable(tilemax ${GAME_SOURCES} src/main.cpp)
target_include_directories(tilemax PRIVATE src)
target_link_libraries(tilemax PRIVATE onnxruntime::onnxruntime)

# ── Tests ─────────────────────────────────────────────────────────────────────
enable_testing()

add_executable(test_board tests/test_board.cpp src/board.cpp)
target_include_directories(test_board PRIVATE src)
add_test(NAME board COMMAND test_board)

add_executable(test_mcts
    tests/test_mcts.cpp
    src/board.cpp
    src/mcts.cpp
    src/heuristic.cpp
    src/network.cpp
    src/solver.cpp
    src/display.cpp)
target_include_directories(test_mcts PRIVATE src)
target_link_libraries(test_mcts PRIVATE onnxruntime::onnxruntime)
add_test(NAME mcts COMMAND test_mcts)
