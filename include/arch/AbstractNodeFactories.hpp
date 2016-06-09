#ifndef ARCH_ABSTRACTNODEFACTORY_HPP
#define ARCH_ABSTRACTNODEFACTORY_HPP

#include <memory>
#include <string>
#include <vector>

typedef std::string AbstractSyntaxTreeNode;

class AbstractInstructionNodeFactory {
 public:
  AbstractInstructionNodeFactory() {}

  virtual ~AbstractInstructionNodeFactory() = 0;

  virtual std::shared_ptr<AbstractSyntaxTreeNode> createInstructionNode(
      std::string& token) = 0;
};

class AbstractImmediateNodeFactory {
 public:
  AbstractImmediateNodeFactory() {}

  virtual ~AbstractImmediateNodeFactory();

  virtual std::shared_ptr<AbstractSyntaxTreeNode> createImmediateNode(
      std::vector<bool> numericalValue) = 0;
};

class AbstractRegisterAccessNodeFactory {
 public:
  AbstractRegisterAccessNodeFactory() {}

  virtual ~AbstractRegisterAccessNodeFactory() = 0;

  virtual std::shared_ptr<AbstractSyntaxTreeNode> createRegisterAccessNode(
      const int registerAdress) = 0;
};

class AbstractMemoryAccessNodeFactory {
 public:
  AbstractMemoryAccessNodeFactory() {}

  virtual ~AbstractMemoryAccessNodeFactory() = 0;

  virtual std::shared_ptr<AbstractSyntaxTreeNode> createMemoryAccessNode() = 0;
};

class AbstractArithmeticOpNodeFactory {
 public:
  static const int ADDITION = 1;
  static const int SUBTRACTION = 2;
  static const int MULTIPLICATION = 3;
  static const int DIVISION = 4;

  AbstractArithmeticOpNodeFactory() {}

  virtual ~AbstractArithmeticOpNodeFactory() = 0;

  virtual std::shared_ptr<AbstractSyntaxTreeNode> createArithmeticOperationNode(
      const int opType) = 0;
};

#endif  // ARCH_ABSTRACTNODEFACTORY_HPP
