project(Test)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Source)

file(GLOB_RECURSE src "*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${src})

find_package(GTest REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest)
