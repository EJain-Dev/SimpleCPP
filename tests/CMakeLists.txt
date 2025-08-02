add_executable(POINTER_TESTS pointer.cpp)
target_link_libraries(POINTER_TESTS PRIVATE SimpleCPP PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(POINTER_TESTS)