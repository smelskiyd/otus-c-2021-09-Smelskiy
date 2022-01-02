# Logging library
## Overview
- Library implements 7 logging levels:
TRACE, DEBUG, INFO, WARNING, ERROR, FATAL, OFF (disables logging)

- FATAL logging level prints backtrace and terminates the program

- Backtrace is implemented only for UNIX-like operating systems 

## Configure
- Redefine MAX_BACKTRACE_SIZE compile definition to change the depth of backtrace (100 by default)
- Redefine MAX_HEADER_LOG_LENGTH compile definition to change maximum log header size (150 by default)

## Usage
- Use ```PRINT_LOG_{LEVEL}(format, arguments...)``` macro to print the log with {LEVEL} level.

Example:
```c
PRINT_LOG_DEBUG("%s", "debug log example");
PRINT_LOG_INFO("%d", 123");
PRINT_LOG_FATAL("%.3f", 123.123f);
```

- Use ```set_global_log_level(LogLevel)``` to set logging level (DEBUG by default)

- Use ```set_output_log_file(FILE*)``` to print logs to the file
