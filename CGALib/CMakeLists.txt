file(GLOB CGALib_SRC
     "./include/Headers/*.h"
     "./src/*.cpp"
)

include_directories(CGALib "./include/" "/usr/include/freetype2")
add_library(CGALib SHARED ${CGALib_SRC})
target_link_libraries(CGALib glut glfw GLEW GLU GL freeimage assimp freetype)