#include "arch/riscv/formats.hpp"


std::vector<bool> RFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  std::vector res(32);

  for (auto bit : key["funct7"]) res.push_back(bit);

  // I need to somehow access the children nodes
  // res.push_back(node.getChild(2));
  // child 1

  for (auto bit : key["funct3"]) res.push_back(bit);

  // destination

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
