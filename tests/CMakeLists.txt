set(TEST_LIBS pthread rt glstreamer)

add_executable(main-test main.cpp testfactory.cpp)

target_link_libraries(main-test ${TEST_LIBS})

add_executable(processlink-provider processlink_provider.cpp testfactory.cpp)
add_executable(processlink-consumer processlink_consumer.cpp testfactory.cpp)
add_executable(case2 case2.cpp case2-test.cpp)
add_executable(deadlock deadlock.cpp)

target_link_libraries(processlink-provider ${TEST_LIBS})
target_link_libraries(processlink-consumer ${TEST_LIBS})
target_link_libraries(case2 ${TEST_LIBS})
target_link_libraries(deadlock ${TEST_LIBS})
