#include "arch/riscv/integer_instructions.hpp"

#include <gtest/gtest.h>

TEST(InstructionTest, Add) {
    AddInstructionNode addiNode{true};
    ASSERT_EQ(NodeType::INSTRUCTION, addiNode.getType());
    ASSERT_EQ("addi", addiNode.getIdentifier());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
