add_library(lab02_utils src/utils.cpp)
target_include_directories(lab02_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(lab02_utils PUBLIC cxx_std_20)

add_executable(lab02_task1 task1.cpp)
add_executable(lab02_task2 task2.cpp)
add_executable(lab02_task3 task3.cpp)
add_executable(lab02_task4 task4.cpp)

find_package(GSL REQUIRED)
target_link_libraries(lab02_utils PUBLIC GSL::gsl GSL::gslcblas)
target_link_libraries(lab02_task1 PRIVATE lab02_utils)
target_link_libraries(lab02_task2 PRIVATE lab02_utils)
target_link_libraries(lab02_task3 PRIVATE lab02_utils)
target_link_libraries(lab02_task4 PRIVATE lab02_utils)

foreach(target lab02_task1 lab02_task2 lab02_task3 lab02_task4)
    target_compile_features(${target} PRIVATE cxx_std_20)
endforeach()

set(LAB02_RESULTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results)
set(LAB02_PLOTS_DIR ${LAB02_RESULTS_DIR}/plots)
set(MATPLOTLIB_CONFIG_DIR ${CMAKE_BINARY_DIR}/matplotlib-cache)
set(PLOT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/plot.py)

set(LAB02_TASK1_SMALL_OUTPUT ${LAB02_RESULTS_DIR}/task1_uniform_100.txt)
set(LAB02_TASK1_MEDIUM_OUTPUT ${LAB02_RESULTS_DIR}/task1_uniform_1000.txt)
set(LAB02_TASK1_LARGE_OUTPUT ${LAB02_RESULTS_DIR}/task1_uniform_10000.txt)
set(LAB02_TASK1_SMALL_ARGS
    100
    1234
    ${LAB02_TASK1_SMALL_OUTPUT}
)
set(LAB02_TASK1_MEDIUM_ARGS
    1000
    1234
    ${LAB02_TASK1_MEDIUM_OUTPUT}
)
set(LAB02_TASK1_LARGE_ARGS
    10000
    1234
    ${LAB02_TASK1_LARGE_OUTPUT}
)

set(LAB02_TASK2_OUTPUT ${LAB02_RESULTS_DIR}/task2_chi_square.csv)
set(LAB02_TASK2_ARGS
    100000
    1234
    ${LAB02_TASK2_OUTPUT}
)

set(LAB02_TASK3_OUTPUT ${LAB02_RESULTS_DIR}/task3_pi_convergence.csv)
set(LAB02_TASK3_ARGS
    1000000
    1234
    ${LAB02_TASK3_OUTPUT}
)

set(LAB02_TASK4_OUTPUT ${LAB02_RESULTS_DIR}/task4_sphere_volume_convergence.csv)
set(LAB02_TASK4_ARGS
    1000000
    1234
    ${LAB02_TASK4_OUTPUT}
)

add_custom_target(run_lab02_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB02_RESULTS_DIR}
    COMMAND lab02_task1 ${LAB02_TASK1_SMALL_ARGS}
    COMMAND lab02_task1 ${LAB02_TASK1_MEDIUM_ARGS}
    COMMAND lab02_task1 ${LAB02_TASK1_LARGE_ARGS}
    DEPENDS lab02_task1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab02 task1"
    VERBATIM
)

add_custom_target(plot_lab02_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB02_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB02_TASK1_SMALL_OUTPUT}
        ${LAB02_PLOTS_DIR}/task1_histogram_100_samples_10_bins.png
        --plot_type histogram
        --x_label x
        --y_label count
        --title "Task 1: 100 samples, 10 bins"
        --bins 10
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB02_TASK1_SMALL_OUTPUT}
        ${LAB02_PLOTS_DIR}/task1_histogram_100_samples_50_bins.png
        --plot_type histogram
        --x_label x
        --y_label count
        --title "Task 1: 100 samples, 50 bins"
        --bins 50
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB02_TASK1_MEDIUM_OUTPUT}
        ${LAB02_PLOTS_DIR}/task1_histogram_1000_samples_10_bins.png
        --plot_type histogram
        --x_label x
        --y_label count
        --title "Task 1: 1000 samples, 10 bins"
        --bins 10
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB02_TASK1_MEDIUM_OUTPUT}
        ${LAB02_PLOTS_DIR}/task1_histogram_1000_samples_50_bins.png
        --plot_type histogram
        --x_label x
        --y_label count
        --title "Task 1: 1000 samples, 50 bins"
        --bins 50
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB02_TASK1_LARGE_OUTPUT}
        ${LAB02_PLOTS_DIR}/task1_histogram_10000_samples_10_bins.png
        --plot_type histogram
        --x_label x
        --y_label count
        --title "Task 1: 10000 samples, 10 bins"
        --bins 10
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB02_TASK1_LARGE_OUTPUT}
        ${LAB02_PLOTS_DIR}/task1_histogram_10000_samples_50_bins.png
        --plot_type histogram
        --x_label x
        --y_label count
        --title "Task 1: 10000 samples, 50 bins"
        --bins 50
    DEPENDS run_lab02_task1 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab02 task1 histograms"
    VERBATIM
)

add_custom_target(run_lab02_task2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB02_RESULTS_DIR}
    COMMAND lab02_task2 ${LAB02_TASK2_ARGS}
    DEPENDS lab02_task2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab02 task2"
    VERBATIM
)

add_custom_target(run_lab02_task3
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB02_RESULTS_DIR}
    COMMAND lab02_task3 ${LAB02_TASK3_ARGS}
    DEPENDS lab02_task3
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab02 task3"
    VERBATIM
)

add_custom_target(plot_lab02_task3
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB02_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB02_TASK3_OUTPUT}
        ${LAB02_PLOTS_DIR}/task3_pi_convergence.png
        --plot_type errorbar
        --x_label "number of trials"
        --y_label "pi estimate"
        --title "Task 3: convergence of Monte Carlo pi estimate"
        --x_column trials
        --value_column pi_estimate
        --error_column stddev
        --log_x
        --reference_line 3.141592653589793
    DEPENDS run_lab02_task3 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab02 task3 convergence plot"
    VERBATIM
)

add_custom_target(run_lab02_task4
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB02_RESULTS_DIR}
    COMMAND lab02_task4 ${LAB02_TASK4_ARGS}
    DEPENDS lab02_task4
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab02 task4"
    VERBATIM
)

add_custom_target(plot_lab02_task4
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB02_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB02_TASK4_OUTPUT}
        ${LAB02_PLOTS_DIR}/task4_sphere_volume_convergence.png
        --plot_type errorbar
        --x_label "number of trials"
        --y_label "sphere volume estimate"
        --title "Task 4: convergence of Monte Carlo sphere volume estimate"
        --x_column trials
        --value_column sphere_volume_estimate
        --error_column stddev
        --log_x
        --reference_line 4.1887902047863905
    DEPENDS run_lab02_task4 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab02 task4 convergence plot"
    VERBATIM
)

add_custom_target(run_lab02_all
    DEPENDS run_lab02_task1 run_lab02_task2 run_lab02_task3 run_lab02_task4
    COMMENT "Running all lab02 tasks"
    VERBATIM
)

add_custom_target(plot_lab02
    DEPENDS plot_lab02_task1 plot_lab02_task3 plot_lab02_task4
)
