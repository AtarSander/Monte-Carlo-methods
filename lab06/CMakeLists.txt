add_executable(
    lab06_task1
    task1.cpp
    ising/model_isinga.cpp
    ising/rng.cpp
    ising/utils.cpp
)
add_executable(
    lab06_task2
    task2.cpp
    ising/model_isinga.cpp
    ising/rng.cpp
    ising/utils.cpp
)

target_include_directories(lab06_task1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ising)
target_include_directories(lab06_task2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ising)
target_compile_features(lab06_task1 PRIVATE cxx_std_20)
target_compile_features(lab06_task2 PRIVATE cxx_std_20)

set(LAB06_RESULTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results)
set(LAB06_PLOTS_DIR ${LAB06_RESULTS_DIR}/plots)
set(MATPLOTLIB_CONFIG_DIR ${CMAKE_BINARY_DIR}/matplotlib-cache)
set(PLOT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/plot.py)

set(LAB06_TASK1_OUTPUT ${LAB06_RESULTS_DIR}/task1_ising_microcanonical.csv)
set(LAB06_TASK1_ARGS
    10000
    10000
    ${LAB06_TASK1_OUTPUT}
)

set(LAB06_TASK2_OUTPUT ${LAB06_RESULTS_DIR}/task2_ising_canonical.csv)
set(LAB06_TASK2_ARGS
    10000
    10000
    ${LAB06_TASK2_OUTPUT}
)

add_custom_target(run_lab06_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB06_RESULTS_DIR}
    COMMAND lab06_task1 ${LAB06_TASK1_ARGS}
    DEPENDS lab06_task1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab06 task1"
    VERBATIM
)

add_custom_target(plot_lab06_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB06_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB06_TASK1_OUTPUT}
        ${LAB06_PLOTS_DIR}/task1_mean_magnetization_vs_temperature.png
        --plot_type multi_series
        --x_label "temperature"
        --y_label "mean magnetization"
        --title "Task 1: mean magnetization vs temperature"
        --x_column temperature
        --value_column mean_magnetization
        --series_column series
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB06_TASK1_OUTPUT}
        ${LAB06_PLOTS_DIR}/task1_mean_energy_vs_temperature.png
        --plot_type multi_series
        --x_label "temperature"
        --y_label "mean system energy"
        --title "Task 1: mean system energy vs temperature"
        --x_column temperature
        --value_column mean_system_energy
        --series_column series
    DEPENDS run_lab06_task1 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab06 task1 plots"
    VERBATIM
)

add_custom_target(run_lab06_task2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB06_RESULTS_DIR}
    COMMAND lab06_task2 ${LAB06_TASK2_ARGS}
    DEPENDS lab06_task2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab06 task2"
    VERBATIM
)

add_custom_target(plot_lab06_task2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB06_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB06_TASK2_OUTPUT}
        ${LAB06_PLOTS_DIR}/task2_mean_magnetization_vs_temperature.png
        --plot_type multi_series
        --x_label "temperature"
        --y_label "mean magnetization"
        --title "Task 2: mean magnetization vs temperature"
        --x_column temperature
        --value_column mean_magnetization
        --series_column series
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB06_TASK2_OUTPUT}
        ${LAB06_PLOTS_DIR}/task2_mean_energy_vs_temperature.png
        --plot_type multi_series
        --x_label "temperature"
        --y_label "mean system energy"
        --title "Task 2: mean system energy vs temperature"
        --x_column temperature
        --value_column mean_system_energy
        --series_column series
    DEPENDS run_lab06_task2 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab06 task2 plots"
    VERBATIM
)

add_custom_target(run_lab06_all
    DEPENDS
        run_lab06_task1
        run_lab06_task2
)

add_custom_target(plot_lab06_all
    DEPENDS
        plot_lab06_task1
        plot_lab06_task2
)
