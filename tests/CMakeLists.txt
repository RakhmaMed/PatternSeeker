cmake_minimum_required(VERSION 3.12)

# ============================================================================
# Test executable
# ============================================================================

add_executable(test_pattern_seeker
    test_main.cpp
)

target_link_libraries(test_pattern_seeker
    PRIVATE
        PatternSeeker::PatternSeeker
)

# Add test
add_test(NAME test_pattern_seeker COMMAND test_pattern_seeker)

# ============================================================================
# Compiler settings
# ============================================================================

if(MSVC)
    target_compile_options(test_pattern_seeker PRIVATE /W4)
else()
    target_compile_options(test_pattern_seeker PRIVATE -Wall -Wextra -Wpedantic)
endif()