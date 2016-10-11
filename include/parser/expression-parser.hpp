/*
* C++ Assembler Interpreter
* Copyright (C) 2016 Chair of Computer Architecture
* at Technical University of Munich
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ERAGPSIM_PARSER_EXPRESSION_PARSER_HPP
#define ERAGPSIM_PARSER_EXPRESSION_PARSER_HPP

#include <algorithm>
#include <regex>
#include <stack>
#include <unordered_map>
#include "common/assert.hpp"
#include "common/multiregex.hpp"
#include "parser/expression-compiler-definitions.hpp"

/**
 * \brief Parses a given token stream and evaluates it.
 * \tparam T The number output type.
 *
 * So, this class applies the Shunting Yard algorithm (oriented at
 * http://wcipeg.com/wiki/Shunting_yard_algorithm). It knows its binary and
 * unary operators and how to decode literals. For this version, the unary
 * operators have one specific precedence which is always higher than the binary
 * one. For the Shunting Yard algorithm, we do only have to go through the
 * string once. Whilst doing that, we maintain two stacks: one stack containing
 * operators and brackets, and the other one containing temporary numbers which
 * cannot be evaluated yet. In the end, the operator stack should be empty and
 * on the output stack (the one which contains the temporary numbers) is only
 * one element. This is the value we want to return.
 *
 * During the parsing procedure, we decide what to do depending on the token
 * type we are currently looking at:
 * _Note:_ When we discard an operator, we apply it to the stack. For a binary
 * operator, the stack size then decreases by one. A unary operator changes
 * nothing at the stack size.
 *
 * * a _literal_ is parsed instantly and pushed upon the output stack of
 * temporary numbers.
 * * an _opening bracket_ is simply pushed onto the operator stack.
 * * for an _operator_, we first of all check, if there is a closing bracket or
 * a number preceeding it. If so, the operator is definitely binary and if not,
 * we probably have a unary operator. So we have determined the arity of our
 * operator. The next step is to evaluate all operators which are now finished,
 * i.e. there cannot come any more operands/operators for them which have an
 * equal precedence. Example: We got the string "1 * 2 + 3", then after parsing
 * '+' we know that '*' is finished, as there can be no more operand/operator
 * after it, which might influence the "1 * 2". As we have pushed our operators
 * on the stack if and only if they have a lower precedence than all previous
 * operators, we can simply pop from the stack until we get to the point that we
 * have at most and equals precedence, but no smaller one on it any more. And we
 * may even discard if we have already an operator of equal precedence lying on
 * it, because if our operator is left-associative, we want to insta-evaluate
 * the expression. If we got a right-associative operator, we want to wait until
 * there is any operator of higher precedence or the end of the string. After we
 * have discarded all operators with lower precedence, we push our operator onto
 * the stack.
 * * a _closing bracket_ behaves similar to an operator which has a higher
 * precedence than possible, but does stop at a bracket on the stack (i.e. we do
 * view the bracket as "left-associative") which we then discard.
 *
 * After we have handled the complete string, there might be still some
 * operators on the stack which we then all discard (by applying a more powerful
 * operator than the one used for brackets, one which is not present on the
 * stack at all times). Then we should be done.
 */
template <typename T>
class ExpressionParser {
 public:
  /**
   * \brief Creates a new expression parser based on a parser definition.
   * \param definition The supplied parser definition.
   */
  ExpressionParser(const ExpressionParserDefinition<T>& definition)
      // Setting up literal decoders.
      : _literalDecodeRegex(setupLiteralDecoders(definition)) {
    // Binary and unary operators are inserted in their corresponding maps.
    for (const auto& i : definition.binaryOperators) {
      _binaryOperators[i.identifier] = i;
    }
    for (const auto& i : definition.unaryOperators) {
      _unaryOperators[i.identifier] = i;
    }
  }

