# /Share/GenerateModel/tests/CMakeLists.txt

set(TEST_SOURCES
    test_hardware_detector.cpp
    test_project_manager.cpp
)

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # Build each test as a separate executable
    add_executable(${TEST_NAME}
        ${TEST_FILE}
        ${CMAKE_SOURCE_DIR}/src/core/HardwareDetector.cpp
        ${CMAKE_SOURCE_DIR}/src/core/ProjectManager.cpp
    )

    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/core
        ${CMAKE_SOURCE_DIR}/src/bridge
    )

    target_link_libraries(${TEST_NAME} PRIVATE
        Qt6::Core
        Qt6::Test
    )

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
