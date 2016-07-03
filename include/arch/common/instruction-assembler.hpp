// There should be a HashMap of instructions to functions for the assembly
// in the abstract factory.

#include <functional>
#include <string>
#include <vector>

#include "arch/common/architecture.hpp"
#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-node.hpp"

class InstructionAssembler {
  using InstructionKey = InstructionInformation::InstructionKey;

 public:
  std::function<std::vector<bool>(InstructionKey, InstructionNode)> assemble;


  // a reference to the arch object, to get the appropriate assemble method from
  // the hashmap
  /*
   * @param arch Reference to the architecture object
   * @param format The format of the current instruction
   */
  InstructionAssembler(const Architecture& arch, const string& format);
};
