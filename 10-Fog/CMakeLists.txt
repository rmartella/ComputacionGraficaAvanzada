add_definitions(-DBUILDING_DLL) # Define a macro indicating DLL import


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Set the directory where DLLs are located
    set(CMAKE_DLLTOOLCHAIN_PREFIX "../dlls")
    include_directories(
        10-Fog
        ${CMAKE_SOURCE_DIR}/CGALib/include
        "/usr/include/freetype2" 
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
        "../external/freealut/lib"
        "../external/OpenAL/lib/x64"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories(${CMAKE_SOURCE_DIR}/CGALib/include "/usr/include/freetype2")
endif()

add_executable(10-Fog ./src/main.cpp)

set_target_properties(10-Fog
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../../bin"
)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(10-Fog opengl32.lib libglfw3dll.dll.a libglew32.dll.a libassimp.dll.a libfreetype.dll.a libalut.dll.a OpenAL32.lib CGALib)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(10-Fog glut glfw GLEW GLU GL freeimage assimp freetype alut openal CGALib)
endif()