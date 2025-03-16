cmake_minimum_required(VERSION 3.10)
project(tab_test)

add_executable(${CMAKE_PROJECT_NAME} WIN32
  tab_test.cpp
)

target_compile_options(${CMAKE_PROJECT_NAME}
  PRIVATE
  "$<$<CONFIG:Debug>:/ZI>"
  "-DUNICODE=1"
)
target_link_options(${CMAKE_PROJECT_NAME}
  PRIVATE
  "$<$<CONFIG:Debug>:/SAFESEH:NO>"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  comctl32.lib
)