  /**
   * \brief Parses the given token stream into a single number.
   * \param tokens The input token stream.
   * \param externalState The compile state to note down any errors.
   * \return The result of the expression if the compilation has been
   * successful. If now, `T()` will be returned.
   */
  T parse(const std::vector<ExpressionToken>& tokens,
          CompileState& externalState) const {
    // The parse state is only used in this class (it contains references as
    // members).
    ParseState state(tokens, externalState);

    for (const auto& i : tokens) {
      // We iterate through each token and try to handle it.
      state.curr = i;

      if (!handleToken(state)) {
        // We failed handling a token.
        return T();
      }

      state.last = state.curr;
    }

    // We handle the rest of the operators.
    if (!decreaseStack(state, IToken{ITokenType::EXPRESSION, ""})) {
      return T();
    }

    // If there is more than one operand left, there must be an error.
    if (state.outputStack.size() != 1) {
      recordError(state, "Malformed expression!");
      return T();
    }

    // Everything ok, let's return out result.
    return state.outputStack.top();
  }

 private:
  // This enum is for the operator stack. It is a bit different from the
  // 'external' token enum.
  enum class ITokenType {
    BINARY_OPERATOR,
    UNARY_OPERATOR,
    BRACKET,
    EXPRESSION
  };

  // An item on the operator stack, it behaves like a token. (that is the reason
  // for the name)
  struct IToken {
    ITokenType type;
    std::string data;
  };

  // A function definition for internal decoding (the regex is found in a
  // different variable).
  using ILiteralDecoder = std::function<bool(std::string, T&, CompileState&)>;

  // The state to carry all internal data (to prevent long parameter lists).
  struct ParseState {
    // Current token.
    ExpressionToken last = ExpressionToken{"", ExpressionTokenType::INVALID};

    // Last token.
    ExpressionToken curr = ExpressionToken{"", ExpressionTokenType::INVALID};

    // Reference on the token vector.
    const std::vector<ExpressionToken>& tokens;

    // Reference to the compile state.
    CompileState& state;

    // Output stack for temporary numbers.
    std::stack<T> outputStack;

    // Stack for operators.
    std::stack<IToken> operatorStack;

    // Constructor, for reference assignment mainly.
    ParseState(const std::vector<ExpressionToken>& xtokens,
               CompileState& xstate)
    : tokens(xtokens), state(xstate) {
    }
  };

  // Records an error in the parsing process.
  void recordError(ParseState& state, const std::string& message) const {
    state.state.addError(message, state.state.position >> state.curr.index);
  }

  // Handles the latest token stored in `state.curr`.
  bool handleToken(ParseState& state) const {
    // We differ for each token type.
    switch (state.curr.type) {
      case ExpressionTokenType::OPERATOR: {
        // Arity is determined, then the operator is pushed.
        auto no = IToken{isUnary(state) ? ITokenType::UNARY_OPERATOR
                                        : ITokenType::BINARY_OPERATOR,
                         state.curr.data};
        return pushOperator(state, no);
      }
      case ExpressionTokenType::LEFT_BRACKET:
        // Just push it onto the stack.
        state.operatorStack.push(IToken{ITokenType::BRACKET, ""});
        return true;
      case ExpressionTokenType::LITERAL:
        // Just parse the literal and push it onto the stack.
        return parseLiteral(state);
      case ExpressionTokenType::RIGHT_BRACKET:
        // We remove everything until a bracket occurs.
        return handleBorder(state, IToken{ITokenType::BRACKET, ""});
      case ExpressionTokenType::INVALID:
        // For you, dear clang, so you don't complain. We fall through.
        break;
    }

    // Should never happen, only if there are implementation errors.
    assert::that(false);
    return false;
  }

  // Pushes an operator onto the stack, but pops all finished operators first.
  bool pushOperator(ParseState& state, const IToken& token) const {
    if (!valid(token)) {
      // For the right error message, we got to determine the arity of our
      // operator.
      if (token.type == ITokenType::UNARY_OPERATOR) {
        recordError(state,
                    "'" + token.data + "' is not a valid unary operator!");
      } else if (token.type == ITokenType::BINARY_OPERATOR) {
        recordError(state,
                    "'" + token.data + "' is not a valid binary operator!");
      } else {
        // Undefined arity.
        assert::that(false);
      }
      return false;
    }

    // Then we decrease the stack...
    if (!decreaseStack(state, token)) {
      return false;
    }

    //...and push our operator on it.
    state.operatorStack.push(token);
    return true;
  }

