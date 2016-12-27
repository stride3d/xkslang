//
// Copyright (C) 

#ifndef XKSLANG_TESTS_DEFINE_H
#define XKSLANG_TESTS_DEFINE_H

#include <string>
#include <vector>

namespace xkslangtest
{
    enum class Source {
        GLSL,
        HLSL,
    };

    // Enum for shader compilation semantics.
    enum class Semantics {
        OpenGL,
        Vulkan,
    };

    // Enum for compilation target.
    enum class Target {
        AST,
        Spv,
        BothASTAndSpv,
    };

    struct ShaderResult {
        std::string shaderName;
        std::string output;
        std::string error;
    };

    // A struct for holding all the information returned by glslang compilation and linking.
    struct GlslangResult {
        std::vector<ShaderResult> shaderResults;
        std::string linkingOutput;
        std::string linkingError;
        std::string spirvWarningsErrors;
        std::string spirv;  // Optional SPIR-V disassembly text.
        bool success;
    };
}

#define EXPECT_THROW(statement, expected_exception) \
  GTEST_TEST_THROW_(statement, expected_exception, GTEST_NONFATAL_FAILURE_)
#define EXPECT_NO_THROW(statement) \
  GTEST_TEST_NO_THROW_(statement, GTEST_NONFATAL_FAILURE_)
#define EXPECT_ANY_THROW(statement) \
  GTEST_TEST_ANY_THROW_(statement, GTEST_NONFATAL_FAILURE_)
#define ASSERT_THROW(statement, expected_exception) \
  GTEST_TEST_THROW_(statement, expected_exception, GTEST_FATAL_FAILURE_)
#define ASSERT_NO_THROW(statement) \
  GTEST_TEST_NO_THROW_(statement, GTEST_FATAL_FAILURE_)
#define ASSERT_ANY_THROW(statement) \
  GTEST_TEST_ANY_THROW_(statement, GTEST_FATAL_FAILURE_)

// Boolean assertions. Condition can be either a Boolean expression or an
// AssertionResult. For more information on how to use AssertionResult with
// these macros see comments on that class.
#define EXPECT_TRUE(condition) \
  GTEST_TEST_BOOLEAN_((condition), #condition, false, true, \
                      GTEST_NONFATAL_FAILURE_)
#define EXPECT_FALSE(condition) \
  GTEST_TEST_BOOLEAN_(!(condition), #condition, true, false, \
                      GTEST_NONFATAL_FAILURE_)
#define ASSERT_TRUE(condition) \
  GTEST_TEST_BOOLEAN_((condition), #condition, false, true, \
                      GTEST_FATAL_FAILURE_)
#define ASSERT_FALSE(condition) \
  GTEST_TEST_BOOLEAN_(!(condition), #condition, true, false, \
                      GTEST_FATAL_FAILURE_)

#endif  // XKSLANG_TESTS_DEFINE_H
