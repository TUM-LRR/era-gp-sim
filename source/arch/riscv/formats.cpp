#include "arch/riscv/formats.hpp"


std::vector<bool> RFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  std::vector res(32);

  for (auto bit : key["funct7"]) res.push_back(bit);

  // This is where we need access to a protected member.
  for (auto bit : node._children.at(2)->getValue()) res.push_back(bit);
  // r2
  for (auto bit : node._children.at(1)->getValue()) res.push_back(bit);
  // r1
  for (auto bit : key["funct3"]) res.push_back(bit);

  // destination
  for (auto bit : node._children.at(0)->getValue()) res.push_back(bit);

  for (auto bit : key["opcode"]) res.push_back(bit);

  return res;
}


std::vector<bool> IFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  // TODO
}

std::vector<bool> SFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  // TODO
}

std::vector<bool> UFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  // TODO
}
