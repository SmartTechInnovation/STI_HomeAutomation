# =============================================================================
# STIx Config tests (story 1.1)
#
# Every file named tst_<name>.cpp under tests/unit/ or tests/integration/ becomes
# its own test executable automatically - no edit of this file is needed.
#
# A test that needs extra source files can be declared explicitly, in a
# tests/tools/<name>.cmake file (included automatically):
#     stix_add_test(tst_foo  unit/foo/tst_foo.cpp  unit/foo/helper.cpp)
# Files listed that way are skipped by the auto-discovery.
# =============================================================================

set(STIX_FIXTURES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fixtures")
set_property(GLOBAL PROPERTY STIX_EXPLICIT_TEST_SOURCES "")

function(stix_add_test name)
    set(sources ${ARGN})
    if(NOT sources)
        message(FATAL_ERROR "stix_add_test(${name}) needs at least one source file")
    endif()
    add_executable(${name} ${sources})
    target_link_libraries(${name} PRIVATE stix_core Qt6::Test)
    target_compile_definitions(${name} PRIVATE STIX_FIXTURES_DIR="${STIX_FIXTURES_DIR}")
    set_target_properties(${name} PROPERTIES MACOSX_BUNDLE FALSE WIN32_EXECUTABLE FALSE)
    add_test(NAME ${name} COMMAND ${name})
    # Run headless (no window server needed, e.g. in CI). Tests that touch
    # QStandardPaths must call QStandardPaths::setTestModeEnabled(true).
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

    foreach(src IN LISTS sources)
        get_filename_component(abs ${src} ABSOLUTE)
        set_property(GLOBAL APPEND PROPERTY STIX_EXPLICIT_TEST_SOURCES ${abs})
    endforeach()
endfunction()

# Explicit declarations and extra test checks (e.g. tests/tools/check_tokens.cmake)
file(GLOB STIX_TEST_TOOLS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/*.cmake")
list(SORT STIX_TEST_TOOLS)
foreach(stix_test_tool IN LISTS STIX_TEST_TOOLS)
    include(${stix_test_tool})
endforeach()

# Auto-discovery: one executable per tst_*.cpp file
file(GLOB_RECURSE STIX_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/tst_*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/integration/tst_*.cpp"
)
list(SORT STIX_TEST_SOURCES)
get_property(STIX_EXPLICIT GLOBAL PROPERTY STIX_EXPLICIT_TEST_SOURCES)
foreach(test_source IN LISTS STIX_TEST_SOURCES)
    if(test_source IN_LIST STIX_EXPLICIT)
        continue()
    endif()
    get_filename_component(test_name ${test_source} NAME_WE)
    if(TARGET ${test_name})
        message(FATAL_ERROR "Two test files are named ${test_name}.cpp; test file names must be unique.")
    endif()
    stix_add_test(${test_name} ${test_source})
endforeach()
