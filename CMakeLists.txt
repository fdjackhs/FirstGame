cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(PROJECT_NAME FirstGame)
project(FirstGame)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME}
	${PROJECT_NAME}/src/main.cpp
	${PROJECT_NAME}/src/RenderEngine.h
	${PROJECT_NAME}/src/RenderEngine.cpp
	${PROJECT_NAME}/src/Game.h
	${PROJECT_NAME}/src/Game.cpp
	${PROJECT_NAME}/src/Object.h
	${PROJECT_NAME}/src/Object.cpp
	${PROJECT_NAME}/src/Shader.h
	${PROJECT_NAME}/src/Mesh.h
	${PROJECT_NAME}/src/Mesh.cpp
	${PROJECT_NAME}/src/Model.h
	${PROJECT_NAME}/src/Model.cpp
	${PROJECT_NAME}/src/Camera.h
	${PROJECT_NAME}/src/Camera.cpp
	${PROJECT_NAME}/src/ResourceManager.h
	${PROJECT_NAME}/src/ResourceManager.cpp
	${PROJECT_NAME}/src/ObjectAttributes.h
	${PROJECT_NAME}/src/Unit.h
	${PROJECT_NAME}/src/Unit.cpp
	${PROJECT_NAME}/src/SelectArea.h
	${PROJECT_NAME}/src/SelectArea.cpp
	${PROJECT_NAME}/src/ManuallyCreatedObject.h
	${PROJECT_NAME}/src/ManuallyCreatedObject.cpp
	${PROJECT_NAME}/src/Planet.h
	${PROJECT_NAME}/src/Planet.cpp
	${PROJECT_NAME}/src/Profile.h
	${PROJECT_NAME}/src/Button.h
	${PROJECT_NAME}/src/Button.cpp
	${PROJECT_NAME}/src/Label.h
	${PROJECT_NAME}/src/Label.cpp
	${PROJECT_NAME}/src/Statistic.h
	${PROJECT_NAME}/src/Utilities.h
	${PROJECT_NAME}/src/WhiteStar.h
	${PROJECT_NAME}/src/AI.cpp
	${PROJECT_NAME}/src/CreateObject.cpp
	${PROJECT_NAME}/src/SaveMap.cpp
	
	${PROJECT_NAME}/src/stb_image.cpp
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(includes/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(includes/glad)
target_link_libraries(${PROJECT_NAME} glad)
  
include_directories(includes)
include_directories(includes/stb)
include_directories(includes/glm)

target_link_libraries(${PROJECT_NAME} assimp-vc140-mt)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
