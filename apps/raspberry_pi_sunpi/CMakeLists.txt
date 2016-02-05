cmake_minimum_required(VERSION 2.8)
project(unabto_raspberrypi)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../build/cmake/unabto_project.cmake)

message("Making makefiles to raspberry pi SunPi demo")

set(src
  ${unabto_src}
  ${unabto_gopt_src}  
  ${CMAKE_CURRENT_SOURCE_DIR}/src/unabto_main.c
)

list(APPEND src ${CMAKE_CURRENT_SOURCE_DIR}/src/unabto_application.c)


include_directories(
  ${unabto_include_directories}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_definitions(${unabto_definitions})

add_executable(${PROJECT_NAME} ${src})

target_link_libraries(${PROJECT_NAME} ${unabto_link_libraries} wiringPi)

install( TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
