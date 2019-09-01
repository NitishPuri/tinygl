
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

add_executable(unit_tests 
                "${PROJECT_SOURCE_DIR}/tinyrenderer/src/tgaimage.cpp"
                "tests.cpp")
                
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


# add_test(test_tinyrenderer test_tinyrenderer)