add_library(lab03_utils src/utils.cpp)
target_include_directories(lab03_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(lab03_utils PUBLIC cxx_std_20)

add_executable(lab03_task1 task1.cpp)
add_executable(lab03_task2 task2.cpp)
add_executable(lab03_task3 task3.cpp)
add_executable(lab03_task4 task4.cpp)
add_executable(lab03_task5 task5.cpp)

find_package(GSL REQUIRED)
target_link_libraries(lab03_utils PUBLIC GSL::gsl GSL::gslcblas)
target_link_libraries(lab03_task1 PRIVATE lab03_utils)
target_link_libraries(lab03_task2 PRIVATE lab03_utils)
target_link_libraries(lab03_task3 PRIVATE lab03_utils)
target_link_libraries(lab03_task4 PRIVATE lab03_utils)
target_link_libraries(lab03_task5 PRIVATE lab03_utils)
target_compile_features(lab03_task1 PRIVATE cxx_std_20)
target_compile_features(lab03_task2 PRIVATE cxx_std_20)
target_compile_features(lab03_task3 PRIVATE cxx_std_20)
target_compile_features(lab03_task4 PRIVATE cxx_std_20)
target_compile_features(lab03_task5 PRIVATE cxx_std_20)

set(LAB03_RESULTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results)
set(LAB03_PLOTS_DIR ${LAB03_RESULTS_DIR}/plots)
set(MATPLOTLIB_CONFIG_DIR ${CMAKE_BINARY_DIR}/matplotlib-cache)
set(PLOT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/plot.py)

set(LAB03_TASK1_OUTPUT ${LAB03_RESULTS_DIR}/task1_clt_normal.txt)
set(LAB03_TASK1_ARGS
    100000
    1234
    ${LAB03_TASK1_OUTPUT}
)

set(LAB03_TASK2_SAMPLE_PAIR_COUNT 100000)
set(LAB03_TASK2_TIMING_PAIR_COUNT 1000000)
set(LAB03_TASK2_ARGS
    ${LAB03_TASK2_SAMPLE_PAIR_COUNT}
    ${LAB03_TASK2_TIMING_PAIR_COUNT}
    1234
    ${LAB03_RESULTS_DIR}
)

set(LAB03_TASK3_SAMPLE_COUNT 100000)
set(LAB03_TASK3_TIMING_COUNT 1000000)
set(LAB03_TASK3_ARGS
    ${LAB03_TASK3_SAMPLE_COUNT}
    ${LAB03_TASK3_TIMING_COUNT}
    1234
    ${LAB03_RESULTS_DIR}
)

set(LAB03_TASK4_SAMPLE_COUNT 100000)
set(LAB03_TASK4_ARGS
    ${LAB03_TASK4_SAMPLE_COUNT}
    ${LAB03_RESULTS_DIR}
)

set(LAB03_TASK5_SAMPLE_COUNT 100000)
set(LAB03_TASK5_ARGS
    ${LAB03_TASK5_SAMPLE_COUNT}
    ${LAB03_RESULTS_DIR}
)

add_custom_target(run_lab03_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_RESULTS_DIR}
    COMMAND lab03_task1 ${LAB03_TASK1_ARGS}
    DEPENDS lab03_task1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab03 task1"
    VERBATIM
)

add_custom_target(plot_lab03_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_TASK1_OUTPUT}
        ${LAB03_PLOTS_DIR}/task1_clt_normal_histogram.png
        --plot_type histogram_normal
        --x_label x
        --y_label density
        --title "Task 1: CLT approximation of N(0,1)"
        --bins 50
        --x_min -4
        --x_max 4
    DEPENDS run_lab03_task1 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab03 task1 histogram with normal density"
    VERBATIM
)

add_custom_target(run_lab03_task2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_RESULTS_DIR}
    COMMAND lab03_task2 ${LAB03_TASK2_ARGS}
    DEPENDS lab03_task2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab03 task2"
    VERBATIM
)

add_custom_target(plot_lab03_task2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task2_box_muller_x.txt
        ${LAB03_PLOTS_DIR}/task2_box_muller_x_histogram.png
        --plot_type histogram_normal
        --x_label x
        --y_label density
        --title "Task 2: Box-Muller, X"
        --bins 50
        --x_min -4
        --x_max 4
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task2_box_muller_y.txt
        ${LAB03_PLOTS_DIR}/task2_box_muller_y_histogram.png
        --plot_type histogram_normal
        --x_label y
        --y_label density
        --title "Task 2: Box-Muller, Y"
        --bins 50
        --x_min -4
        --x_max 4
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task2_marsaglia_bray_x.txt
        ${LAB03_PLOTS_DIR}/task2_marsaglia_bray_x_histogram.png
        --plot_type histogram_normal
        --x_label x
        --y_label density
        --title "Task 2: Marsaglia-Bray, X"
        --bins 50
        --x_min -4
        --x_max 4
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task2_marsaglia_bray_y.txt
        ${LAB03_PLOTS_DIR}/task2_marsaglia_bray_y_histogram.png
        --plot_type histogram_normal
        --x_label y
        --y_label density
        --title "Task 2: Marsaglia-Bray, Y"
        --bins 50
        --x_min -4
        --x_max 4
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task2_timings.csv
        ${LAB03_PLOTS_DIR}/task2_timings.png
        --plot_type bar
        --y_label "time [us]"
        --title "Task 2: generation time comparison"
        --value_column time_us
        --sort
    DEPENDS run_lab03_task2 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab03 task2 plots"
    VERBATIM
)

