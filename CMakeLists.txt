cmake_minimum_required(VERSION 3.21)

# ws-error-embedded-c — typed error values for embedded C.
# Zero dependencies; builds standalone or when linked from $WS_ROOT/ws-libs/.
project(ws-error-embedded-c
    VERSION 0.1.0
    DESCRIPTION "Typed error values for embedded C firmware (ws-error-embedded-c)"
    LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(WS_ERROR_WARNING_FLAGS -Wall -Wextra -Werror)
else()
    set(WS_ERROR_WARNING_FLAGS "")
endif()

add_library(ws-error-embedded-c STATIC src/error.c)
add_library(ws-error::ws-error-embedded-c ALIAS ws-error-embedded-c)

target_include_directories(ws-error-embedded-c
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_compile_options(ws-error-embedded-c PRIVATE ${WS_ERROR_WARNING_FLAGS})

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTING)
        add_executable(error_test tests/error_test.c)
        target_link_libraries(error_test PRIVATE ws-error-embedded-c)
        target_compile_options(error_test PRIVATE ${WS_ERROR_WARNING_FLAGS})
        add_test(NAME error_test COMMAND error_test)

        add_executable(basic_error_example examples/basic_error.c)
        target_link_libraries(basic_error_example PRIVATE ws-error-embedded-c)
        target_compile_options(basic_error_example PRIVATE ${WS_ERROR_WARNING_FLAGS})
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS ws-error-embedded-c
    EXPORT ws-error-embedded-c-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ws-error-embedded-c-targets
    NAMESPACE ws-error::
    FILE ws-error-embedded-c-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ws-error-embedded-c)
