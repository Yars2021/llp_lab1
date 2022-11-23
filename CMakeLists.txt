cmake_minimum_required(VERSION 3.23)
project(llp_lab1_c C)

set(CMAKE_C_STANDARD 99)

add_executable(llp_lab1_c main.c db_internals.h db_internals.c db_file_manager.c db_file_manager.h db_interface.c db_interface.h data_generator.c data_generator.h)
