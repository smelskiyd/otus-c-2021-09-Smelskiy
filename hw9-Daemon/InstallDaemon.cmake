# Set output directory for binaries
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/)

# Directory with configuration files
set(DAEMON_CONF_DIR "/tmp/my-daemon/")

# Directory with systemd unit files
set(SYSTEMD_UNIT_DIR "/etc/systemd/system/")

# Default directory for PID file
set(DAEMON_PID_DIR "/tmp/my-daemon/")

# Macro for installing configuration files
function(install_conf src dest)
    if(NOT IS_ABSOLUTE "${src}")
        set(src "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    endif()
    get_filename_component(src_name "${src}" NAME)
    if (NOT IS_ABSOLUTE "${dest}")
        set(dest "${CMAKE_INSTALL_PREFIX}/${dest}")
    endif()
    install(CODE "
    if(NOT EXISTS \"\$ENV{DESTDIR}${dest}/${src_name}\")
      #file(INSTALL \"${src}\" DESTINATION \"${dest}\")
      message(STATUS \"Installing: \$ENV{DESTDIR}${dest}/${src_name}\")
      execute_process(COMMAND \${CMAKE_COMMAND} -E copy \"${src}\"
                      \"\$ENV{DESTDIR}${dest}/${src_name}\"
                      RESULT_VARIABLE copy_result
                      ERROR_VARIABLE error_output)
      if(copy_result)
        message(FATAL_ERROR \${error_output})
      endif()
    else()
      message(STATUS \"Skipping  : \$ENV{DESTDIR}${dest}/${src_name}\")
    endif()
  ")
endfunction(install_conf)

# Install systemd unit files
install_conf(./my-daemon.service ${SYSTEMD_UNIT_DIR})

# Install configuration file
install_conf(./my-daemon.conf ${DAEMON_CONF_DIR})

# Create empty directory for default PID file
install(DIRECTORY DESTINATION ${DAEMON_PID_DIR})

install(TARGETS OtusHW9_MyDaemon
            RUNTIME
            DESTINATION /bin)
