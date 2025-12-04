cmake_minimum_required(VERSION 4.0)
project(LifeGame)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp)

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/saves)

if(DEFINED TRACE_LOGGING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "TRACE_LOG")
elseif (DEFINED DEBUG_LOGGING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "DEBUG_LOG")
endif ()

add_subdirectory(source)
add_subdirectory(test)

target_link_libraries(${PROJECT_NAME} PUBLIC Source)