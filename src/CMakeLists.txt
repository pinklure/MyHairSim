configure_file(
	PathConfig.h.in 
	PathConfig.h
)

add_executable(HairSimulation
        Shader.cc
		main.cpp
)

if (MSVC)
	target_compile_options(HairSimulation PRIVATE /W4)
else()
	target_compile_options(HairSimulation PRIVATE -Wall -Wextra -pedantic)
endif()


target_include_directories(HairSimulation
	PRIVATE
		${CMAKE_SOURCE_DIR}/Dependencies/ImageLoader/
		${CMAKE_SOURCE_DIR}/Dependencies/glm/
		${CMAKE_SOURCE_DIR}/Dependencies/OBJ-Loader/Source/
		${CMAKE_BINARY_DIR}/src/
)

target_link_libraries(HairSimulation
	PRIVATE
		Glad
		OpenGL::GL
		glfw
)
