# LibCURL
add_subdirectory(curl)

# Jansson - easy to use JSON parser for C
set(JANSSON_BUILD_DOCS OFF CACHE BOOL "Disable building DOCS for Jansson" FORCE)
set(JANSSON_WITHOUT_TESTS ON CACHE BOOL "Disable building tests for Jansson" FORCE)
set(JANSSON_INSTALL OFF CACHE BOOL "Disable installation for Jansson" FORCE)
set(JANSSON_EXAMPLES OFF CACHE BOOL "Disable building examples for Jansson" FORCE)
add_subdirectory(jansson)
