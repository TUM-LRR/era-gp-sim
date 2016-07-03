#include "arch/common/instruction-assembler.hpp"

InstructionAssembler::InstructnionAssembler(const Architecture& arch,
                                            const string& format) {
  // getAssembler is supposed to use a hashmap to fetch the appropriate assemble
  // function
  assemble = arch.getAssembler(format);
}
