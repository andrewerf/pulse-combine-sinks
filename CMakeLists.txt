project(pulse_combine_sinks)

set(CMAKE_CXX_STANDARD 20)

add_executable(pulse_combine_sinks main.cpp)
target_link_libraries(pulse_combine_sinks pulse)
install(TARGETS pulse_combine_sinks)
