# formatting definitions
if(COLOR_OUTPUT)
    string(ASCII 27 ESC)
    set(RESET_FORMAT "${ESC}[m")
    set(BOLD "${ESC}[1m")
    set(COLOR_WHITE "${ESC}[37m")
    set(COLOR_WHITE_BOLD "${ESC}[1;37m")
    set(COLOR_RED "${ESC}[31m")
    set(COLOR_RED_BOLD "${ESC}[1;31m")
    set(COLOR_YELLOW "${ESC}[33m")
    set(COLOR_YELLOW_BOLD "${ESC}[1;33m")
    set(COLOR_GREEN "${ESC}[32m")
    set(COLOR_GREEN_BOLD "${ESC}[1;32m")
    set(COLOR_CYAN "${ESC}[36m")
    set(COLOR_CYAN_BOLD "${ESC}[1;36m")
    set(COLOR_BLUE "${ESC}[34m")
    set(COLOR_BLUE_BOLD "${ESC}[1;34m")
    set(COLOR_MAGENTA "${ESC}[35m")
    set(COLOR_MAGENTA_BOLD "${ESC}[1;35m")
else()
    set(RESET_FORMAT "")
    set(BOLD "")
    set(COLOR_WHITE "")
    set(COLOR_WHITE_BOLD "")
    set(COLOR_RED "")
    set(COLOR_RED_BOLD "")
    set(COLOR_YELLOW "")
    set(COLOR_YELLOW_BOLD "")
    set(COLOR_GREEN "")
    set(COLOR_GREEN_BOLD "")
    set(COLOR_CYAN "")
    set(COLOR_CYAN_BOLD "")
    set(COLOR_BLUE "")
    set(COLOR_BLUE_BOLD "")
    set(COLOR_MAGENTA "")
    set(COLOR_MAGENTA_BOLD "")
endif()
# helper functions

function(pad_output RESULT REQUESTED_LENGTH VALUE)
    string(LENGTH "${VALUE}" VALUE_LENGTH)
    math(EXPR NEED_PAD "${REQUESTED_LENGTH} - ${VALUE_LENGTH}")
    if(NEED_PAD GREATER 0)
        math(EXPR REQUIRED_MINUS_ONE "${NEED_PAD} - 1")
        foreach(BLAH RANGE ${NEED_PAD})
            string(APPEND VALUE " ")
        endforeach()
    endif()
    set(${RESULT} "${VALUE}" PARENT_SCOPE)
endfunction()

set(ILLIXR_SUMMARY_PADDING 35 CACHE STRING "Padding of each report summary line")
mark_as_advanced(ILLIXR_SUMMARY_PADDING)

function(report_value value msg)
    pad_output(padded_value ${ILLIXR_SUMMARY_PADDING} "  ${value}")
    message(STATUS "${padded_value}: ${msg}")
endfunction()

function(print_enabled_disabled value msg)
    pad_output(padded_value ${ILLIXR_SUMMARY_PADDING} "  ${msg}")
    if(value)
        message(STATUS "${padded_value}: ${COLOR_GREEN}Enabled${RESET_FORMAT}")
    else()
        message(STATUS "${padded_value}: ${COLOR_YELLOW}Disabled${RESET_FORMAT}")
    endif()
endfunction()

function(report_padded varname)
    pad_output(padded_value ${ILLIXR_SUMMARY_PADDING} " ${varname}")
    message(STATUS "${padded_value}: ${${varname}}")
endfunction()

