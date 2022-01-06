# LibCURL
set(BUILD_CUSTOM_CURL OFF)
if (BUILD_CUSTOM_CURL)
    message(STATUS "Building custom curl library")
    set(CURL_DISABLE_VERBOSE_STRINGS ON CACHE BOOL "Disable verbose strings for CURL" FORCE)
    set(CURL_DISABLE_TESTS ON CACHE BOOL "Disable building tests for CURL" FORCE)
    add_subdirectory(curl)
endif(BUILD_CUSTOM_CURL)

# Jansson - easy to use JSON parser for C
set(JANSSON_BUILD_DOCS OFF CACHE BOOL "Disable building DOCS for Jansson" FORCE)
set(JANSSON_WITHOUT_TESTS ON CACHE BOOL "Disable building tests for Jansson" FORCE)
set(JANSSON_INSTALL OFF CACHE BOOL "Disable installation for Jansson" FORCE)
set(JANSSON_EXAMPLES OFF CACHE BOOL "Disable building examples for Jansson" FORCE)
add_subdirectory(jansson)
