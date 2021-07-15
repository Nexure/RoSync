# CMakeLists.txt : CMake Project for RoSync

cmake_minimum_required (VERSION 3.8)

# Project Def
project (RoSync)

# Set C++ Standard
set (CMAKE_CXX_STANDARD 20)

# Web Server (Based on web sockets)
add_subdirectory (webservice)
add_subdirectory (core)

# Project - WebService (Main Project, Executable)
# Libary Project - Core