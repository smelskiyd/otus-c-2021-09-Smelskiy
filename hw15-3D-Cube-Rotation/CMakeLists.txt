project(OtusHW15_3DCubeRotation C)

find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(OtusHW15_3DCubeRotation main.c)
target_link_libraries(OtusHW15_3DCubeRotation PRIVATE GLUT::GLUT GLEW::GLEW OpenGL::GL)
