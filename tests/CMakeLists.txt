add_executable(PointerTests pointer.cpp)
target_link_libraries(PointerTests PRIVATE GTest::gtest_main PRIVATE SimpleCPP)