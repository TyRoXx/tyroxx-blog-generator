file(GLOB sources "*.hpp" "*.cpp")
set(formatted ${formatted} ${sources} PARENT_SCOPE)
add_executable(system_test ${sources})
target_link_libraries(system_test ${Boost_LIBRARIES} ${CONAN_LIBS})
if(UNIX)
	target_link_libraries(system_test pthread rt)
endif()
