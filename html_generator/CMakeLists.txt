file(GLOB sources "*.hpp" "*.cpp")
set(formatted ${formatted} ${sources} PARENT_SCOPE)
add_executable(html_generator ${sources})
target_link_libraries(html_generator ${Boost_LIBRARIES} ${CONAN_LIBS})
