project(Nara-Sandbox LANGUAGES C)

add_executable(Sandbox)
target_link_libraries(Sandbox PRIVATE Nara)
target_sources(Sandbox PRIVATE Sandbox.c)

add_custom_target(Run
	COMMAND Sandbox
	DEPENDS Sandbox
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
