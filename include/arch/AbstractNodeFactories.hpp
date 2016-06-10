#ifndef ARCH_ABSTRACTNODEFACTORIES_HPP
#define ARCH_ABSTRACTNODEFACTORIES_HPP

#include <memory>
#include <string>
#include <vector>

/**
 * @brief The AbstractSyntaxTreeNode struct
 * Placeholder struct, will be replaced by an #include, when
 * AbstractSyntaxTreeNode-Header is ready
 */
struct AbstractSyntaxTreeNode {};

/**
 * @brief The AbstractInstructionNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type instruction
 */
class AbstractInstructionNodeFactory {
 public:
  AbstractInstructionNodeFactory() {}

  virtual ~AbstractInstructionNodeFactory() = 0;

  /**
   * @brief createInstructionNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type instruction for
   * the given token. If the architecture-specific implementation cannot find a
   * instruction with the given token, a nullptr is returned
   *
   * @param token std::string, that contains the identifer of a instruction
   * (e.g. "ADD" for Add-Instruction) in upper-case
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given token cannot be mapped to a architecture-specific
   * instruction
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createInstructionNode(
      std::string& token) = 0;
};

/**
 * @brief The AbstractImmediateNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type immediate
 */
class AbstractImmediateNodeFactory {
 public:
  AbstractImmediateNodeFactory() {}

  virtual ~AbstractImmediateNodeFactory();

  /**
   * @brief createImmediateNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type immediate with the given numericalValue. If the
   * architecture-specific implementation cannot create an immediate from the
   * given numericalValue, a nullptr is returned
   *
   * @param numericalValue representation of a number to create the immediate
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the architecture-specific implementation cannot create an
   * immediate from the given numericalValue
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createImmediateNode(
      std::vector<bool> numericalValue) = 0;
};

/**
 * @brief The AbstractRegisterAccessNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type register access
 */
class AbstractRegisterAccessNodeFactory {
 public:
  AbstractRegisterAccessNodeFactory() {}

  virtual ~AbstractRegisterAccessNodeFactory() = 0;

  /**
   * @brief createRegisterAccessNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type register access, linked to the given
   * registerAddress. If the given registerAddress is invalid a nullptr is
   * returned.
   *
   * @param registerAdress Identifier linking to a valid register
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given registerAddress is invalid
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createRegisterAccessNode(
      const int registerAddress) = 0;
};

/**
 * @brief The AbstractMemoryAccessNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type memory access
 */
class AbstractMemoryAccessNodeFactory {
 public:
  AbstractMemoryAccessNodeFactory() {}

  virtual ~AbstractMemoryAccessNodeFactory() = 0;

  /**
   * @brief createMemoryAccessNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type memory access.
   *
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createMemoryAccessNode() = 0;
};

/**
 * @brief The AbstractArithmeticOpNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type arithmetic
 * operation
 */
class AbstractArithmeticOpNodeFactory {
 public:
  static const int ADDITION = 1;
  static const int SUBTRACTION = 2;
  static const int MULTIPLICATION = 3;
  static const int DIVISION = 4;

  AbstractArithmeticOpNodeFactory() {}

  virtual ~AbstractArithmeticOpNodeFactory() = 0;

  /**
   * @brief createArithmeticOperationNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type arithmetic operation. Each operation type
   * can be described by constants defined in this class. If the given opType is
   * invalid, a nullptr is returned.
   *
   * @param opType constant, defined in this class, describing the type of the
   * arithmetical operation
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given opType is invalid
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createArithmeticOperationNode(
      const int opType) = 0;
};

#endif  // ARCH_ABSTRACTNODEFACTORIES_HPP
