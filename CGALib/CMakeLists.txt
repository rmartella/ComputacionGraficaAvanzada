add_definitions(-DBUILDING_DLL) # Define a macro indicating DLL import

file(GLOB CGALib_SRC
     "./include/Headers/*.h"
     "./src/*.cpp"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
     include_directories(
          CGALib 
          "./include/" 
          "../external/assimp/include"
          "../external/glfw/include"
          "../external/glm"
          "../external/glew/include"
          "../external/FreeImage/include"
          "../external/OpenAL/include"
          "../external/Freetype/include"
          "../external/freealut/include"

     )
     link_directories(
          "../external/assimp/lib"
          "../external/glfw/lib"
          "../external/Freetype/lib"
          "../external/FreeImage/lib"
          "../external/glew/lib"
     )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
     include_directories(
          CGALib 
          "./include/" 
          "/usr/include/freetype2" 
     )
endif()
add_library(CGALib SHARED ${CGALib_SRC})
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
     set_target_properties(CGALib
          PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY "../../bin"
     )
     target_link_libraries(CGALib opengl32.lib libglfw3dll.dll.a libglew32.dll.a libassimp.dll.a libfreetype.dll.a)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
     target_link_libraries(CGALib glut glfw GLEW GLU GL freeimage assimp freetype)
endif()