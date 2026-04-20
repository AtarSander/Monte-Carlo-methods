cmake_minimum_required(VERSION 3.16)

project(monte-carlo-methods LANGUAGES CXX)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(PROJECT_PYTHON_VENV ${CMAKE_BINARY_DIR}/.venv)
if(WIN32)
    set(PROJECT_VENV_PYTHON ${PROJECT_PYTHON_VENV}/Scripts/python.exe)
else()
    set(PROJECT_VENV_PYTHON ${PROJECT_PYTHON_VENV}/bin/python)
endif()

set(PROJECT_PYTHON_VENV_STAMP ${PROJECT_PYTHON_VENV}/.stamp)

add_custom_command(
    OUTPUT ${PROJECT_PYTHON_VENV_STAMP}
    COMMAND ${Python3_EXECUTABLE} -m venv ${PROJECT_PYTHON_VENV}
    COMMAND ${PROJECT_VENV_PYTHON} -m pip install --upgrade pip
    COMMAND ${PROJECT_VENV_PYTHON} -m pip install -r ${CMAKE_SOURCE_DIR}/requirements.txt
    COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_PYTHON_VENV_STAMP}
    DEPENDS ${CMAKE_SOURCE_DIR}/requirements.txt
    COMMENT "Creating Python virtual environment"
)

add_custom_target(python_venv DEPENDS ${PROJECT_PYTHON_VENV_STAMP})

add_subdirectory(lab01)