  // Handles brackets.
  bool handleBorder(ParseState& state, const IToken& token) const {
    // We decrease the stack until we get to the first bracket.
    if (!decreaseStack(state, token)) {
      return false;
    }

    // If there has been nothing on it, there are some brackets closed that have
    // not been opened.
    if (state.operatorStack.empty()) {
      recordError(state, "There are some opening brackets missing!");
      return false;
    }

    // Then we remove the bracket.
    state.operatorStack.pop();
    return true;
  }

  // Returns if we got a unary operator.
  bool isUnary(ParseState& state) const {
    // We define unary to be if we cannot be binary (no literal, no brackets
    // closed).
    return !(state.last.type == ExpressionTokenType::LITERAL ||
             state.last.type == ExpressionTokenType::RIGHT_BRACKET);
  }

  // Parses a literal.
  bool parseLiteral(ParseState& state) const {
    // This is the second use case for the Multiregex class.
    MSMatch match;
    if (_literalDecodeRegex.search(state.curr.data, match)) {
      // We need to 'tokenize' the literal again, as our previous tokenizer does
      // only give us the plain strings. So here we got to determine which
      // literal decoder we should use.
      T parsed;
      if (!_literalDecoders.at(match.choice)(
              state.curr.data, parsed, state.state)) {
        // This may also fail...
        return false;
      }

      state.outputStack.push(parsed);
      return true;
    }

    // Not found, might also be an implementation error?
    // recordError(state, "Invalid literal.");
    assert::that(false);
    return false;
  }

  // Tries to get as many operands as needed.
  bool
  getValues(ParseState& state, size_t count, std::vector<T>& output) const {
    for (size_t i = 0; i < count; ++i) {
      if (state.outputStack.empty()) {
        // There were not enough operands on the stack.
        size_t left = count - i;
        recordError(state,
                    left == 1 ? "There is an operand missing."
                              : "There are " + std::to_string(left) +
                                    " operands missing.");
        return false;
      }
      output.push_back(state.outputStack.top());
      state.outputStack.pop();
    }

    // We need to reverse the operands, as we push them in our vector the wrong
    // way.
    std::reverse(output.begin(), output.end());
    return true;
  }

  // Handles a unary operator.
  bool handleUnaryOperator(ParseState& state, const IToken& token) const {
    // We get one value.
    std::vector<T> args;
    if (!getValues(state, 1, args)) {
      return false;
    }

    // We try to apply it to the stack.
    T outputValue;
    if (!_unaryOperators.at(token.data)
             .handler(args[0], outputValue, state.state)) {
      return false;
    }

    // The output value is then put on the stack again.
    state.outputStack.push(outputValue);
    return true;
  }


  // Handles a binary operator.
  bool handleBinaryOperator(ParseState& state, const IToken& token) const {
    // We get two values.
    std::vector<T> args;
    if (!getValues(state, 2, args)) {
      return false;
    }

    // We try to apply it to the stack.
    T outputValue;
    if (!_binaryOperators.at(token.data)
             .handler(args[0], args[1], outputValue, state.state)) {
      return false;
    }

    // The output value is then put on the stack again.
    state.outputStack.push(outputValue);
    return true;
  }

  // Applies the topmost token of the operator stack and discards it.
  bool applyTopmostToken(ParseState& state) const {
    // To have an empty operator stack here is completely unexpected (as this
    // error is treated somewhere else).
    assert::that(!state.operatorStack.empty());

    IToken token = state.operatorStack.top();

    // Again, we handle each token type differently.
    switch (token.type) {
      case ITokenType::BRACKET:
        // We should never apply a bracket token (then, some closing brackets
        // are missing).
        recordError(state, "There are some closing brackets missing!");
        return false;
      case ITokenType::UNARY_OPERATOR:
        // Handles the unary operator on the stack.
        return handleUnaryOperator(state, token);
      case ITokenType::BINARY_OPERATOR:
        // Handles the binary operator on the stack.
        return handleBinaryOperator(state, token);
      case ITokenType::EXPRESSION:
        // For you, dear clang, so you don't complain. We fall through.
        break;
    }

    // Cannot handle that token.
    assert::that(false);
    return false;
  }

