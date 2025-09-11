cmake_minimum_required(VERSION 3.10)

project(demo)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/INSTALL CACHE PATH "Default prefix of install directories" FORCE)
endif()


find_package(ROOT REQUIRED COMPONENTS Core Gui Tree Gpad )
include(${ROOT_USE_FILE})

#add_library(guitest SHARED guitest.C)
add_executable(guitest guitest.C)

get_target_property(ROOT_LIBRARY_LOCATION ROOT::Core LOCATION)
get_filename_component(ROOT_LIBRARY_DIR ${ROOT_LIBRARY_LOCATION} DIRECTORY)

set_target_properties( guitest PROPERTIES
     INSTALL_RPATH "${ROOT_LIBRARY_DIR}"
  )

target_link_libraries(guitest PUBLIC ${ROOT_LIBRARIES})
target_compile_definitions(guitest PRIVATE STANDALONE)

install(TARGETS guitest
    RUNTIME DESTINATION bin



)



