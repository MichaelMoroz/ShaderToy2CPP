cmake_minimum_required(VERSION 3.8)
project(ShaderToy VERSION "0.1")
set(CMAKE_CXX_STANDARD 17)

configure_file(src/config.h.in config.h)

if (WIN32)
	#CHECK THE BITNESS
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	    set(BIT 64)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	    set(BIT 32)
	endif()

	## DEPENDENCIES
	set(GLEW_DIR C:/Libraries/GLEW/)
	set(GLEW_INCLUDE_DIRS ${GLEW_DIR}/include)
	set(SDK_DIRECTORY "C:/Program Files (x86)/Microsoft SDKs")
	set(GLM_INCLUDE_DIR C:/Libraries/glm)
	set(SFML_STATIC_LIBRARIES TRUE)

	if(BIT EQUAL 32)
	#32 bit compilation
		set(SFML_DIR C:/Libraries/SFML-2.5.1/lib/cmake/SFML)
		set(SFML_INCLUDE_DIR C:/Libraries/SFML-2.5.1/include)
		set(GLEW_LIBRARIES ${GLEW_DIR}/lib/Release/Win32/glew32.lib)
	else()
	#64 bit compilation
		set(SFML_DIR C:/Libraries/SFML-2.5.1_64/lib/cmake/SFML)
		set(SFML_INCLUDE_DIR C:/Libraries/SFML-2.5.1_64/include)
		set(GLEW_LIBRARIES ${GLEW_DIR}/lib/Release/x64/glew32.lib)
	endif()

	find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)
	
elseif(UNIX)	
	set(SFML_STATIC_LIBRARIES FALSE)
	find_package(GLEW REQUIRED)
	find_package(OpenGL REQUIRED)
	find_package(glm REQUIRED)
endif()

## TARGETS
add_subdirectory(src)
target_include_directories(ShaderToySources PUBLIC
  src
  ${GLEW_INCLUDE_DIRS}
  ${OpenGL_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${GLM_INCLUDE_DIR})

if(WIN32)
	target_include_directories(ShaderToySources PUBLIC
	  ${SFML_INCLUDE_DIR}
	)
	include_directories(${ANTTWEAKBAR_DIR}/include)
	include_directories(${SDK_DIRECTORY}/Windows/v7.1/Include)
else()
	target_include_directories(ShaderToySources PUBLIC
	  src
	  /usr/include
	)
endif()

target_compile_definitions(ShaderToySources PRIVATE SFML_STATIC)

if(WIN32)
  add_executable(ShaderToy WIN32 src/Main.cpp src/Resource.rc)
else()
  add_executable(ShaderToy src/Main.cpp)
endif()

if(WIN32)
	if(BIT EQUAL 32)
		add_custom_command(TARGET ShaderToy PRE_BUILD
		           COMMAND ${CMAKE_COMMAND} -E copy_directory
		               ${CMAKE_SOURCE_DIR}/bin32 $<TARGET_FILE_DIR:ShaderToy>)
	else()
		add_custom_command(TARGET ShaderToy PRE_BUILD
		           COMMAND ${CMAKE_COMMAND} -E copy_directory
		               ${CMAKE_SOURCE_DIR}/bin64 $<TARGET_FILE_DIR:ShaderToy>)
	endif()	
endif()	

if(CMAKE_COMPILER_IS_GNUCC)
  # needed for gcc 4.6+
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()				   

target_compile_definitions(ShaderToy PRIVATE SFML_STATIC)

target_link_libraries(ShaderToy
  ShaderToySources
  ${ATB_LIB}
  ${GLEW_LIBRARIES}
  sfml-system
  sfml-window
  sfml-graphics
  sfml-audio
)

