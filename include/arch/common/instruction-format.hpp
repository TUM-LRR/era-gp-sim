/*
 * Format is an abstract class, from which all the specific formats inherit.
 */
struct Format {
  /*
   * @param key Has the information about the bit representation of the opcode
   * function bits.
   * @param node This gives us access to the arguments in the instruction.
   * @return Bit representation of the instruction.
   */
  virtual std::vector<bool>
  operator()(const InstructionInformation::InstructionKey& key,
             const InstructionNode& node) = 0;
};
