project(Source)

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED TRACE_LOGGING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "TRACE_LOG")
elseif (DEFINED DEBUG_LOGGING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "DEBUG_LOG")
endif ()

file(GLOB_RECURSE src "*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${src})

find_package(Boost REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})

find_package(SFML REQUIRED COMPONENTS Graphics Window System Main)
target_link_libraries(${PROJECT_NAME} PUBLIC SFML::Graphics SFML::Window SFML::System SFML::Main)

find_package(spdlog REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)