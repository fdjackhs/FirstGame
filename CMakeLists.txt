cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(PROJECT_NAME FirstGame)
project(FirstGame)

add_executable(${PROJECT_NAME}
	${PROJECT_NAME}/src/main.cpp
	${PROJECT_NAME}/src/RenderEngine.h
	${PROJECT_NAME}/src/RenderEngine.cpp
	${PROJECT_NAME}/src/Game.h
	${PROJECT_NAME}/src/Game.cpp
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} glad)

include_directories(external/glm)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)