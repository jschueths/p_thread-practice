cmake_minimum_required(VERSION 3.10)

project(threads_practice CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

add_executable(pthread_practice
	pthread_practice.cpp
	pthread_practice.hpp
)
target_link_libraries(pthread_practice
	Threads::Threads
)

add_executable(thread_practice
	thread_practice.cpp
	thread_practice.hpp
)
target_link_libraries(thread_practice
	Threads::Threads
)