# write out a script to invoke monado
function(write_monado ver PTR)
    if(CSHELL)
        set(OUTFILE "illixr_monado_${ver}.csh")
        set(XR_OUT "openxr_launch.csh")
        if(TCSHELL)
            file(WRITE ${OUTFILE} "#!/usr/bin/env tcsh\n\n")
            file(WRITE ${XR_OUT} "#!/usr/bin/env tcsh\n\n")
        else()
            file(WRITE ${OUTFILE} "#!/usr/bin/env csh\n\n")
            file(WRITE ${XR_OUT} "#!/usr/bin/env csh\n\n")
        endif()
    else()
        set(OUTFILE "illixr_monado_${ver}.sh")
        set(XR_OUT "openxr_launch.sh")
        file(WRITE ${OUTFILE} "#!/usr/bin/env sh\n\n")
        file(WRITE ${XR_OUT} "#!/usr/bin/env sh\n\n")
    endif()

    file(APPEND ${OUTFILE} "# set environment variables\n")
    file(APPEND ${XR_OUT} "# set environment variables\n")

    file(APPEND ${OUTFILE} "${SET_CMD} IXIR_PTH${SPACER}${CMAKE_INSTALL_PREFIX}/lib\n\n")
    file(APPEND ${XR_OUT} "${SET_CMD} IXIR_PTH${SPACER}${CMAKE_INSTALL_PREFIX}/lib\n\n")

    if(CSHELL)
        file(APPEND ${OUTFILE} "if ( $?LD_LIBRARY_PATH ) then\n")
        file(APPEND ${XR_OUT} "if ( $?LD_LIBRARY_PATH ) then\n")
    else()
        file(APPEND ${OUTFILE} "if [ ! -z \"$LD_LIBRARY_PATH\" ]\nthen\n")
        file(APPEND ${XR_OUT} "if [ ! -z \"$LD_LIBRARY_PATH\" ]\nthen\n")
    endif()
    file(APPEND ${OUTFILE} "  ${SET_CMD} LD_LIBRARY_PATH${SPACER}\${IXIR_PTH}:\${LD_LIBRARY_PATH}\n")
    file(APPEND ${XR_OUT} "  ${SET_CMD} LD_LIBRARY_PATH${SPACER}\${IXIR_PTH}:\${LD_LIBRARY_PATH}\n")
    file(APPEND ${OUTFILE} "else\n")
    file(APPEND ${XR_OUT} "else\n")
    file(APPEND ${OUTFILE} "  ${SET_CMD} LD_LIBRARY_PATH${SPACER}\${IXIR_PTH}\n")
    file(APPEND ${XR_OUT} "  ${SET_CMD} LD_LIBRARY_PATH${SPACER}\${IXIR_PTH}\n")
    if(CSHELL)
        file(APPEND ${OUTFILE} "endif\n\n")
        file(APPEND ${XR_OUT} "endif\n\n")
    else()
        file(APPEND ${OUTFILE} "fi\n\n")
        file(APPEND ${XR_OUT} "fi\n\n")
    endif()

    file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_DATA${SPACER}${CMAKE_SOURCE_DIR}/data/mav0\n")
    if(DEMO_DATA)
        file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_DEMO_DATA${SPACER}${DEMO_DATA}\n")
    else()
        file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_DEMO_DATA${SPACER}${CMAKE_CURRENT_SOURCE_DIR}/demo_data\n")
    endif()
    file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_PATH${SPACER}\${IXIR_PTH}/libplugin.main${ILLIXR_BUILD_SUFFIX}.so\n")
    file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_COMP${SPACER}${PTR}\n")
    file(APPEND ${OUTFILE} "${SET_CMD} XRT_TRACING${SPACER}true\n")
    file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_OFFLOAD_ENABLE${SPACER}${ENABLE_OFFLOAD}\n")
    file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_ENABLE_VERBOSE_ERRORS${SPACER}${ENABLE_VERBOSE_ERRORS}\n")
    file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_ALIGNMENT_ENABLE${SPACER}${ENABLE_ALIGNMENT}\n")
    file(APPEND ${OUTFILE} "${SET_CMD} ILLIXR_ENABLE_PRE_SLEEP${SPACER}${ENABLE_PRE_SLEEP}\n\n")

    if(CSHELL)
        file(APPEND ${OUTFILE} "if ( $?XDG_RUNTIME_DIR ) then\n")
        file(APPEND ${XR_OUT} "if ( $?XDG_RUNTIME_DIR ) then\n")
    else()
        file(APPEND ${OUTFILE} "if [ ! -z \"$XDG_RUNTIME_DIR\" ]; then\n")
        file(APPEND ${XR_OUT} "if [ ! -z \"$XDG_RUNTIME_DIR\" ]; then\n")
    endif()
    file(APPEND ${OUTFILE} "  ${SET_CMD} IPC_PTH${SPACER}$XDG_RUNTIME_DIR\n")
    file(APPEND ${XR_OUT} "  ${SET_CMD} IPC_PTH${SPACER}$XDG_RUNTIME_DIR\n")
    file(APPEND ${OUTFILE} "else\n")
    file(APPEND ${XR_OUT} "else\n")
    file(APPEND ${OUTFILE} "  ${SET_CMD} IPC_PTH${SPACER}/tmp\n")
    file(APPEND ${XR_OUT} "  ${SET_CMD} IPC_PTH${SPACER}/tmp\n")
    if(CSHELL)
        file(APPEND ${OUTFILE} "endif\n\n")
        file(APPEND ${XR_OUT} "endif\n\n")
    else()
        file(APPEND ${OUTFILE} "fi\n\n")
        file(APPEND ${XR_OUT} "fi\n\n")
    endif()

    file(APPEND ${OUTFILE} "${SET_CMD} XR_RUNTIME_JSON${SPACER}${MONADO_RUNTIME_${ver}}\n\n")
    file(APPEND ${XR_OUT} "${SET_CMD} XR_RUNTIME_JSON${SPACER}${MONADO_RUNTIME_${ver}}\n\n")
    file(APPEND ${OUTFILE} "rm -f \${IPC_PTH}/monado_comp_ipc\n\n")
    if(CSHELL)
        file(APPEND ${XR_OUT} "set socket_file=\${IPC_PTH}/monado_comp_ipc\n\n")
    else()
        file(APPEND ${XR_OUT} "socket_file=\${IPC_PTH}/monado_comp_ipc\n\n")
    endif()
    file(APPEND ${OUTFILE} "# launch monado service\n")
    file(APPEND ${OUTFILE} "${CMAKE_INSTALL_PREFIX}/bin/monado_${ver}-service\n")

    file(APPEND ${XR_OUT} "# give the monado service script time to clean up socket\n")
    file(APPEND ${XR_OUT} "sleep 3\n\n")
    file(APPEND ${XR_OUT} "# now loop, waiting for the socket file to appear\n")

    if(CSHELL)
        file(APPEND ${XR_OUT} "set timeout=90   # seconds\n\n")
        file(APPEND ${XR_OUT} "set x=0\n")
        file(APPEND ${XR_OUT} "while (\$x <= \$timeout)\n")
        file(APPEND ${XR_OUT} "    if ( -e \"\$socket_file\" ) then\n")
        file(APPEND ${XR_OUT} "        break\n")
        file(APPEND ${XR_OUT} "    endif\n")
        file(APPEND ${XR_OUT} "    sleep 1\n")
        file(APPEND ${XR_OUT} "    @ x= \$x + 1\n")
        file(APPEND ${XR_OUT} "end\n\n")
        file(APPEND ${XR_OUT} "# exit if timeout reached\n")
        file(APPEND ${XR_OUT} "if ( \$x >= \$timeout ) then\n")
        file(APPEND ${XR_OUT} "    echo \"Timeout exceeded waiting for socket to open at\\n \$socket_file\"\n")
        file(APPEND ${XR_OUT} "    exit 1\n")
        file(APPEND ${XR_OUT} "endif\n\n")
    else()
        file(APPEND ${XR_OUT} "timeout=90   # seconds\n\n")
        file(APPEND ${XR_OUT} "x=0\n")
        file(APPEND ${XR_OUT} "while [ \$x -le \$timeout ]\n")
        file(APPEND ${XR_OUT} "do\n")
        file(APPEND ${XR_OUT} "    if [ -e \"\$socket_file\" ]; then\n")
        file(APPEND ${XR_OUT} "        break\n")
        file(APPEND ${XR_OUT} "    fi\n")
        file(APPEND ${XR_OUT} "    sleep 1\n")
        file(APPEND ${XR_OUT} "    x=\$(( \$x + 1 ))\n")
        file(APPEND ${XR_OUT} "done\n\n")
        file(APPEND ${XR_OUT} "# exit if timeout reached\n")
        file(APPEND ${XR_OUT} "if [ \$x -ge \$timeout ]; then\n")
        file(APPEND ${XR_OUT} "    echo \"Timeout exceeded waiting for socket to open at\\n \$socket_file\"\n")
        file(APPEND ${XR_OUT} "    exit 1\n")
        file(APPEND ${XR_OUT} "fi\n\n")
    endif()

    file(APPEND ${XR_OUT} "# launch openxr app\n")
    file(APPEND ${XR_OUT} "${OPENXR_RUNTIME}\n\n")

    execute_process(COMMAND chmod 744 ${CMAKE_SOURCE_DIR}/${OUTFILE})
    execute_process(COMMAND chmod 744 ${CMAKE_SOURCE_DIR}/${XR_OUT})
    message("  ./${OUTFILE}")
    message("\nthen in a seperate terminal run\n")
    message("  ./${XR_OUT}")
