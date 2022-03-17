#https://github.com/meemknight/cmakeSetup

cmake_minimum_required(VERSION 3.16)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

project(pavo-gui)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw-3.3.2)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/stb_image)
add_subdirectory(thirdparty/stb_truetype)
add_subdirectory(thirdparty/gl2d)
add_subdirectory(thirdparty/raudio)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/imgui-docking)

if(WIN32)

elseif(UNIX)
	add_subdirectory(thirdparty/libelfin-fbreg)
endif()


# Define PAVO-GUI-CPPS to be a list of all the source files for cli
file(GLOB_RECURSE PAVO-GUI-CPPS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pavo-gui/src/*.cpp")
file(GLOB_RECURSE PAVO-SHARED-CPPS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shared/src/*.cpp")
file(GLOB_RECURSE PAVO-CLI-CPPS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pavo-cli/src/*.cpp")


####################################################################################################################################################################################
#pavo-gui
####################################################################################################################################################################################

add_executable(pavo-gui)
set_property(TARGET pavo-gui PROPERTY CXX_STANDARD 17)

target_compile_definitions(pavo-gui PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/pavo-gui/resources/") 
#target_compile_definitions(pavo-gui PUBLIC RESOURCES_PATH="./resources/") #for release-ship builds

target_sources(pavo-gui PRIVATE ${PAVO-GUI-CPPS} ${PAVO-SHARED-CPPS} )


if(MSVC) # If using the VS compiler...
	target_compile_definitions(pavo-gui PUBLIC _CRT_SECURE_NO_WARNINGS)
	set_target_properties(pavo-gui PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

if(WIN32)
	target_compile_definitions(pavo-gui PUBLIC PAVO_WIN32)
elseif(UNIX)
	target_compile_definitions(pavo-gui PUBLIC PAVO_UNIX) 
else()
	message(FATAL_ERROR "Pavo only supports windows and unix" )
endif()


target_include_directories(pavo-gui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/shared/include/")
target_include_directories(pavo-gui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/pavo-gui/include/")
target_include_directories(pavo-gui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/pavo-gui/include/gameLayer/")
target_include_directories(pavo-gui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/pavo-gui/include/platform/")


if(WIN32)

elseif(UNIX)
	target_link_libraries(pavo-gui PRIVATE libelfin)
endif()

target_link_libraries(pavo-gui PRIVATE glm glfw glad stb_image stb_truetype gl2d raudio imgui)



####################################################################################################################################################################################
#pavo-cli
####################################################################################################################################################################################

add_executable(pavo-cli)
set_property(TARGET pavo-cli PROPERTY CXX_STANDARD 17)

target_sources(pavo-cli PRIVATE ${PAVO-CLI-CPPS} ${PAVO-SHARED-CPPS} )

if(MSVC) # If using the VS compiler...
	target_compile_definitions(pavo-cli PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

if(WIN32)
	target_compile_definitions(pavo-cli PUBLIC PAVO_WIN32)
elseif(UNIX)
	target_compile_definitions(pavo-cli PUBLIC PAVO_UNIX) 
else()
	message(FATAL_ERROR "Pavo only supports windows and unix")
endif()

target_include_directories(pavo-cli PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/shared/include/")
target_include_directories(pavo-cli PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/pavo-cli/include/")

if(WIN32)

elseif(UNIX)
	target_link_libraries(pavo-cli PRIVATE libelfin imgui)
endif()

#target_link_libraries(pavo-cli PRIVATE )
