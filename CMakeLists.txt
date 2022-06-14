#https://github.com/meemknight/cmakeSetup

cmake_minimum_required(VERSION 3.16)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

#enable avx on gcc
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")

project(pavo-gui)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


add_subdirectory(thirdparty/glfw-3.3.2)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/stb_image)
add_subdirectory(thirdparty/stb_truetype)
add_subdirectory(thirdparty/gl2d)
add_subdirectory(thirdparty/raudio)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/imgui-docking)
add_subdirectory(thirdparty/cpu_features)
add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/linenoise)
add_subdirectory(thirdparty/Catch2-3.0.1)

if(WIN32)

elseif(UNIX)
        #add_subdirectory(thirdparty/libelfin-fbreg)
endif()

include(CTest)
include(thirdparty/Catch2-3.0.1/extras/Catch.cmake)

# Define PAVO-GUI-CPPS to be a list of all the source files for cli
file(GLOB_RECURSE PAVO-GUI-CPPS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pavo-gui/src/*.cpp")
file(GLOB_RECURSE PAVO-SHARED-CPPS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shared/src/*.cpp")
file(GLOB_RECURSE PAVO-CLI-CPPS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pavo-cli/src/*.cpp")
file(GLOB_RECURSE PAVO-TESTS-CPPS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pavo-tests/src/*.cpp")


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

target_link_libraries(pavo-gui PRIVATE glm glfw glad stb_image stb_truetype gl2d raudio imgui cpu_features INTERFACE fmt-header-only)



####################################################################################################################################################################################
#pavo-cli
####################################################################################################################################################################################

add_executable(pavo-cli)
set_property(TARGET pavo-cli PROPERTY CXX_STANDARD 20)

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
target_include_directories(pavo-cli PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libelfin-fbreg/elf/")
target_include_directories(pavo-cli PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libelfin-fbreg/dwarf/")

if(WIN32)

elseif(UNIX)

        add_custom_target(
           libelfin
           COMMAND make
           WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/thirdparty/libelfin-fbreg
        )

        target_link_libraries(pavo-cli PRIVATE
                              ${PROJECT_SOURCE_DIR}/thirdparty/libelfin-fbreg/dwarf/libdwarf++.so
                              ${PROJECT_SOURCE_DIR}/thirdparty/libelfin-fbreg/elf/libelf++.so)

        add_dependencies(pavo-cli libelfin)

endif()

	target_link_libraries(pavo-cli PRIVATE imgui fmt cpu_features lineNoise)


###test program
#add_executable(pavo-test "${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp")




####################################################################################################################################################################################
#pavo-tests
####################################################################################################################################################################################

add_executable(pavo-tests)
set_property(TARGET pavo-tests PROPERTY CXX_STANDARD 20)

target_sources(pavo-tests PRIVATE ${PAVO-TESTS-CPPS} ${PAVO-SHARED-CPPS} )

if(MSVC) # If using the VS compiler...
	target_compile_definitions(pavo-tests PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

if(WIN32)
	target_compile_definitions(pavo-tests PUBLIC PAVO_WIN32)

elseif(UNIX)
	target_compile_definitions(pavo-tests PUBLIC PAVO_UNIX) 
else()
	message(FATAL_ERROR "Pavo only supports windows and unix")
endif()

target_include_directories(pavo-tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/shared/include/")

if(WIN32)

elseif(UNIX)
        #target_link_libraries(pavo-tests PRIVATE libelfin)

endif()

target_link_libraries(pavo-tests PRIVATE imgui fmt cpu_features Catch2::Catch2WithMain)
catch_discover_tests(pavo-tests)

add_executable(pavo-test "${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp")