endfunction()

# report how to run ILLIXR
macro(giveExecString)
    message("")
    message("After building and installing ILLIXR, use one of the following command(s) to run it.")
    message("${COLOR_YELLOW_BOLD}Note${RESET_FORMAT}: please ensure that ${CMAKE_INSTALL_PREFIX}/bin is in your ${BOLD}PATH${RESET_FORMAT} and ${CMAKE_INSTALL_PREFIX}/lib is in your ${BOLD}LD_LIBRARY_PATH${RESET_FORMAT}")
    message("")
    if(USE_OPENVINS)
        message("To use OpenVINS")
        message("  main${ILLIXR_BUILD_SUFFIX}.exe --yaml=${CMAKE_SOURCE_DIR}/illixr.yaml --vis=openvins")
    endif()
    if(OPENXR_RUNTIME)
        string(REGEX MATCH "tcsh$" TCSHELL $ENV{SHELL})
        string(REGEX MATCH "csh$" CSHELL $ENV{SHELL})
        if(CSHELL)
            set(SET_CMD "setenv")
            set(SPACER " ")
        else()
            set(SET_CMD "export")
            set(SPACER "=")
        endif()
        message("")
        message("To use monado use the following:")
        message("${COLOR_YELLOW_BOLD}Note${RESET_FORMAT}: It is up to you to make sure the plugins listed are compatible with the given monado library.")

        set(PLUGINS_TO_RUN_LIST "")
        foreach(ITEM IN LISTS OPENXR_PLUGINS)
            list(APPEND PLUGINS_TO_RUN_LIST "\${IXIR_PTH}/libplugin.${ITEM}${ILLIXR_BUILD_SUFFIX}.so")
        endforeach()
        string(REPLACE ";" ":" PLUGINS_TO_RUN "${PLUGINS_TO_RUN_LIST}")


        message("")
        message("If using a realsense camera give the camera type as follows")
        message("  ${SET_CMD} REALSENSE_CAM${SPACER}<TYPE>")
        set(BOTH_MONADO false)
        if(MONADO_RUNTIME_vk AND MONADO_RUNTIME_gl)
            set(BOTH_MONADO true)
        endif()
        message("")
        if(MONADO_RUNTIME_vk)
            if(BOTH_MONADO)
                message("To use monado_vk")
            endif()
            write_monado("vk" ${PLUGINS_TO_RUN})
        endif ()
        if(MONADO_RUNTIME_gl)
            if(BOTH_MONADO)
                message("")
                message("To use monado_gl")
            endif()
            write_monado("gl" ${PLUGINS_TO_RUN})
        endif()
    endif()
    message("")
