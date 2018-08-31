cmake_minimum_required(VERSION 3.10)

project(threads_practice CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(pthread_practice
	pthread_practice.cpp
	pthread_practice.hpp
)

add_executable(thread_practice
	thread_practice.cpp
	thread_practice.hpp
)

