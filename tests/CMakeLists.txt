include(FetchContent)

# Download gtest if not provided
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(config)

if(WITH_SNOWBALLS_SERVER)
  add_subdirectory(server)
endif()

if(WITH_SNOWBALLS_CLIENT)
  add_subdirectory(client)
endif()

if (NOT TARGET unit_test)
    add_custom_target(unit_test)
endif()

function(add_test_executable name)
    add_executable(${ARGV})
    target_link_libraries("${name}"
            GTest::gtest_main
            GTest::gmock
    )

    add_test(NAME "${name}" COMMAND "${name}")

    add_dependencies(unit_test "${name}")
endfunction()