endmacro()

# Report configuration

string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
message("")
message("${COLOR_BLUE_BOLD}-------------------------------------------------------------")
message("------------------- Configuration Options -------------------${RESET_FORMAT}")
if(YAML_FILE)
    report_value("Arguments read from" "Command line and ${YAML_FILE}")
else()
    report_value("Arguments read from" "Command line")
endif()
report_value("Linux vendor" "${OS_FLAVOR}")
report_value("CMAKE_CXX_COMPILER_ID type" "${CMAKE_CXX_COMPILER_ID}")
report_value("CMAKE_CXX_COMPILER_VERSION" "${CMAKE_CXX_COMPILER_VERSION}")
report_value("CMake version"    "${CMAKE_VERSION}")
report_value("CMake generator"  "${CMAKE_GENERATOR}")
report_value("CMake build tool" "${CMAKE_BUILD_TOOL}")
report_value("Build type" "${CMAKE_BUILD_TYPE}")
report_value("Library suffix" "${ILLIXR_BUILD_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}")
report_value("C compilation flags" "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}")
report_value("C++ compilation flags" "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}")
report_value("Install location" "${CMAKE_INSTALL_PREFIX}")

# ------------------------- External Libraries ---------------------------------
message("${COLOR_BLUE_BOLD}External Libraries${RESET_FORMAT}")
foreach(ITEM IN LISTS EXTERNAL_LIBRARIES)
    if(${ITEM}_EXTERNAL)
        report_value("${ITEM}" "${COLOR_CYAN}Download and Install${RESET_FORMAT}")
    else()
        report_value("${ITEM}" "${COLOR_GREEN}Found version ${${ITEM}_VERSION}${RESET_FORMAT}")
    endif()
