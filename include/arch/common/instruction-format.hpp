/*
 * Format is an abstract class, from which all the specific formats inherit.
 * It should be a friend class of the AbstractSyntaxNode class in order to have
 * access to the child nodes of the instruction node.
 */
struct Format {
  /*
   * @param key Has the information about the bit representation of the opcode
   * function bits.
   * @param args Vector of bit representations of the child nodes..
   * @return Bit representation of the instruction.
   */
  virtual std::vector<bool>
  operator()(const InstructionInformation::InstructionKey& key,
             const std::vector<MemoryValue> args) = 0;
};


#endif /* ERAGPSIM_ARCH_COMMON_INSTRUCTION_FORMAT_HPP */
