cmake_minimum_required (VERSION 3.8)
project ("kbMouselook" VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set ( APP_NAME "kbmouselook" )
set ( SRC_FOLDER "${CMAKE_SOURCE_DIR}/src" )
set ( HEADER_FOLDER "${CMAKE_SOURCE_DIR}/src/headers" )
# Ajoutez une source à l'exécutable de ce projet.
add_executable ( ${APP_NAME} "${SRC_FOLDER}/kbmouselook.cpp"
    "${HEADER_FOLDER}/hotkey_action.cpp" "${HEADER_FOLDER}/sendinput_funcs.cpp" )

target_include_directories( ${APP_NAME} PRIVATE ${HEADER_FOLDER}00 )

if(WIN32)
    target_link_libraries(${APP_NAME} PRIVATE Winmm)
endif()

if(MSVC)
    target_sources(${APP_NAME} PRIVATE "${SRC_FOLDER}/resources/resources.rc")
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
