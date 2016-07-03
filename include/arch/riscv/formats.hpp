#include "arch/common/instruction-format.hpp"

/*
 * funct7 | rs2 | rs1 | funct3 | rd | opcode
 */
struct RFormat : public Format {};

/*
 * imm[11:0] | rs1 | funct3 | rd | opcode
 */
struct IFormat : public Format {};

/*
 * imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode
 */
struct SFormat : public Format {};

/*
 * imm[31:12] | rd | opcode
 */
struct UFormat : public Format {};
