cmake_minimum_required(VERSION 3.5)

# O nome do projeto é fase1
PROJECT(fase1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 1. Definir os dois executáveis
add_executable(generator generator.cpp)
add_executable(engine engine.cpp catmull.cpp TinyXML/tinyxml2.cpp)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

if (WIN32)
    message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")
    
    if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(ERROR ": GLUT not found")
    endif()   
    
    include_directories(${TOOLKITS_FOLDER}/glut)
    
    # Fazemos o link para ambos os alvos explicitamente
    target_link_libraries(generator ${OPENGL_LIBRARIES} ${TOOLKITS_FOLDER}/glut/glut32.lib)
    target_link_libraries(engine ${OPENGL_LIBRARIES} ${TOOLKITS_FOLDER}/glut/glut32.lib)
    
    if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif() 
    
else() # Linux e Mac

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})
    
    # FAZEMOS O LINK PARA OS DOIS EXECUTÁVEIS:
    target_link_libraries(generator ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    target_link_libraries(engine ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    
    if(NOT GLUT_FOUND)
       message(ERROR ": GLUT not found!")
    endif()
    
endif(WIN32)