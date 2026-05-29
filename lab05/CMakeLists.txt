add_executable(lab05_task1 task1.cpp sir_model.cpp)
target_compile_features(lab05_task1 PRIVATE cxx_std_20)

set(LAB05_RESULTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results)
set(LAB05_PLOTS_DIR ${LAB05_RESULTS_DIR}/plots)
set(MATPLOTLIB_CONFIG_DIR ${CMAKE_BINARY_DIR}/matplotlib-cache)
set(PLOT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/plot.py)

set(LAB05_TASK1_TRAJECTORIES ${LAB05_RESULTS_DIR}/task1_trajectories.csv)
set(LAB05_TASK1_FORECAST_SAMPLES ${LAB05_RESULTS_DIR}/task1_forecast_samples.csv)
set(LAB05_TASK1_FORECAST_SUMMARY ${LAB05_RESULTS_DIR}/task1_forecast_summary.csv)
set(LAB05_TASK1_CONDITIONAL_FORECAST_SAMPLES ${LAB05_RESULTS_DIR}/task1_conditional_forecast_samples.csv)
set(LAB05_TASK1_CONDITIONAL_FORECAST_SUMMARY ${LAB05_RESULTS_DIR}/task1_conditional_forecast_summary.csv)

add_custom_target(run_lab05_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB05_RESULTS_DIR}
    COMMAND lab05_task1 ${LAB05_RESULTS_DIR}
    DEPENDS lab05_task1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running lab05 task1"
    VERBATIM
)

add_custom_target(plot_lab05_task1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LAB05_PLOTS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATPLOTLIB_CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB05_TASK1_TRAJECTORIES}
        ${LAB05_PLOTS_DIR}/task1_infected_trajectories.png
        --plot_type multi_series
        --x_label "day"
        --y_label "infectious"
        --title "Lab05 Task 1: infectious trajectories"
        --x_column day
        --value_column infectious
        --series_column series
        --hide_legend
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB05_TASK1_FORECAST_SAMPLES}
        ${LAB05_PLOTS_DIR}/task1_peak_day_distribution.png
        --plot_type histogram
        --x_label "peak day"
        --y_label "count"
        --title "Lab05 Task 1: predicted peak day distribution"
        --histogram_column peak_day
        --bins 35
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB05_TASK1_FORECAST_SAMPLES}
        ${LAB05_PLOTS_DIR}/task1_peak_infected_distribution.png
        --plot_type histogram
        --x_label "peak infectious"
        --y_label "count"
        --title "Lab05 Task 1: predicted peak size distribution"
        --histogram_column peak_infected
        --bins 35
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB05_TASK1_CONDITIONAL_FORECAST_SAMPLES}
        ${LAB05_PLOTS_DIR}/task1_conditional_peak_day_distribution.png
        --plot_type histogram
        --x_label "peak day"
        --y_label "count"
        --title "Lab05 Task 1: conditional peak day forecast"
        --histogram_column peak_day
        --bins 35
    COMMAND ${CMAKE_COMMAND} -E env MPLCONFIGDIR=${MATPLOTLIB_CONFIG_DIR} ${PROJECT_VENV_PYTHON} ${PLOT_SCRIPT}
        ${LAB05_TASK1_CONDITIONAL_FORECAST_SAMPLES}
        ${LAB05_PLOTS_DIR}/task1_conditional_peak_infected_distribution.png
        --plot_type histogram
        --x_label "peak infectious"
        --y_label "count"
        --title "Lab05 Task 1: conditional peak size forecast"
        --histogram_column peak_infected
        --bins 35
    DEPENDS run_lab05_task1 python_venv
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating lab05 task1 plots"
    VERBATIM
)

add_custom_target(run_lab05_all
    DEPENDS run_lab05_task1
)

add_custom_target(plot_lab05_all
    DEPENDS plot_lab05_task1
)
