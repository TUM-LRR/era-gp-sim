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

#include "parser/expression-compiler-clike.hpp"

const ExpressionCompiler<ExpressionBigIntType> EXPRESSION_COMPILER_CLIKE = ExpressionCompiler<ExpressionBigIntType>(ExpressionCompilerDefinition<ExpressionBigIntType>
{
	ExpressionParserDefinition<ExpressionBigIntType>
	{
		std::vector<ExpressionBinaryOperator<ExpressionBigIntType>>
		{
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
				"||",
				120,
				[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f || s ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
					return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
				"&&",
				110,
				[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f && s ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
				"|",
				100,
				[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f | s;
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
				"^",
				90,
				[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f ^ s;
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
				"&",
				80,
				[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f & s;
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
				"==",
				70,
				[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f == s ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
				"!=",
				70,
				[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f != s ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
 return true;
				}
			},
				ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
					"<",
					60,
					[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f < s ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
					"<=",
					60,
					[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f <= s ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
					">",
					60,
					[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f > s ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
					">=",
					60,
					[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f >= s ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
 return true;
				}
			},
				ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
					"<<",
					50,
					[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f << s;
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
					">>",
					50,
					[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f >> s;
 return true;
				}
			},
				ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
					"+",
					40,
					[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f + s;
 return true;
				}
			},
			ExpressionBinaryOperator<ExpressionBigIntType>
			{
				ExpressionOperatorAssociativity::LEFT,
					"-",
					40,
					[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = f - s;
 return true;
				}
			},
					ExpressionBinaryOperator<ExpressionBigIntType>
				{
					ExpressionOperatorAssociativity::LEFT,
						"*",
						30,
						[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
					{
						out = f * s;
 return true;
					}
				},
				ExpressionBinaryOperator<ExpressionBigIntType>
				{
					ExpressionOperatorAssociativity::LEFT,
						"/",
						30,
						[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
					{
						out = f / s;
 return true;
					}
				},
				ExpressionBinaryOperator<ExpressionBigIntType>
					{
						ExpressionOperatorAssociativity::LEFT,
							"%",
							30,
							[](const ExpressionBigIntType& f, const ExpressionBigIntType& s, ExpressionBigIntType& out, CompileState& state) -> bool
						{
							out = f % s;
 return true;
						}
					}
		},
		std::vector<ExpressionUnaryOperator<ExpressionBigIntType>>
		{
			ExpressionUnaryOperator<ExpressionBigIntType>
			{
				"+",
				[](const ExpressionBigIntType& v, ExpressionBigIntType& out, CompileState& state) -> bool
				{
					out = +v;
 return true;
				}
			},
				ExpressionUnaryOperator<ExpressionBigIntType>
				{
					"-",
						[](const ExpressionBigIntType& v, ExpressionBigIntType& out, CompileState& state) -> bool
					{
						out = -v;
 return true;
					}
				},
					ExpressionUnaryOperator<ExpressionBigIntType>
					{
						"!",
							[](const ExpressionBigIntType& v, ExpressionBigIntType& out, CompileState& state) -> bool
						{
							out = !v ? ExpressionBigIntType(1) : ExpressionBigIntType(0);
 return true;
						}
					},
						ExpressionUnaryOperator<ExpressionBigIntType>
						{
							"~",
								[](const ExpressionBigIntType& v, ExpressionBigIntType& out, CompileState& state) -> bool
							{
								out = ~v;
 return true;
							}
						}
		},
		std::vector<ExpressionLiteralDecoder<ExpressionBigIntType>>
		{
			ExpressionLiteralDecoder<ExpressionBigIntType>
			{
				"0x[0-9a-fA-F]+",
					[](const std::string& number, ExpressionBigIntType& output, CompileState& state) -> bool
				{
					output = std::stol(number.substr(2), nullptr, 16);
					return true;
				}
			},
				ExpressionLiteralDecoder<ExpressionBigIntType>
				{
					"0b[01]+",
						[](const std::string& number, ExpressionBigIntType& output, CompileState& state) -> bool
					{
						output = std::stol(number.substr(2), nullptr, 2);
						return true;
					}
				},
					ExpressionLiteralDecoder<ExpressionBigIntType>
					{
						"[0-9]+",
							[](const std::string& number, ExpressionBigIntType& output, CompileState& state) -> bool
						{
							output = std::stol(number, nullptr, 10);
							return true;
						}
					}
		}
	},
	ExpressionHelpRegexes
	{
		"\\(",
		"\\)"
	}
});
