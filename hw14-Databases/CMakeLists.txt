project(OtusHW14_Database C)

add_executable(OtusHW14_Databases main.c)

# Link libpq
target_include_directories(OtusHW14_Databases PRIVATE /opt/homebrew/opt/libpq/include)
target_link_directories(OtusHW14_Databases PRIVATE /opt/homebrew/opt/libpq/lib)
target_link_libraries(OtusHW14_Databases PRIVATE libpq.a)