endforeach()
if(BUILDING_OPENCV)
    report_value("OpenCV" "${COLOR_CYAN}Download and Install${RESET_FORMAT}")
endif()

# ------------------------- Plugins --------------------------------------------
message("${COLOR_BLUE_BOLD}Build Plugins${RESET_FORMAT}")
set(PLUGIN_LINE "plugins: ")
set(VIS_LINE "")
set(FIRST True)
set(HAVE_PLUGIN False)
if(PLUGIN_UNORDERED)
    set(HAVE_PLUGIN True)
    string(APPEND PLUGIN_LINE "${PLUGIN_UNORDERED}")
endif()
foreach(ITEM IN LISTS PLUGIN_LIST)
    string(TOUPPER "USE_${ITEM}" ITEM_UPPER)
    if(NOT PLUGIN_UNORDERED)
        if(${${ITEM_UPPER}})
            if(NOT FIRST)
                string(APPEND PLUGIN_LINE ",")
            else()
                set(FIRST False)
                set(HAVE_PLUGIN True)
            endif()
            string(APPEND PLUGIN_LINE "${ITEM}")
        endif()
    endif()
    print_enabled_disabled("${${ITEM_UPPER}}" "${ITEM}")
    unset(ITEM_UPPER)
endforeach()

foreach(ITEM IN LISTS VISUALIZER_LIST)
    string(TOUPPER "USE_${ITEM}" ITEM_UPPER)
    if(${${ITEM_UPPER}})
        if(VIS_LINE)
            string(APPEND VIS_LINE ",")
        endif()
        string(APPEND VIS_LINE "${ITEM}")
    endif()
    print_enabled_disabled("${${ITEM_UPPER}}" "${ITEM}")
endforeach()
message("")
report_value("Data file" "${DATA_FILE}")
report_value("Generating yaml file" "illixr.yaml")
message("${COLOR_BLUE_BOLD}-------------------------------------------------------------${RESET_FORMAT}")

set(OUTFILE "illixr.yaml")

file(WRITE ${OUTFILE} "")
if(HAVE_PLUGIN)
    file(APPEND ${OUTFILE} "${PLUGIN_LINE}\n")
    if(ILLIXR_RUN_NAMES)
        file(APPEND ${OUTFILE} "run: ${ILLIXR_RUN_NAMES}\n")
    endif()
endif()
if(VIS_LINE)
    file(APPEND ${OUTFILE} "visualizers: ${VIS_LINE}\n")
endif()
if(LOCAL_DATA)
    file(APPEND ${OUTFILE} "data: ${LOCAL_DATA}\n")
elseif(DATA_FILE)
    file(APPEND ${OUTFILE} "data: ${CMAKE_SOURCE_DIR}/data/mav0\n")
endif()
if(DEMO_DATA)
    file(APPEND ${OUTFILE} "demo_data: ${DEMO_DATA}\n")
else()
    file(APPEND ${OUTFILE} "demo_data: ${CMAKE_CURRENT_SOURCE_DIR}/demo_data\n")
endif()
if(CMAKE_INSTALL_PREFIX)
    file(APPEND ${OUTFILE} "install_prefix: ${CMAKE_INSTALL_PREFIX}\n")
endif()
file(APPEND ${OUTFILE} "enable_offload: ${ENABLE_OFFLOAD}\n")
file(APPEND ${OUTFILE} "enable_alignment: ${ENABLE_ALIGNMENT}\n")
file(APPEND ${OUTFILE} "enable_verbose_errors: ${ENABLE_VERBOSE_ERRORS}\n")
file(APPEND ${OUTFILE} "enable_pre_sleep: ${ENABLE_PRE_SLEEP}\n")

giveExecString()

message("${COLOR_BLUE_BOLD}-------------------------------------------------------------${RESET_FORMAT}")
message("")
