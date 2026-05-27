cmake_minimum_required(VERSION 3.16)
project(TileMaxxing CXX)

# Suppress warnings regarding extraction timestamps for URL downloads in CMake 3.24+
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable modern CMake RPATH settings so that targets can locate the dynamic
# ONNX Runtime library at runtime from the build tree.
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# ── Locate ONNX Runtime ───────────────────────────────────────────────────────
# Try the official CMake config first (works if ORT was installed via a package
# that ships onnxruntimeConfig.cmake, e.g. a manual install or vcpkg).
find_package(onnxruntime CONFIG QUIET)

if(NOT onnxruntime_FOUND)
    # Check if ORT_ROOT is already set, otherwise search common locations.
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

    if(ORT_ROOT)
        message(STATUS "Found local ONNX Runtime installation at: ${ORT_ROOT}")
        add_library(onnxruntime::onnxruntime SHARED IMPORTED)
        
        if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set_target_properties(onnxruntime::onnxruntime PROPERTIES
                IMPORTED_LOCATION             "${ORT_ROOT}/lib/libonnxruntime.dylib"
                INTERFACE_INCLUDE_DIRECTORIES "${ORT_ROOT}/include")
        else()
            set_target_properties(onnxruntime::onnxruntime PROPERTIES
                IMPORTED_LOCATION             "${ORT_ROOT}/lib/libonnxruntime.so"
                INTERFACE_INCLUDE_DIRECTORIES "${ORT_ROOT}/include")
        endif()
        set(onnxruntime_FOUND TRUE)
    endif()
endif()

if(NOT onnxruntime_FOUND)
    message(STATUS "ONNX Runtime not found on host. Downloading prebuilt official binaries...")

    # Use a highly compatible and stable version
    set(ORT_VERSION "1.18.0")
    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
            set(ORT_ARCH "osx-arm64")
        else()
            set(ORT_ARCH "osx-x86_64")
        endif()
        set(ORT_URL "https://github.com/microsoft/onnxruntime/releases/download/v${ORT_VERSION}/onnxruntime-${ORT_ARCH}-${ORT_VERSION}.tgz")
        set(ORT_LIB_NAME "libonnxruntime.dylib")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
            set(ORT_ARCH "linux-aarch64")
        else()
            set(ORT_ARCH "linux-x64")
        endif()
        set(ORT_URL "https://github.com/microsoft/onnxruntime/releases/download/v${ORT_VERSION}/onnxruntime-${ORT_ARCH}-${ORT_VERSION}.tgz")
        set(ORT_LIB_NAME "libonnxruntime.so")
    else()
        message(FATAL_ERROR "Unsupported platform for automatic ONNX Runtime download. Please install manually.")
    endif()

    message(STATUS "Downloading ONNX Runtime package: ${ORT_URL}")

    include(FetchContent)
    FetchContent_Declare(
        onnxruntime_download
        URL ${ORT_URL}
    )
    FetchContent_MakeAvailable(onnxruntime_download)

    # Determine extraction directory structure (nested or flat)
    if(EXISTS "${onnxruntime_download_SOURCE_DIR}/include/onnxruntime_cxx_api.h")
        set(ORT_ROOT "${onnxruntime_download_SOURCE_DIR}")
    else()
        set(ORT_ROOT "${onnxruntime_download_SOURCE_DIR}/onnxruntime-${ORT_ARCH}-${ORT_VERSION}")
    endif()

    if(NOT EXISTS "${ORT_ROOT}/include/onnxruntime_cxx_api.h")
        message(FATAL_ERROR "Failed to locate extracted headers in: ${ORT_ROOT}")
    endif()

    message(STATUS "ONNX Runtime successfully extracted at: ${ORT_ROOT}")

    add_library(onnxruntime::onnxruntime SHARED IMPORTED)
    set_target_properties(onnxruntime::onnxruntime PROPERTIES
        IMPORTED_LOCATION             "${ORT_ROOT}/lib/${ORT_LIB_NAME}"
        INTERFACE_INCLUDE_DIRECTORIES "${ORT_ROOT}/include")
    
    set(onnxruntime_FOUND TRUE)
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
