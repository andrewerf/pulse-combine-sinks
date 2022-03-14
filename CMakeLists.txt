project(pulse-combine-sinks)

set(CMAKE_CXX_STANDARD 20)

add_executable(pulse-combine-sinks main.cpp)
target_link_libraries(pulse-combine-sinks pulse)
install(TARGETS pulse-combine-sinks)
