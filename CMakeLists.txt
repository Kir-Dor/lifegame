cmake_minimum_required(VERSION 4.0)
project(LifeGame)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE source)

file(GLOB_RECURSE src "source/*.cpp")
target_sources(${PROJECT_NAME} PUBLIC ${src})

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/saves)

if(DEFINED TRACE_LOGGING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "TRACE_LOG")
elseif (DEFINED DEBUG_LOGGING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "DEBUG_LOG")
endif ()

find_package(Boost REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})

find_package(SFML REQUIRED COMPONENTS Graphics Window System Main)
target_link_libraries(${PROJECT_NAME} PUBLIC SFML::Graphics SFML::Window SFML::System SFML::Main)

find_package(spdlog REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)