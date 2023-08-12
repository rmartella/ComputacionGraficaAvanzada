include_directories(${CMAKE_SOURCE_DIR}/CGALib/include)
add_executable(01-AnimacionKeyFrames ./src/main.cpp)

set_target_properties(01-AnimacionKeyFrames 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../../bin"
)

target_link_libraries(01-AnimacionKeyFrames glut glfw GLEW GLU GL freeimage assimp freetype alut openal CGALib)