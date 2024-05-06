cmake_minimum_required(VERSION 3.5)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

set(PROJECT_NAME json2var)


project(
  ${PROJECT_NAME}
  VERSION 0.1
  LANGUAGES C CXX
)

set(ENABLE_CJSON_TEST OFF)

include(CheckSymbolExists)
add_subdirectory(cJSON)


add_library(${PROJECT_NAME}
    json2var.h
    json2var.c
    varcommon/varcommon.c
    varcommon/varcommon.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    varcommon/
    cJSON/
)
target_link_libraries(${PROJECT_NAME} PRIVATE cjson)




