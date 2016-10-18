#include "arithmetic-test-utils.hpp"
#include "common/assert.hpp"

void testIntegerInstructionValidation(MemoryAccess& memAccess,
                                      const NodeFactoryCollection& instrF,
                                      ImmediateNodeFactory& immF,
                                      std::string instructionToken,
                                      bool isImmediateInstr) {
    using AssertionError = assert::AssertionError;
  std::string registerId = "not relevant";
  // add 0-3 random Nodes
  auto instructionNode = instrF.createInstructionNode(instructionToken);
  //ASSERT_THROW(instructionNode->getValue(memAccess), AssertionError);
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  //ASSERT_THROW(instructionNode->getValue(memAccess), AssertionError);
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  //ASSERT_THROW(instructionNode->getValue(memAccess), AssertionError);
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  //ASSERT_THROW(instructionNode->getValue(memAccess), AssertionError);

  // test opposite operand configuration
  auto instructionNode2 = instrF.createInstructionNode(instructionToken);
  instructionNode2->addChild(
      std::move(std::make_unique<RegisterNode>(registerId)));
  instructionNode2->addChild(
      std::move(std::make_unique<RegisterNode>(registerId)));
  if (isImmediateInstr) {
    // Immediate Instraction has now 3 Register Operands
    instructionNode2->addChild(
        std::move(std::make_unique<RegisterNode>(registerId)));
  } else {
    // Register Instraction has now 2 Register & 1 Immediate Operand
    instructionNode2->addChild(
        std::move(immF.createImmediateNode(riscv::convert<uint64_t>(0))));
  }
  ASSERT_FALSE(instructionNode2->validate(memAccess).isSuccess());
  //ASSERT_THROW(instructionNode2->getValue(memAccess), AssertionError);

  // test valid children, but with one more operand
  auto instructionNode3 = instrF.createInstructionNode(instructionToken);
  instructionNode3->addChild(
      std::move(std::make_unique<RegisterNode>(registerId)));
  instructionNode3->addChild(
      std::move(std::make_unique<RegisterNode>(registerId)));
  if (isImmediateInstr) {
    instructionNode3->addChild(
        std::move(immF.createImmediateNode(riscv::convert<uint64_t>(0))));
  } else {
    instructionNode3->addChild(
        std::move(std::make_unique<RegisterNode>(registerId)));
  }
  ASSERT_TRUE(instructionNode3->validate(memAccess).isSuccess());
  // add one more random node
  instructionNode3->addChild(
      std::move(immF.createImmediateNode(riscv::convert<uint64_t>(0))));
  ASSERT_FALSE(instructionNode3->validate(memAccess).isSuccess());
  //ASSERT_THROW(instructionNode3->getValue(memAccess), AssertionError);
}

void test12BitImmediateBounds(const NodeFactoryCollection &instrF,
                              std::string instructionToken,
                              ImmediateNodeFactory& immF, MemoryAccess &access) {
  constexpr uint64_t boundary = 0x7FF;
  constexpr int64_t negative_boundary =  -2048;//smallest negative integer in 12bit
  std::string registerId = "not relevant";
  auto nodeTrue = instrF.createInstructionNode(instructionToken);
  nodeTrue->addChild(std::move(std::make_unique<RegisterNode>(registerId)));
  nodeTrue->addChild(std::move(std::make_unique<RegisterNode>(registerId)));
  auto immediateNodeIn =
      immF.createImmediateNode(riscv::convert<uint64_t>(boundary));
  nodeTrue->addChild(std::move(immediateNodeIn));
  ASSERT_TRUE(nodeTrue->validate(access).isSuccess());

  auto nodeTrueNegative = instrF.createInstructionNode(instructionToken);
  nodeTrueNegative->addChild(std::move(std::make_unique<RegisterNode>(registerId)));
  nodeTrueNegative->addChild(std::move(std::make_unique<RegisterNode>(registerId)));
  auto immediateNodeNegative =
      immF.createImmediateNode(riscv::convert<int64_t>(negative_boundary));
  nodeTrueNegative->addChild(std::move(immediateNodeNegative));
  ASSERT_TRUE(nodeTrueNegative->validate(access).isSuccess());

  auto nodeFalse = instrF.createInstructionNode(instructionToken);
  nodeFalse->addChild(std::move(std::make_unique<RegisterNode>(registerId)));
  nodeFalse->addChild(std::move(std::make_unique<RegisterNode>(registerId)));
  auto immediateNodeOut =
      immF.createImmediateNode(riscv::convert<int64_t>(boundary + 1));
  nodeFalse->addChild(std::move(immediateNodeOut));
  ASSERT_FALSE(nodeFalse->validate(access).isSuccess());
}
