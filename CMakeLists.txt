cmake_minimum_required(VERSION 3.0.2)
project(rellaf)

set(CMAKE_CXX_STANDARD 11)

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
        )

include_directories(
        ${INC}
)

option(WITH_JSON "enable rellaf model to json converter, default ON" ON)
option(WITH_MYSQL "enable simple mysql conntection pool, default ON" ON)
option(WITH_TEST "enable test, default ON" ON)

set(THIRD_DEPS)
if (WITH_JSON)
    find_library(JSONCPP_LIB jsoncpp)
    find_path(JSONCPP_INC json/json.h)

    include_directories(${JSONCPP_INC} src/json)
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

    include_directories(${MYSQL_INC} src/mysql)
    file(GLOB MYSQL_SRC
            src/mysql/*.h
            src/mysql/*.hpp
            src/mysql/*.cpp
            )
    list(APPEND SRC "${MYSQL_SRC}")

    list(APPEND THIRD_DEPS ${MYSQL_LIB})
endif ()
if (WITH_TEST)
    find_path(GTEST_INC gtest/gtest.h)
    find_library(GTEST_LIB gtest)

    include_directories(${GTEST_INC})

    list(APPEND THIRD_DEPS ${GTEST_LIB})
endif ()


message("THIRD_DEPS : ${THIRD_DEPS}")

link_libraries(
        ${THIRD_DEPS}
)

add_library(rellaf STATIC ${SRC})

if (WITH_TEST)
    link_libraries(rellaf)
    add_executable(test_model test/test_model.cpp)
    add_executable(test_mapper test/test_mapper.cpp)
endif ()