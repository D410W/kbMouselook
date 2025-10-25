cmake_minimum_required (VERSION 3.8)
project ("Mouselook")

# include(FetchContent)

# FetchContent_Declare(
#     MyExternalLib
#     GIT_REPOSITORY https://github.com/user/myexternallib.git
#     GIT_TAG v1.0.0 # Or a specific commit hash
# )

# FetchContent_MakeAvailable(MyExternalLib)

set ( APP_NAME "kbmouselook" )
set ( SRC_FOLDER "${CMAKE_SOURCE_DIR}/src" )
set ( HEADER_FOLDER "${CMAKE_SOURCE_DIR}/src/headers" )
# Ajoutez une source à l'exécutable de ce projet.
add_executable ( ${APP_NAME} "${SRC_FOLDER}/mouselook.cpp"
    "${HEADER_FOLDER}/hotkey_action.cpp" "${HEADER_FOLDER}/sendinput_funcs.cpp" )

target_include_directories( ${APP_NAME} PRIVATE ${HEADER_FOLDER} )
# target_link_libraries ( ${APP_NAME} PRIVATE MyExternalLib::target_name )
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
