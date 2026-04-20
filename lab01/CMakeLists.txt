add_library(lab01_utils src/utils.cpp)
target_include_directories(lab01_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(lab01_utils PUBLIC cxx_std_20)

add_executable(lab01_task1 task1.cpp)
add_executable(lab01_task2 task2.cpp)
add_executable(lab01_task3 task3.cpp)
add_executable(lab01_task4 task4.cpp)

target_link_libraries(lab01_task1 PRIVATE lab01_utils)
target_link_libraries(lab01_task2 PRIVATE lab01_utils)
target_link_libraries(lab01_task3 PRIVATE lab01_utils)

find_package(GSL REQUIRED)
target_link_libraries(lab01_task4 PRIVATE lab01_utils GSL::gsl GSL::gslcblas)

foreach(target lab01_task1 lab01_task2 lab01_task3 lab01_task4)
    target_compile_features(${target} PRIVATE cxx_std_20)
endforeach()

target_compile_options(lab01_task3 PRIVATE -O2)
target_compile_options(lab01_task4 PRIVATE -O2)

set(LAB01_TASK1_A17_STEP1_ARGS
    17 0 8191 1 2500 1
    ${CMAKE_CURRENT_SOURCE_DIR}/results/task1_a17_step1.csv
)

set(LAB01_TASK1_A17_STEP2_ARGS
    17 0 8191 1 2500 2
    ${CMAKE_CURRENT_SOURCE_DIR}/results/task1_a17_step2.csv
)

set(LAB01_TASK1_A85_STEP1_ARGS
    85 0 8191 1 2500 1
    ${CMAKE_CURRENT_SOURCE_DIR}/results/task1_a85_step1.csv
)

set(LAB01_TASK1_A85_STEP2_ARGS
    85 0 8191 1 2500 2
    ${CMAKE_CURRENT_SOURCE_DIR}/results/task1_a85_step2.csv
)

set(LAB01_TASK2_STEP1_ARGS
    1 1 1 10000 1
    ${CMAKE_CURRENT_SOURCE_DIR}/results/task2_step1.csv
)

set(LAB01_TASK2_STEP2_ARGS
    1 1 1 10000 2
    ${CMAKE_CURRENT_SOURCE_DIR}/results/task2_step2.csv
)

set(LAB01_TASK3_ARGS
    300 0.0 1.0 5489
    ${CMAKE_CURRENT_SOURCE_DIR}/results/task3_results.csv
)

set(LAB01_TASK4_ARGS
    300 5489
    ${CMAKE_CURRENT_SOURCE_DIR}/results/task4_results.csv
)

set(LAB01_PLOTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results/plots)
set(PLOT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/plot.py)

add_custom_target(run_lab01_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/results
    COMMAND lab01_task1 ${LAB01_TASK1_A17_STEP1_ARGS}
    COMMAND lab01_task1 ${LAB01_TASK1_A17_STEP2_ARGS}
    COMMAND lab01_task1 ${LAB01_TASK1_A85_STEP1_ARGS}
    COMMAND lab01_task1 ${LAB01_TASK1_A85_STEP2_ARGS}
    DEPENDS lab01_task1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab01_task1 for a=17/85 and step=1/2"
)

add_custom_target(run_lab01_task2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/results
    COMMAND lab01_task2 ${LAB01_TASK2_STEP1_ARGS}
    COMMAND lab01_task2 ${LAB01_TASK2_STEP2_ARGS}
    DEPENDS lab01_task2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab01_task2 for step=1/2"
)

add_custom_target(run_lab01_task3
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/results
    COMMAND lab01_task3 ${LAB01_TASK3_ARGS}
    DEPENDS lab01_task3
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab01_task3: <min_milliseconds> <low> <high> <seed> <filepath>"
)

add_custom_target(run_lab01_task4
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/results
    COMMAND lab01_task4 ${LAB01_TASK4_ARGS}
    DEPENDS lab01_task4
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab01_task4: <min_milliseconds> <seed> <filepath>"
)

add_custom_target(run_lab01_all
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/results
    COMMAND lab01_task1 ${LAB01_TASK1_A17_STEP1_ARGS}
    COMMAND lab01_task1 ${LAB01_TASK1_A17_STEP2_ARGS}
    COMMAND lab01_task1 ${LAB01_TASK1_A85_STEP1_ARGS}
    COMMAND lab01_task1 ${LAB01_TASK1_A85_STEP2_ARGS}
    COMMAND lab01_task2 ${LAB01_TASK2_STEP1_ARGS}
    COMMAND lab01_task2 ${LAB01_TASK2_STEP2_ARGS}
    COMMAND lab01_task3 ${LAB01_TASK3_ARGS}
    COMMAND lab01_task4 ${LAB01_TASK4_ARGS}
    DEPENDS lab01_task1 lab01_task2 lab01_task3 lab01_task4
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all lab01 tasks"
)

add_custom_target(plot_lab01
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB01_PLOTS_DIR}
    COMMAND ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${CMAKE_CURRENT_SOURCE_DIR}/results/task1_a17_step1.csv
        ${LAB01_PLOTS_DIR}/task1_a17_step1.png
        --plot_type scatter
        --x_label x_n
        --y_label x_n+1
        --title "Task 1: a=17, step=1"
    COMMAND ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${CMAKE_CURRENT_SOURCE_DIR}/results/task1_a17_step2.csv
        ${LAB01_PLOTS_DIR}/task1_a17_step2.png
        --plot_type scatter
        --x_label x_n
        --y_label x_n+2
        --title "Task 1: a=17, step=2"
    COMMAND ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${CMAKE_CURRENT_SOURCE_DIR}/results/task1_a85_step1.csv
        ${LAB01_PLOTS_DIR}/task1_a85_step1.png
        --plot_type scatter
        --x_label x_n
        --y_label x_n+1
        --title "Task 1: a=85, step=1"
    COMMAND ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${CMAKE_CURRENT_SOURCE_DIR}/results/task1_a85_step2.csv
        ${LAB01_PLOTS_DIR}/task1_a85_step2.png
        --plot_type scatter
        --x_label x_n
        --y_label x_n+2
        --title "Task 1: a=85, step=2"
    COMMAND ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${CMAKE_CURRENT_SOURCE_DIR}/results/task2_step1.csv
        ${LAB01_PLOTS_DIR}/task2_step1.png
        --plot_type scatter
        --x_label k_n
        --y_label k_n+1
        --title "Task 2: step=1"
    COMMAND ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${CMAKE_CURRENT_SOURCE_DIR}/results/task2_step2.csv
        ${LAB01_PLOTS_DIR}/task2_step2.png
        --plot_type scatter
        --x_label k_n
        --y_label k_n+2
        --title "Task 2: step=2"
    COMMAND ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${CMAKE_CURRENT_SOURCE_DIR}/results/task3_results.csv
        ${LAB01_PLOTS_DIR}/task3_times.png
        --plot_type bar
        --y_label "time [us]"
        --title "Task 3: C++ generator timings"
        --value_column time_us
    COMMAND ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${CMAKE_CURRENT_SOURCE_DIR}/results/task4_results.csv
        ${LAB01_PLOTS_DIR}/task4_relative_times.png
        --plot_type bar
        --y_label "relative time"
        --title "Task 4: GSL timings relative to default"
        --value_column relative_to_default
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab01 plots"
)

add_dependencies(plot_lab01 run_lab01_all python_venv)
