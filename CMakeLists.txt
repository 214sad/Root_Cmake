cmake_minimum_required(VERSION 3.10)
project(demo)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/INSTALL)

find_package(ROOT REQUIRED COMPONENTS RIO Net Graf Gui)

#add_library(graph_shared SHARED demo1.cpp)

#target_link_libraries(graph_shared PUBLIC ROOT::RIO ROOT::Net)

#set()

add_executable(main demo1.cpp)

get_target_property(ROOT_LIB_LOCATION ROOT::Core LOCATION)
get_filename_component(ROOT_LIBRARY_DIR ${ROOT_LIB_LOCATION} DIRECTORY)

set_target_properties(main PROPERTIES
  INSTALL_RPATH "${ROOT_LIBRARY_DIR}"
)



target_link_libraries(main PUBLIC ROOT::RIO ROOT::Net ROOT::Graf ROOT::Gui)

install(TARGETS main 
      RUNTIME DESTINATION bin

)