  bool decreaseStack(ParseState& state, const IToken& token) const {
    // To decrease the stack, we need the precedence of our operator and if we
    // may discard operators of equal precedence.
    int mPrec   = precedence(token);
    bool mEqual = popSamePrecedence(token);

    while (!state.operatorStack.empty() &&
           (mPrec > precedence(state.operatorStack.top()) ||
            (mEqual && mPrec == precedence(state.operatorStack.top())))) {
      // We loop until we cannot do this any more (or there are no operators
      // left to process).
      if (!applyTopmostToken(state)) {
        return false;
      }
      state.operatorStack.pop();
    }

    return true;
  }

  // Checks if an operator is valid.
  bool valid(const IToken& token) const {
    // Anything other than an operator is always valid. For operators, we look
    // if there is an operator defined like this and with this specific arity.
    if (token.type == ITokenType::UNARY_OPERATOR) {
      return _unaryOperators.find(token.data) != _unaryOperators.end();
    } else if (token.type == ITokenType::BINARY_OPERATOR) {
      return _binaryOperators.find(token.data) != _binaryOperators.end();
    } else {
      return true;
    }
  }

  // Returns, if we should also remove tokens of same precedence from the
  // operator stack.
  bool popSamePrecedence(const IToken& token) const {
    // Only applies to binary operators as there is no associativity for
    // anything else.
    switch (token.type) {
      case ITokenType::UNARY_OPERATOR:
      case ITokenType::BRACKET:
      case ITokenType::EXPRESSION: return false;
      case ITokenType::BINARY_OPERATOR:
        return _binaryOperators.at(token.data).associativity ==
               ExpressionOperatorAssociativity::LEFT;
    }

    // Should never happen, only if the implementation is broken.
    assert::that(false);
    return false;
  }

  // Retrieves the precedence of the token.
  int precedence(const IToken& token) const {
    switch (token.type) {
      case ITokenType::UNARY_OPERATOR:
        // Unary operators always have the lowest precedence, i.e. they do not
        // alter the stack.
        return -1;
      case ITokenType::BRACKET:
        // Brackets have a higher precedence than any other operator, i.e. they
        // remove any other operators from the stack.
        return 0x10000;
      case ITokenType::EXPRESSION:
        // An expression has the highest precedence and is never found on the
        // stack. It makes everything disappear.
        return 0x1ffff;
      case ITokenType::BINARY_OPERATOR:
        // If we got a binary operator, we may simply take its precedence.
        return _binaryOperators.at(token.data).precedence;
    }

    // Should never happen, only if the implementation is broken.
    assert::that(false);
    return false;
  }

  // Sets the two attributes connected to literal decoding.
  MSRegex
  setupLiteralDecoders(const ExpressionParserDefinition<T>& definition) {
    _literalDecoders.reserve(definition.literalDecoders.size());
    std::vector<std::string> literalRegexDecode;
    literalRegexDecode.reserve(definition.literalDecoders.size());
    for (const auto& i : definition.literalDecoders) {
      // We separate decoder functions from their regexes.
      _literalDecoders.push_back(i.decoder);
      literalRegexDecode.push_back(i.regex);
    }

    // They we create our multiregex.
    return MSRegex("^", "$", literalRegexDecode, std::regex::optimize);
  }

  // Operator maps for fast access.
  std::unordered_map<std::string, ExpressionBinaryOperator<T>> _binaryOperators;
  std::unordered_map<std::string, ExpressionUnaryOperator<T>> _unaryOperators;

  // Literal decoding attributes.
  std::vector<ILiteralDecoder> _literalDecoders;
  MSRegex _literalDecodeRegex;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_PARSER_HPP */
