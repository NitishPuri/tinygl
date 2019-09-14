
#add_library(Catch2::Catch IMPORTED INTERFACE)
#set_property(Catch2::Catch PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CATCH_INCLUDE_DIR}")

include(CTest)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
enable_testing()

add_library(Catch INTERFACE)
target_include_directories(Catch
	INTERFACE
		"${PROJECT_SOURCE_DIR}/extern/catch/"
)

FILE(GLOB TINY_SOURCE ${PROJECT_SOURCE_DIR}/tinygl/src/*.cpp)

add_executable(unit_tests 
                ${TINY_SOURCE}
                "tests.cpp")

target_link_libraries(${unit_tests} tinygl)    
                
target_include_directories(unit_tests
	PUBLIC
		"${PROJECT_SOURCE_DIR}/extern/catch/"
        "${PROJECT_SOURCE_DIR}/src/"
)
target_link_libraries(unit_tests
	Catch
)

include(ParseAndAddCatchTests)
set (PARSE_CATCH_TESTS_VERBOSE OFF)
message("PARSE_CATCH_TESTS_VERBOSE is ${PARSE_CATCH_TESTS_VERBOSE}")
ParseAndAddCatchTests(unit_tests)


# add_test(test_tinygl test_tinygl)