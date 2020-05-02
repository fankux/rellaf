cmake_minimum_required(VERSION 3.1.0)
project(rellaf)

option(DEBUG "Print debug logs" OFF)
option(WITH_DEBUG_SYMBOLS "With debug symbols" OFF)
option(WITH_JSON "enable rellaf model to json converter" ON)
option(WITH_MYSQL "enable simple mysql connection pool" ON)
option(WITH_BRPC_EXT "enable brpc invoker" ON)
option(WITH_TEST "enable test" ON)
option(WITH_DEMO "CURD web demo" ON)

message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}, version: "
        "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}, version: "
        "${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC is too old, please install a newer version supporting C++11")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # require at least clang 3.3
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
        message(FATAL_ERROR "Clang is too old, please install a newer version supporting C++11")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
else ()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
endif ()

include(ProcessorCount)
ProcessorCount(NUM_OF_PROCESSOR)
message(NUM_OF_PROCESSOR: ${NUM_OF_PROCESSOR})

set(THIRD_DEPS)
set(SRC)

if (WITH_DEMO)
    set(WITH_JSON ON)
    set(WITH_MYSQL ON)
    set(WITH_BRPC_EXT ON)
endif ()

if (WITH_JSON)
    find_path(JSONCPP_INCLUDE_DIR NAMES json/json.h)
    find_library(JSONCPP_LIBRARIES NAMES json_libmt jsoncpp)
    if ((NOT JSONCPP_INCLUDE_DIR) OR (NOT JSONCPP_LIBRARIES))
        message(FATAL_ERROR "Fail to find jsoncpp")
    endif ()

    include_directories(
            ${JSONCPP_INCLUDE_DIR}
    )
    file(GLOB JSON_SRC
            src/json/*.h
            src/json/*.hpp
            src/json/*.cpp
            )
    list(APPEND SRC "${JSON_SRC}")
    list(APPEND THIRD_DEPS ${JSONCPP_LIBRARIES})
endif ()

if (WITH_MYSQL)
    find_path(MYSQL_INCLUDE_DIR NAMES mysql.h)
    find_library(MYSQL_LIBRARIES NAMES mysqlclient)
    if ((NOT MYSQL_INCLUDE_DIR) OR (NOT MYSQL_LIBRARIES))
        message(FATAL_ERROR "Fail to find mysqlclient")
    endif ()

    include_directories(
            ${MYSQL_INCLUDE_DIR}
    )
    file(GLOB MYSQL_SRC
            src/mysql/*.h
            src/mysql/*.hpp
            src/mysql/*.cpp
            )
    list(APPEND SRC "${MYSQL_SRC}")
    list(APPEND THIRD_DEPS ${MYSQL_LIBRARIES})
endif ()

if (WITH_BRPC_EXT)
    find_package(OpenSSL REQUIRED)
    message(STATUS "ssl:" ${OPENSSL_SSL_LIBRARY})
    message(STATUS "crypto:" ${OPENSSL_CRYPTO_LIBRARY})

    find_path(BRPC_INC brpc/server.h)
    find_library(BRPC_LIB NAMES libbrpc.a)
    find_library(GFLAGS_LIB NAMES gflags)
    find_library(PROTOBUF_LIB NAMES protobuf)
    find_library(PROTOC_LIB NAMES protoc)
    find_library(TCMALLOC_LIB NAMES tcmalloc)
    find_library(LEVELDB_LIB NAMES leveldb)

    include_directories(
            ${BRPC_INC}
            include/brpc
    )
    file(GLOB BRPC_SRC
            include/brpc/*.h
            include/brpc/*.hpp
            src/brpc/*.cpp
            )
    list(APPEND SRC "${BRPC_SRC}")
    list(APPEND THIRD_DEPS ${BRPC_LIB})
    list(APPEND THIRD_DEPS ${GFLAGS_LIB})
    list(APPEND THIRD_DEPS ${TCMALLOC_LIB})
    list(APPEND THIRD_DEPS ${PROTOBUF_LIB})
    list(APPEND THIRD_DEPS ${PROTOC_LIB})
    list(APPEND THIRD_DEPS ${LEVELDB_LIB})
endif ()

if (WITH_TEST)
    find_path(GTEST_INCLUDE_PATH NAMES gtest/gtest.h)
    find_library(GTEST_LIB NAMES gtest)
    if ((NOT GTEST_INCLUDE_PATH) OR (NOT GTEST_LIB))
        message(FATAL_ERROR "Fail to find gtest")
    endif ()

    include_directories(${GTEST_INCLUDE_PATH})
    list(APPEND THIRD_DEPS ${GTEST_LIB})
endif ()

if (WITH_DEBUG_SYMBOLS)
    set(DEBUG_SYMBOL "-ggdb")
endif ()

if (NOT DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -O2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -O2")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${DEBUG_SYMBOL} -pipe -m64 -Wall -W -fPIC -Wno-unused-parameter -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${DEBUG_SYMBOL} -pipe -m64 -Wall -W -fPIC -Wno-unused-parameter -fno-omit-frame-pointer")

macro(use_cxx11)
    if (CMAKE_VERSION VERSION_LESS "3.1.3")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif ()
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif ()
    else ()
        set(CMAKE_CXX_STANDARD 11)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
    endif ()
endmacro(use_cxx11)

use_cxx11()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DEP_LIB ${DEP_LIB} rt)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(DEP_LIB ${DEP_LIB}
            "-framework CoreFoundation"
            "-framework CoreGraphics"
            "-framework CoreData"
            "-framework CoreText"
            "-framework Security"
            "-framework Foundation"
            "-Wl,-U,_MallocExtension_ReleaseFreeMemory"
            "-Wl,-U,_ProfilerStart"
            "-Wl,-U,_ProfilerStop")
    include(CheckFunctionExists)
    CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)
    if (NOT HAVE_CLOCK_GETTIME)
        set(DEFINE_CLOCK_GETTIME "-DNO_CLOCK_GETTIME_IN_MAC")
    endif ()
endif ()

add_definitions(
        ${DEFINE_CLOCK_GETTIME}
        -D_GNU_SOURCE
        -D__STDC_FORMAT_MACROS
        -D__STDC_LIMIT_MACROS
        -D__STDC_CONSTANT_MACROS
        -DBRPC_WITH_GLOG=1
)

file(GLOB COMMON_SRC
        include/*.h
        include/*.hpp
        src/*.h
        src/*.hpp
        src/*.cpp
        )
list(APPEND SRC "${COMMON_SRC}")
set(INC
        include
        src
        test/proto
        )

message("THIRD_DEPS : ${THIRD_DEPS}")

add_library(rellaf STATIC ${SRC})
include_directories(${INC})
link_libraries(rellaf
        ${THIRD_DEPS}
        pthread
        dl
        ssl
        crypto
)

##### demo
if (WITH_DEMO)
    # protobuf
    include(FindProtobuf)
    message("protoc : ${PROTOBUF_PROTOC_EXECUTABLE}, proto include : ${PROTOBUF_INCLUDE_DIRS}")

    file(GLOB DEMO_SRC
            demo/*.h
            demo/*.hpp
            demo/*.cpp
            )

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/demo)
    list(APPEND DEMO_SRC "${CMAKE_CURRENT_BINARY_DIR}/demo/cweb_service.pb.cc")
    execute_process(
            COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}/demo
            --proto_path=${PROTOBUF_INCLUDE_DIRS}
            --proto_path=${CMAKE_SOURCE_DIR}/demo ${CMAKE_SOURCE_DIR}/demo/cweb_service.proto
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            ERROR_VARIABLE PROTO_ERROR
            RESULT_VARIABLE PROTO_RESULT
    )
    if (${PROTO_RESULT} EQUAL 0)
    else ()
        message(FATAL_ERROR "Fail to generate cpp of cweb_service.pb.cc : ${PROTO_ERROR}")
    endif ()

    include_directories(
            demo
            ${CMAKE_CURRENT_BINARY_DIR}/demo
    )
    add_executable(cweb_demo ${DEMO_SRC})
    add_dependencies(cweb_demo rellaf)
    target_link_libraries(cweb_demo PUBLIC rellaf ${THIRD_DEPS})

endif ()

if (WITH_TEST)
    add_executable(test_model test/test_model.cpp)
    add_dependencies(test_model rellaf)
    target_link_libraries(test_model PUBLIC rellaf ${THIRD_DEPS})
    add_executable(test_enum test/test_enum.cpp)
    add_dependencies(test_enum rellaf)
    target_link_libraries(test_enum PUBLIC rellaf ${THIRD_DEPS})
    add_executable(test_mapper test/test_sql_builder.cpp)
    add_dependencies(test_mapper rellaf)
    target_link_libraries(test_mapper PUBLIC rellaf ${THIRD_DEPS})

    if (WITH_BRPC_EXT)
        # protobuf
        include(FindProtobuf)
        message("protoc : ${PROTOBUF_PROTOC_EXECUTABLE}, proto include : ${PROTOBUF_INCLUDE_DIRS}")

        file(GLOB PROTO_FILES "${CMAKE_SOURCE_DIR}/test/proto/*.proto")
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/proto)
        foreach (PROTO ${PROTO_FILES})
            message(proto : ${PROTO})
            get_filename_component(PROTO_WE ${PROTO} NAME_WE)
            list(APPEND PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/proto/${PROTO_WE}.pb.cc")
            execute_process(
                    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
                    --cpp_out=${CMAKE_CURRENT_BINARY_DIR}/proto
                    --proto_path=${PROTOBUF_INCLUDE_DIRS}
                    --proto_path=${CMAKE_SOURCE_DIR}/test/proto ${PROTO}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    ERROR_VARIABLE PROTO_ERROR
                    RESULT_VARIABLE PROTO_RESULT
            )
            if (${PROTO_RESULT} EQUAL 0)
            else ()
                message(FATAL_ERROR "Fail to generate cpp of ${PROTO} : ${PROTO_ERROR}")
            endif ()
        endforeach ()
        message("protoc : ${PROTOBUF_PROTOC_EXECUTABLE}, proto srcs : ${PROTO_SRCS}")

        include_directories(${CMAKE_CURRENT_BINARY_DIR}/proto)
        add_executable(test_brpc_serive test/test_brpc_service.cpp ${PROTO_SRCS})
    endif ()

    if (WITH_JSON)
        add_executable(test_json test/test_json.cpp)
    endif ()
endif ()