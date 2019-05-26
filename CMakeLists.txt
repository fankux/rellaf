cmake_minimum_required(VERSION 3.0.2)
project(rellaf)

file(GLOB SRC
        include/*.h
        include/*.hpp
        src/*.h
        src/*.hpp
        src/*.cpp
        )
set(INC
        include
        src

        test/proto
        )

include_directories(
        ${INC}
)

option(WITH_JSON "enable rellaf model to json converter" ON)
option(WITH_MYSQL "enable simple mysql connection pool" ON)
option(WITH_BRPC_EXT "enable brpc invoker" ON)
option(WITH_TEST "enable test" ON)

set(THIRD_DEPS)
if (WITH_JSON)
    find_library(JSONCPP_LIB jsoncpp)
    find_path(JSONCPP_INC json/json.h)

    include_directories(
            ${JSONCPP_INC}
    )
    file(GLOB JSON_SRC
            src/json/*.h
            src/json/*.hpp
            src/json/*.cpp
            )
    list(APPEND SRC "${JSON_SRC}")

    list(APPEND THIRD_DEPS ${JSONCPP_LIB})
endif ()

if (WITH_MYSQL)
    find_library(MYSQL_LIB mysqlclient)
    find_path(MYSQL_INC mysql.h)

    include_directories(
            ${MYSQL_INC}
    )
    file(GLOB MYSQL_SRC
            src/mysql/*.h
            src/mysql/*.hpp
            src/mysql/*.cpp
            )
    list(APPEND SRC "${MYSQL_SRC}")

    list(APPEND THIRD_DEPS ${MYSQL_LIB})
endif ()

if (WITH_BRPC_EXT)
    find_path(BRPC_INC brpc/server.h)
    find_library(BRPC_LIB NAMES libbrpc.a)
    find_library(GFLAGS_LIB NAMES gflags)
    find_library(TCMALLOC_LIB NAMES tcmalloc)
    find_library(PROTOBUF_LIB NAMES protobuf)
    find_library(PROTOC_LIB NAMES protoc)
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
    find_path(GTEST_INC gtest/gtest.h)
    find_library(GTEST_LIB gtest)

    include_directories(${GTEST_INC})

    list(APPEND THIRD_DEPS ${GTEST_LIB})
endif ()

message("THIRD_DEPS : ${THIRD_DEPS}")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DYNAMIC_LIB ${DYNAMIC_LIB} rt)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include(CheckFunctionExists)
    CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)
    if (NOT HAVE_CLOCK_GETTIME)
        set(DEFINE_CLOCK_GETTIME "-DNO_CLOCK_GETTIME_IN_MAC")
    endif ()
    set(DYNAMIC_LIB ${DYNAMIC_LIB}
            "-framework CoreFoundation"
            "-framework CoreGraphics"
            "-framework CoreData"
            "-framework CoreText"
            "-framework Security"
            "-framework Foundation"
            "-Wl,-U,_MallocExtension_ReleaseFreeMemory"
            "-Wl,-U,_ProfilerStart"
            "-Wl,-U,_ProfilerStop"
            )
endif ()

link_libraries(
        ${THIRD_DEPS}
        ${DYNAMIC_LIB}
        pthread
        dl
        ssl
        crypto
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CPP_FLAGS "${DEFINE_CLOCK_GETTIME} -DBRPC_WITH_GLOG=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -pipe -Wall -W -fPIC -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -pipe -Wall -W -fPIC -Wno-unused-parameter")

add_library(rellaf STATIC ${SRC})

if (WITH_TEST)
    link_libraries(rellaf)
    add_executable(test_model test/test_model.cpp)
    add_executable(test_enum test/test_enum.cpp)
    add_executable(test_mapper test/test_sql_builder.cpp)

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