add_custom_target(run_lab03_task3
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_RESULTS_DIR}
    COMMAND lab03_task3 ${LAB03_TASK3_ARGS}
    DEPENDS lab03_task3
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab03 task3"
    VERBATIM
)

add_custom_target(plot_lab03_task3
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task3_acceptance_rejection.txt
        ${LAB03_PLOTS_DIR}/task3_acceptance_rejection_histogram.png
        --plot_type histogram_custom_density
        --x_label x
        --y_label density
        --title "Task 3: acceptance-rejection"
        --bins 50
        --x_min 0
        --x_max 2
        --density_function lab03_task3
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task3_superposition.txt
        ${LAB03_PLOTS_DIR}/task3_superposition_histogram.png
        --plot_type histogram_custom_density
        --x_label x
        --y_label density
        --title "Task 3: superposition"
        --bins 50
        --x_min 0
        --x_max 2
        --density_function lab03_task3
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task3_timings.csv
        ${LAB03_PLOTS_DIR}/task3_timings.png
        --plot_type bar
        --y_label "time [us]"
        --title "Task 3: method efficiency comparison"
        --value_column time_us
        --sort
    DEPENDS run_lab03_task3 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab03 task3 plots"
    VERBATIM
)

add_custom_target(run_lab03_task4
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_RESULTS_DIR}
    COMMAND lab03_task4 ${LAB03_TASK4_ARGS}
    DEPENDS lab03_task4
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab03 task4"
    VERBATIM
)

add_custom_target(plot_lab03_task4
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task4_laplace.txt
        ${LAB03_PLOTS_DIR}/task4_laplace_histogram.png
        --plot_type histogram_custom_density
        --x_label x
        --y_label density
        --title "Task 4: Laplace distribution"
        --bins 50
        --x_min -6
        --x_max 6
        --density_function lab03_task4_laplace
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task4_pareto.txt
        ${LAB03_PLOTS_DIR}/task4_pareto_histogram.png
        --plot_type histogram_custom_density
        --x_label x
        --y_label density
        --title "Task 4: Pareto distribution"
        --bins 50
        --x_min 1
        --x_max 8
        --density_function lab03_task4_pareto
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task4_gamma.txt
        ${LAB03_PLOTS_DIR}/task4_gamma_histogram.png
        --plot_type histogram_custom_density
        --x_label x
        --y_label density
        --title "Task 4: Gamma distribution"
        --bins 50
        --x_min 0
        --x_max 20
        --density_function lab03_task4_gamma
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task4_dirichlet.csv
        ${LAB03_PLOTS_DIR}/task4_dirichlet_scatter.png
        --plot_type scatter
        --x_label theta1
        --y_label theta2
        --title "Task 4: Dirichlet samples (theta1 vs theta2)"
    DEPENDS run_lab03_task4 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab03 task4 plots"
    VERBATIM
)

add_custom_target(run_lab03_task5
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_RESULTS_DIR}
    COMMAND lab03_task5 ${LAB03_TASK5_ARGS}
    DEPENDS lab03_task5
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab03 task5"
    VERBATIM
)

add_custom_target(plot_lab03_task5
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB03_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task5_cauchy.txt
        ${LAB03_PLOTS_DIR}/task5_cauchy_histogram.png
        --plot_type histogram_custom_density
        --x_label x
        --y_label density
        --title "Task 5: Cauchy distribution"
        --bins 50
        --x_min -10
        --x_max 10
        --density_function lab03_task5_cauchy
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB03_RESULTS_DIR}/task5_triangular.txt
        ${LAB03_PLOTS_DIR}/task5_triangular_histogram.png
        --plot_type histogram_custom_density
        --x_label x
        --y_label density
        --title "Task 5: triangular distribution"
        --bins 50
        --x_min 0
        --x_max 2
        --density_function lab03_task5_triangular
    DEPENDS run_lab03_task5 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab03 task5 plots"
    VERBATIM
)

add_custom_target(run_lab03_all
    DEPENDS
        run_lab03_task1
        run_lab03_task2
        run_lab03_task3
        run_lab03_task4
        run_lab03_task5
    COMMENT "Running all lab03 tasks"
)

add_custom_target(plot_lab03_all
    DEPENDS
        plot_lab03_task1
        plot_lab03_task2
        plot_lab03_task3
        plot_lab03_task4
        plot_lab03_task5
    COMMENT "Generating all lab03 plots"
)
