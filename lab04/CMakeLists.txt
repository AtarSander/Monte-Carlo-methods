add_library(lab04_utils src/utils.cpp)
target_include_directories(lab04_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(lab04_utils PUBLIC cxx_std_20)

add_executable(lab04_task3 task3.cpp)
add_executable(lab04_task6 task6.cpp)
target_compile_features(lab04_task3 PRIVATE cxx_std_20)
target_compile_features(lab04_task6 PRIVATE cxx_std_20)

find_package(GSL REQUIRED)
target_link_libraries(lab04_utils PUBLIC GSL::gsl GSL::gslcblas)
target_link_libraries(lab04_task3 PRIVATE lab04_utils)
target_link_libraries(lab04_task6 PRIVATE lab04_utils)

set(LAB04_RESULTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results)
set(LAB04_PLOTS_DIR ${LAB04_RESULTS_DIR}/plots)
set(MATPLOTLIB_CONFIG_DIR ${CMAKE_BINARY_DIR}/matplotlib-cache)
set(PLOT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/plot.py)

set(LAB04_TASK3_OUTPUT ${LAB04_RESULTS_DIR}/task3_comparison.csv)
set(LAB04_TASK3_ARGS
    500000000
    1234
    ${LAB04_TASK3_OUTPUT}
)

set(LAB04_TASK6_OUTPUT_1D ${LAB04_RESULTS_DIR}/task6_1d_comparison.csv)
set(LAB04_TASK6_OUTPUT_3D ${LAB04_RESULTS_DIR}/task6_3d_comparison.csv)
set(LAB04_TASK6_ARGS
    50000000
    1234
    ${LAB04_TASK6_OUTPUT_1D}
    ${LAB04_TASK6_OUTPUT_3D}
)

add_custom_target(run_lab04_task3
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB04_RESULTS_DIR}
    COMMAND lab04_task3 ${LAB04_TASK3_ARGS}
    DEPENDS lab04_task3
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab04 task3"
    VERBATIM
)

add_custom_target(plot_lab04_task3
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB04_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB04_TASK3_OUTPUT}
        ${LAB04_PLOTS_DIR}/task3_metrics.png
        --plot_type grouped_multi_value_series
        --x_label "number of samples"
        --y_label "value"
        --title "Task 3: uncertainty and absolute error"
        --x_column samples
        --series_column method
        --value_columns uncertainty abs_error
        --value_labels uncertainty absolute_error
        --log_x
        --log_y
    DEPENDS run_lab04_task3 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab04 task3 plots"
    VERBATIM
)

add_custom_target(run_lab04_task6
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB04_RESULTS_DIR}
    COMMAND lab04_task6 ${LAB04_TASK6_ARGS}
    DEPENDS lab04_task6
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab04 task6"
    VERBATIM
)

add_custom_target(plot_lab04_task6
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB04_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB04_TASK6_OUTPUT_1D}
        ${LAB04_PLOTS_DIR}/task6_1d_metrics.png
        --plot_type grouped_multi_value_series
        --x_label "number of samples"
        --y_label "value"
        --title "Task 6: 1D uncertainty and absolute error"
        --x_column samples
        --series_column method
        --value_columns uncertainty abs_error
        --value_labels uncertainty absolute_error
        --log_x
        --log_y
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB04_TASK6_OUTPUT_3D}
        ${LAB04_PLOTS_DIR}/task6_3d_metrics.png
        --plot_type grouped_multi_value_series
        --x_label "number of samples"
        --y_label "value"
        --title "Task 6: 3D uncertainty and absolute error"
        --x_column samples
        --series_column method
        --value_columns uncertainty abs_error
        --value_labels uncertainty absolute_error
        --log_x
        --log_y
    DEPENDS run_lab04_task6 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab04 task6 plots"
    VERBATIM
)

add_custom_target(run_lab04_all
    DEPENDS
        run_lab04_task3
        run_lab04_task6
)

add_custom_target(plot_lab04_all
    DEPENDS
        plot_lab04_task3
        plot_lab04_task6
)
