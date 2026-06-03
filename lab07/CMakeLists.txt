add_library(lab07_utils src/utils.cpp)
add_executable(lab07_task1 task1.cpp)
add_executable(lab07_task2 task2.cpp)
target_include_directories(lab07_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(lab07_utils PUBLIC cxx_std_20)
target_compile_features(lab07_task1 PRIVATE cxx_std_20)
target_compile_features(lab07_task2 PRIVATE cxx_std_20)
target_link_libraries(lab07_task1 PRIVATE lab07_utils)
target_link_libraries(lab07_task2 PRIVATE lab07_utils)

set(LAB07_RESULTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results)
set(LAB07_PLOTS_DIR ${LAB07_RESULTS_DIR}/plots)
set(MATPLOTLIB_CONFIG_DIR ${CMAKE_BINARY_DIR}/matplotlib-cache)
set(PLOT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/plot.py)

set(LAB07_TASK1_SUMMARY_OUTPUT ${LAB07_RESULTS_DIR}/task1_summary.csv)
set(LAB07_TASK1_SAMPLES_OUTPUT ${LAB07_RESULTS_DIR}/task1_samples.csv)
set(LAB07_TASK1_ARGS
    1000000
    1234
    ${LAB07_TASK1_SUMMARY_OUTPUT}
    ${LAB07_TASK1_SAMPLES_OUTPUT}
)

set(LAB07_TASK2_SUMMARY_OUTPUT ${LAB07_RESULTS_DIR}/task2_summary.csv)
set(LAB07_TASK2_SAMPLES_OUTPUT ${LAB07_RESULTS_DIR}/task2_samples.csv)
set(LAB07_TASK2_ARGS
    1000000
    1234
    ${LAB07_TASK2_SUMMARY_OUTPUT}
    ${LAB07_TASK2_SAMPLES_OUTPUT}
)

add_custom_target(run_lab07_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB07_RESULTS_DIR}
    COMMAND lab07_task1 ${LAB07_TASK1_ARGS}
    DEPENDS lab07_task1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab07 task1"
    VERBATIM
)

add_custom_target(plot_lab07_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB07_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB07_TASK1_SAMPLES_OUTPUT}
        ${LAB07_PLOTS_DIR}/task1_histogram.png
        --plot_type histogram_normal
        --histogram_column y
        --x_label "y"
        --y_label "density"
        --title "Task 1: Monte Carlo samples and theoretical normal density"
        --bins 80
        --x_min -10
        --x_max 10
        --normal_mean 0.0
        --normal_stddev 2.2360679775
    DEPENDS run_lab07_task1 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab07 task1 plot"
    VERBATIM
)

add_custom_target(run_lab07_task2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB07_RESULTS_DIR}
    COMMAND lab07_task2 ${LAB07_TASK2_ARGS}
    DEPENDS lab07_task2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab07 task2"
    VERBATIM
)

add_custom_target(plot_lab07_task2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB07_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB07_TASK2_SAMPLES_OUTPUT}
        ${LAB07_PLOTS_DIR}/task2_x1_histogram.png
        --plot_type histogram_custom_density
        --histogram_column x1
        --x_label "x1"
        --y_label "density"
        --title "Task 2: X1 histogram"
        --bins 80
        --x_min 0
        --x_max 4
        --density_function lab07_task2_x1
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB07_TASK2_SAMPLES_OUTPUT}
        ${LAB07_PLOTS_DIR}/task2_x2_histogram.png
        --plot_type histogram_custom_density
        --histogram_column x2
        --x_label "x2"
        --y_label "density"
        --title "Task 2: X2 histogram"
        --bins 80
        --x_min 5
        --x_max 6
        --density_function lab07_task2_x2
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB07_TASK2_SAMPLES_OUTPUT}
        ${LAB07_PLOTS_DIR}/task2_y_histogram.png
        --plot_type histogram_custom_density
        --histogram_column y
        --x_label "y"
        --y_label "density"
        --title "Task 2: Y histogram"
        --bins 100
        --x_min 5
        --x_max 10
        --density_function lab07_task2_y
    DEPENDS run_lab07_task2 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab07 task2 plots"
    VERBATIM
)

add_custom_target(run_lab07_all
    DEPENDS
        run_lab07_task1
        run_lab07_task2
)

add_custom_target(plot_lab07_all
    DEPENDS
        plot_lab07_task1
        plot_lab07_task2
)
