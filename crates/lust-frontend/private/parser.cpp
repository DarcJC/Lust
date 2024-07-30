#include "parser.hpp"

#include <string_view>
#include <iostream>
#include <sstream>
#include <utility>

#include "container/unique_ptr.hpp"
#include "grammar.hpp"
#include "grammar/type_expr.hpp"
#include "grammar/operator_expr.hpp"
#include "lexer.hpp"

namespace lust
{
namespace grammar
{
    /**
     * A LL(1) parser
     */
    class Parser : public IParser {
    public:
        Parser(lexer::TokenStream& token_stream);

        lust::UniquePtr<ASTNode_Program> parse() override;

        bool is_error_occurred() const override;

    private:
        /**
         * @brief Notify a parser error and set parser error flag
         */
        void error_msg(std::string_view msg);

        /**
         * @brief Notify a parser error, set parser error flag and consume current token
         */
        void error(std::string_view msg);

        lexer::Token next_token();

        /**
         * @brief The token consumer
         */
        bool expected(lexer::TerminalTokenType expected_type, std::string_view failure_msg = {});

        /**
         * @brief The token consumer but no cause an error when failure
         */
        bool optional(lexer::TerminalTokenType expected_type);
    private:
        lexer::TokenStream& m_token_stream;
        lexer::Token m_current_token{};

        bool m_error_occurred = false;

    private:
        UniquePtr<ASTNode_Program> parse_program();
    
        UniquePtr<ASTNode_Statement> parse_statement();

        UniquePtr<ASTNode_Statement> parse_statement_pub_prefix();

        UniquePtr<ASTNode_VarDecl> parse_variable_declaration();

        UniquePtr<ASTNode_FunctionDecl> parse_function_declaration();

        vector<UniquePtr<ASTNode_Attribute>> parse_attribute_declaration();

        QualifiedName parse_qualifier_name();

        Visibility parse_visibility();

        UniquePtr<ASTNode_GenericParam> parse_generic_param();

        vector<UniquePtr<ASTNode_GenericParam>> try_parse_generic_params();

        UniquePtr<ASTNode_TypeExpr> parse_trival_type();

        UniquePtr<ASTNode_TypeExpr_Tuple> parse_tuple_type();

        UniquePtr<ASTNode_TypeExpr_Array> parse_array_type();

        UniquePtr<ASTNode_TypeExpr_Reference> parse_reference_type();

        UniquePtr<ASTNode_TypeExpr_Function> parse_function_type();

        UniquePtr<ASTNode_TypeExpr> parse_type_expr();

        UniquePtr<ASTNode_TypeExpr> create_unit_type();

        UniquePtr<ASTNode_ParamDecl> parse_invokable_wanted_param();

        UniquePtr<ASTNode_InvokeParameters> parse_invoke_param_list();

        UniquePtr<ASTNode_Block> parse_code_block();

        UniquePtr<ASTNode_Statement> parse_statement_with_attributes();

        UniquePtr<ASTNode_StructDecl> parse_struct_declaration();

        UniquePtr<ASTNode_StructField> parse_struct_field_declaration();

        UniquePtr<ASTNode_TraitDecl> parse_trait_declaration();

        UniquePtr<ASTNode_MorphismsType> parse_morphisms_type();

        UniquePtr<ASTNode_MorphismsConstant> parse_morphisms_constant();

        // === Basic Expressions ===
        UniquePtr<ASTNode_Operator> parse_expression();
        UniquePtr<ASTNode_Operator> parse_expr_assignment();
        UniquePtr<ASTNode_Operator> parse_expr_logical_or();
        UniquePtr<ASTNode_Operator> parse_expr_logical_and();
        UniquePtr<ASTNode_Operator> parse_expr_logical_equality();
        UniquePtr<ASTNode_Operator> parse_expr_logical_none_equality();
        UniquePtr<ASTNode_Operator> parse_expr_logical_less();
        UniquePtr<ASTNode_Operator> parse_expr_logical_less_equalty();
        UniquePtr<ASTNode_Operator> parse_expr_logical_greater();
        UniquePtr<ASTNode_Operator> parse_expr_logical_greater_equalty();
        UniquePtr<ASTNode_Operator> parse_expr_arithmetic_add();
        UniquePtr<ASTNode_Operator> parse_expr_arithmetic_subtract();
        UniquePtr<ASTNode_Operator> parse_expr_arithmetic_multiply();
        UniquePtr<ASTNode_Operator> parse_expr_arithmetic_divide();
        UniquePtr<ASTNode_Operator> parse_expr_bitwise_or();
        UniquePtr<ASTNode_Operator> parse_expr_bitwise_xor();
        UniquePtr<ASTNode_Operator> parse_expr_bitwise_and();
        UniquePtr<ASTNode_Operator> parse_expr_arithmetic_mod();
        UniquePtr<ASTNode_Operator> parse_expr_arithmetic_exponent();
        UniquePtr<ASTNode_Operator> parse_expr_member_visit();
        UniquePtr<ASTNode_Operator> parse_expr_unary();
        UniquePtr<ASTNode_Operator> parse_expr_primary();

        // === Composed Expressions ===
        UniquePtr<ASTNode_Operator> parse_expr_evaluate_block();
        UniquePtr<ASTNode_Operator> parse_expr_conditional_evaluate_block();
    };

    lust::UniquePtr<IParser> IParser::create(lexer::TokenStream &token_stream)
    {
        return UniquePtr<IParser>(new Parser(token_stream));
    }

    Parser::Parser(lexer::TokenStream &token_stream)
        : m_token_stream(token_stream)
        , m_current_token(next_token())
    {
    }

    lust::UniquePtr<ASTNode_Program> Parser::parse()
    {
        return parse_program();
    }

    bool Parser::is_error_occurred() const
    {
        return m_error_occurred;
    }

    void Parser::error_msg(std::string_view msg)
    {
        m_error_occurred = true;
        lexer::SourceLoc loc = lexer::pos_to_line_and_row(m_token_stream->original_text(), m_current_token.pos);
        std::cerr << "Error occurred while parsing at L" << loc.line << ":" << loc.row << " :\n\t" << msg << "\n";
    }

    void Parser::error(std::string_view msg)
    {
        error_msg(msg);
        
        // Try to recover
        m_current_token = next_token();
    }

    lexer::Token Parser::next_token()
    {
        lexer::Token current = m_token_stream->next_token();

        // Ignore comment for now
        // TODO: Comment might useful while generating documents
        while (current.type == lexer::TerminalTokenType::COMMENTVAL) {
            current = m_token_stream->next_token();
        }

        return current;
    }

    bool Parser::expected(lexer::TerminalTokenType expected_type, std::string_view failure_msg)
    {
        if (m_current_token.type == expected_type) {
            m_current_token = next_token();
            return true;
        } else {
            std::stringstream s;
            s << "Unexpected token. Expected " << lexer::token_type_to_string(expected_type) << ", found " << lexer::token_type_to_string(m_current_token.type) << ".";
            if (!failure_msg.empty()) {
                s << "\n\tReason: " << failure_msg;
            }
            s << std::endl;
            error(s.str());
            return false;
        }
    }

    bool Parser::optional(lexer::TerminalTokenType expected_type)
    {
        if (m_current_token.type == expected_type) {
            m_current_token = next_token();
            return true;
        }
        return false;
    }

    UniquePtr<ASTNode_Program> Parser::parse_program()
    {
        UniquePtr<ASTNode_Program> node = lust::make_unique<ASTNode_Program>();
        while (true) {
            switch (m_current_token.type)
            {
            case lexer::TerminalTokenType::ERROR:
            case lexer::TerminalTokenType::END:
                return node;

            case lexer::TerminalTokenType::GLOBAL_ATTRIBUTE_START:
                node->attributes.extend(parse_attribute_declaration());
                break;

            case lexer::TerminalTokenType::ATTRIBUTE_START:
                node->statements.push_back(parse_statement_with_attributes());
                break;
            
            default:
                node->statements.push_back(parse_statement());
                break;
            }
        }
        return node;
    }

    UniquePtr<ASTNode_Statement> Parser::parse_statement()
    {
        UniquePtr<ASTNode_Statement> statement = nullptr;

        switch (m_current_token.type)
        {
        case lexer::TerminalTokenType::LET:
        case lexer::TerminalTokenType::CONST:
            statement = parse_variable_declaration();
            break;
        case lexer::TerminalTokenType::ASYNC:
        case lexer::TerminalTokenType::FN:
            statement = parse_function_declaration();
            break;
        case lexer::TerminalTokenType::PUB:
            statement = parse_statement_pub_prefix();
            break;
        case lexer::TerminalTokenType::LBRACE:
            statement = parse_code_block();
            break;
        case lexer::TerminalTokenType::STRUCT:
            statement = parse_struct_declaration();
            break;
        case lexer::TerminalTokenType::TRAIT:
            statement = parse_trait_declaration();
            break;
        
        default:
            auto expr = parse_expression();
            if (!expr || expr->operator_type == OperatorType::INVALID) {
                error("Invalid statement");
            } else {
                auto new_statement = make_unique<ASTNode_ExprStatement>();
                new_statement->expression = std::move(expr);
                if (!optional(lexer::TerminalTokenType::SEMICOLON)) {
                    new_statement->is_end_with_semicolon = false;
                }
                statement = std::move(new_statement);
            }
            break;
        }

        return statement;
    }

    UniquePtr<ASTNode_Statement> Parser::parse_statement_pub_prefix()
    {
        Visibility visibility = parse_visibility();
        UniquePtr<ASTNode_Statement> statement = parse_statement();
        statement->visibility = visibility;
        return statement;
    }

    UniquePtr<ASTNode_VarDecl> Parser::parse_variable_declaration()
    {
        UniquePtr<ASTNode_VarDecl> res = make_unique<ASTNode_VarDecl>();

        bool is_let = false;
        bool is_const = false;
        if (optional(lexer::TerminalTokenType::LET)) is_let = true;
        if (optional(lexer::TerminalTokenType::CONST)) {
            res->is_const = true;
            is_const = true;
        }

        if ( is_let && is_const ) {
            error("Keyword 'let' and 'const' is appearing at the same time");
            return nullptr;
        }

        if (optional(lexer::TerminalTokenType::MUT)) {
            res->is_mutable = true;
        }

        res->identifier = m_current_token.value;
        expected(lexer::TerminalTokenType::IDENT);

        // type is optional if it can be infered from assign expression
        if (optional(lexer::TerminalTokenType::COLON)) {
            res->specified_type = parse_type_expr();
        }

        if (optional(lexer::TerminalTokenType::SEMICOLON)) {
            res->is_forward_decl_only = true;
            return res;
        }

        expected(lexer::TerminalTokenType::EQ);

        res->evaluate_expression = parse_expression();

        expected(lexer::TerminalTokenType::SEMICOLON, "Variable declaration must be ended with ';'");

        return res;
    }

    UniquePtr<ASTNode_FunctionDecl> Parser::parse_function_declaration()
    {
        UniquePtr<ASTNode_FunctionDecl> function = make_unique<ASTNode_FunctionDecl>();

        function->is_async = optional(lexer::TerminalTokenType::ASYNC);
        expected(lexer::TerminalTokenType::FN);
        function->identifier = m_current_token.value;
        expected(lexer::TerminalTokenType::IDENT);
        function->generic_params = try_parse_generic_params();

        expected(lexer::TerminalTokenType::LPAREN);
        
        while (!optional(lexer::TerminalTokenType::RPAREN)) {
            if (auto param = parse_invokable_wanted_param(); param) {
                function->params->params.push_back(param);
            }

            if (!optional(lexer::TerminalTokenType::COMMA)) {
                expected(lexer::TerminalTokenType::RPAREN);
                break;
            }

        }

        if (optional(lexer::TerminalTokenType::ARROW)) {
            function->ret_type = parse_type_expr();
        } else {
            function->ret_type = create_unit_type();
        }

        if (optional(lexer::TerminalTokenType::SEMICOLON)) {
            return function;
        } else if (lexer::TerminalTokenType::LBRACE == m_current_token.type) {
            function->body = parse_code_block();

            return function;
        }

        error("Unterminated function declaration");
        return nullptr;
    }

    vector<UniquePtr<ASTNode_Attribute>> Parser::parse_attribute_declaration()
    {
        // #[label::label, label2(ident, ident)]

        vector<UniquePtr<ASTNode_Attribute>> attributes;

        if (optional(lexer::TerminalTokenType::GLOBAL_ATTRIBUTE_START)) {
            expected(lexer::TerminalTokenType::LBRACKET);
        } else {
            expected(lexer::TerminalTokenType::ATTRIBUTE_START);
        }

        auto parse_item = [&] () -> UniquePtr<ASTNode_Attribute> {
            auto result = make_unique<ASTNode_Attribute>();
            result->name = parse_qualifier_name();
            if (optional(lexer::TerminalTokenType::LPAREN)) {
                while (m_current_token.type != lexer::TerminalTokenType::RPAREN) {
                    if (!expected(lexer::TerminalTokenType::IDENT)) {
                        result->args.push_back(m_current_token.value);
                        break;
                    }
                }
                expected(lexer::TerminalTokenType::RPAREN);
            }
            return result;
        };

        // Doesn't allow empty attribute declaration
        attributes.push_back(parse_item());

        while (m_current_token.type == lexer::TerminalTokenType::COMMA) {
            expected(lexer::TerminalTokenType::COMMA);
            attributes.push_back(parse_item());
        }

        expected(lexer::TerminalTokenType::RBRACKET, "Attribute should be closed");

        return attributes;
    }

    QualifiedName Parser::parse_qualifier_name()
    {
        vector<simple_string> parts;
        parts.push_back(m_current_token.value);
        expected(lexer::TerminalTokenType::IDENT);

        while (m_current_token.type == lexer::TerminalTokenType::COLONCOLON) {
            expected(lexer::TerminalTokenType::COLONCOLON);
            parts.push_back(m_current_token.value);
            expected(lexer::TerminalTokenType::IDENT);
        }
        
        return QualifiedName {
            parts.back(),
            parts.slice(0, parts.size() - 1),
        };
    }

    Visibility Parser::parse_visibility()
    {
        Visibility res = Visibility::PUBLIC;

        expected(lexer::TerminalTokenType::PUB);

        if (optional(lexer::TerminalTokenType::LPAREN)) {

            if (optional(lexer::TerminalTokenType::SELF)) {
                res = Visibility::SELF;
            }
            if (optional(lexer::TerminalTokenType::SUPER)) {
                res = Visibility::SUPER;
            }
            if (optional(lexer::TerminalTokenType::CRATE)) {
                res = Visibility::CRATE;
            }

            expected(lexer::TerminalTokenType::RPAREN);
        }

        return res;
    }

    UniquePtr<ASTNode_GenericParam> Parser::parse_generic_param()
    {
        UniquePtr<ASTNode_GenericParam> res = make_unique<ASTNode_GenericParam>();

        res->types.push_back(parse_type_expr());

        if (optional(lexer::TerminalTokenType::COLON)) {
            do {
                res->constraints.push_back(parse_qualifier_name());
                if (!optional(lexer::TerminalTokenType::PLUS))
                    break;
            } while (true);
        }

        return res;
    }

    vector<UniquePtr<ASTNode_GenericParam>> Parser::try_parse_generic_params()
    {
        if (optional(lexer::TerminalTokenType::LT)) {
            vector<UniquePtr<ASTNode_GenericParam>> res;

            while (!optional(lexer::TerminalTokenType::GT)) {
                if (auto param = parse_generic_param()) {
                    res.push_back(std::move(param));
                } else {
                    break;
                }

                if (!optional(lexer::TerminalTokenType::COMMA)) {
                    expected(lexer::TerminalTokenType::GT);
                    break;
                }
            }

            return res;
        }
        return {};
    }

    UniquePtr<ASTNode_TypeExpr> Parser::parse_trival_type()
    {

        QualifiedName type_name = parse_qualifier_name();

        if (m_current_token.type == lexer::TerminalTokenType::LT) {
            UniquePtr<ASTNode_TypeExpr_Generic> res = make_unique<ASTNode_TypeExpr_Generic>();
            res->base_type = type_name;
            res->params = try_parse_generic_params();
            return res;
        }

        UniquePtr<ASTNode_TypeExpr_Trivial> res = make_unique<ASTNode_TypeExpr_Trivial>();

        res->type_name = type_name;

        return res;
    }

    UniquePtr<ASTNode_TypeExpr_Tuple> Parser::parse_tuple_type()
    {
        UniquePtr<ASTNode_TypeExpr_Tuple> res = make_unique<ASTNode_TypeExpr_Tuple>();

        expected(lexer::TerminalTokenType::LPAREN);

        while (!optional(lexer::TerminalTokenType::RPAREN)) {
            if (auto type_exp = parse_type_expr(); type_exp) {
                res->composite_types.push_back(std::move(type_exp));
            } else {
                break;
            }

            if (!optional(lexer::TerminalTokenType::COMMA)) {
                expected(lexer::TerminalTokenType::LPAREN, "Expected ',' or ')' in tuple list");
                break;
            }
        }

        return res;
    }

    UniquePtr<ASTNode_TypeExpr_Array> Parser::parse_array_type()
    {
        UniquePtr<ASTNode_TypeExpr_Array> res = make_unique<ASTNode_TypeExpr_Array>();

        expected(lexer::TerminalTokenType::LBRACKET);

        res->array_type = parse_type_expr();

        expected(lexer::TerminalTokenType::SEMICOLON);

        std::string num_string = m_current_token.get_value();
        if (expected(lexer::TerminalTokenType::INT, "It must be an integer to describe array size")) {
            Number<size_t> num = convert_string_to_number<size_t>(num_string);
            if (num.is_null) {
                error_msg("Weird integer, it might cause undefined behavior");
            }
            res->array_size = num.value;
        }

        expected(lexer::TerminalTokenType::RBRACKET);

        return res;
    }

    UniquePtr<ASTNode_TypeExpr_Reference> Parser::parse_reference_type()
    {
        UniquePtr<ASTNode_TypeExpr_Reference> res = make_unique<ASTNode_TypeExpr_Reference>();

        expected(lexer::TerminalTokenType::BITAND);

        res->referenced_type = parse_type_expr();

        return res;
    }

    UniquePtr<ASTNode_TypeExpr_Function> Parser::parse_function_type()
    {
        UniquePtr<ASTNode_TypeExpr_Function> res = make_unique<ASTNode_TypeExpr_Function>();

        expected(lexer::TerminalTokenType::FN);
        expected(lexer::TerminalTokenType::LPAREN);

        if (!optional(lexer::TerminalTokenType::RPAREN)) {
            do {
                if (auto type_exp = parse_type_expr(); type_exp) {
                    res->param_types.push_back(type_exp);
                } else {
                    break;
                }

                if (optional(lexer::TerminalTokenType::COMMA)) {
                    continue;
                } else if (optional(lexer::TerminalTokenType::RPAREN)) {
                    break;
                } else {
                    error("Expected ',' or ')' in function type parameter list");
                    return nullptr;
                }

            } while (true);
        }

        if (optional(lexer::TerminalTokenType::ARROW)) {
            res->return_type = parse_type_expr();
        } else {
            res->return_type = create_unit_type();
        }

        return res;
    }

    UniquePtr<ASTNode_TypeExpr> Parser::parse_type_expr()
    {
        if (lexer::TerminalTokenType::FN == m_current_token.type) {
            return UniquePtr<ASTNode_TypeExpr>(parse_function_type());
        } else if (lexer::TerminalTokenType::LBRACKET == m_current_token.type) {
            return UniquePtr<ASTNode_TypeExpr>(parse_array_type());
        } else if (lexer::TerminalTokenType::LPAREN == m_current_token.type) {
            return UniquePtr<ASTNode_TypeExpr>(parse_tuple_type());
        } else if (lexer::TerminalTokenType::BITAND == m_current_token.type) {
            return UniquePtr<ASTNode_TypeExpr>(parse_reference_type());
        } else if (lexer::TerminalTokenType::IDENT == m_current_token.type) {
            return parse_trival_type();
        }

        error("Failed to parse type expr");
        return nullptr;
    }

    UniquePtr<ASTNode_TypeExpr> Parser::create_unit_type()
    {
        return make_unique<ASTNode_TypeExpr>();
    }

    UniquePtr<ASTNode_ParamDecl> Parser::parse_invokable_wanted_param()
    {
        UniquePtr<ASTNode_ParamDecl> res = make_unique<ASTNode_ParamDecl>();

        if (optional(lexer::TerminalTokenType::SELF)) {
            res->is_instance_function = true;
            res->identifier = m_current_token.value;
        } else if (optional(lexer::TerminalTokenType::IDENT)) {
            res->identifier = m_current_token.value;

            expected(lexer::TerminalTokenType::COLON);

            if (auto expr = parse_type_expr()) {
                res->type = std::move(expr);
            } else {
                return nullptr;
            }
        } else {
            error("Expected parameter name or 'self'");
            return nullptr;
        }

        return res;
    }

    UniquePtr<ASTNode_InvokeParameters> Parser::parse_invoke_param_list() {
        auto new_node = make_unique<ASTNode_InvokeParameters>();

        expected(lexer::TerminalTokenType::LPAREN);
        
        while (!optional(lexer::TerminalTokenType::RPAREN)) {
            new_node->parameter_expressions.push_back(parse_expression());

            // Every parameter expression should follow a comma
            if (!optional(lexer::TerminalTokenType::COMMA)) {
                expected(lexer::TerminalTokenType::RPAREN);
                break;
            }
        }
        
        return new_node;
    }

    UniquePtr<ASTNode_Block> Parser::parse_code_block() {
        UniquePtr<ASTNode_Block> res = make_unique<ASTNode_Block>();

        expected(lexer::TerminalTokenType::LBRACE);

        while (!optional(lexer::TerminalTokenType::RBRACE)) {
            res->statements.push_back(parse_statement());
        }

        return res;
    }

    UniquePtr<ASTNode_Statement> Parser::parse_statement_with_attributes() {
        vector<UniquePtr<ASTNode_Attribute>> attributes;
        while (lexer::TerminalTokenType::ATTRIBUTE_START == m_current_token.type) {
            attributes.extend(parse_attribute_declaration());
        }
        auto statement = parse_statement();
        if (statement) {
            statement->attributes = std::move(attributes);
        }
        return statement;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expression() {
        return parse_expr_assignment();
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_assignment() {
        UniquePtr<ASTNode_Operator> node = parse_expr_logical_or();

        while (lexer::is_assignment_token(m_current_token.type)) {
            UniquePtr<ASTNode_Operator> new_node = make_unique<ASTNode_Operator>();
            switch (m_current_token.type) {
                case lexer::TerminalTokenType::EQ:
                    new_node->operator_type = OperatorType::ASSIGNMENT;
                    break;
                case lexer::TerminalTokenType::PLUS_EQUAL:
                    new_node->operator_type = OperatorType::ASSIGNMENT_ADD;
                    break;
                case lexer::TerminalTokenType::MINUS_EQUAL:
                    new_node->operator_type = OperatorType::ASSIGNMENT_SUBTRACT;
                    break;
                case lexer::TerminalTokenType::STAR_EQUAL:
                    new_node->operator_type = OperatorType::ASSIGNMENT_MULTIPLY;
                    break;
                case lexer::TerminalTokenType::SLASH_EQUAL:
                    new_node->operator_type = OperatorType::ASSIGNMENT_DIVIDE;
                    break;
                case lexer::TerminalTokenType::OR_EQUAL:
                    new_node->operator_type = OperatorType::ASSIGNMENT_BITWISE_OR;
                    break;
                case lexer::TerminalTokenType::AND_EQUAL:
                    new_node->operator_type = OperatorType::ASSIGNMENT_BITWISE_AND;
                    break;
                case lexer::TerminalTokenType::XOR_EQUAL:
                    new_node->operator_type = OperatorType::ASSIGNMENT_BITWISE_XOR;
                    break;
                case lexer::TerminalTokenType::PRECENTAGE_EQUAL:
                    new_node->operator_type = OperatorType::ASSIGNMENT_MOD;
                    break;
                default:
                    new_node->operator_type = OperatorType::INVALID;
                    break;
            }
            expected(m_current_token.type);
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_logical_or();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_logical_or() {
        auto node = parse_expr_logical_and();

        while (optional(lexer::TerminalTokenType::OR)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::LOGICAL_OR;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_logical_and();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_logical_and() {
        auto node = parse_expr_logical_equality();

        while (optional(lexer::TerminalTokenType::AND)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::LOGICAL_AND;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_logical_equality();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_logical_equality() {
        auto node = parse_expr_logical_none_equality();

        while (optional(lexer::TerminalTokenType::EQEQ)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::LOGICAL_EQUALITY;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_logical_none_equality();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_logical_none_equality() {
        auto node = parse_expr_logical_less();

        while (optional(lexer::TerminalTokenType::NEQ)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::LOGICAL_NONE_EQUALITY;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_logical_less();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_logical_less() {
        auto node = parse_expr_logical_less_equalty();

        while (optional(lexer::TerminalTokenType::LT)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::LOGICAL_RELATION_LESS_THAN;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_logical_none_equality();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_logical_less_equalty() {
        auto node = parse_expr_logical_greater();

        while (optional(lexer::TerminalTokenType::LTE)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::LOGICAL_RELATION_LESS_THAN_EQUALITY;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_logical_greater();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_logical_greater() {
        auto node = parse_expr_logical_greater_equalty();

        while (optional(lexer::TerminalTokenType::GT)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::LOGICAL_RELATION_GREATER_THAN;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_logical_greater_equalty();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_logical_greater_equalty() {
        auto node = parse_expr_arithmetic_add();

        while (optional(lexer::TerminalTokenType::GTE)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::LOGICAL_RELATION_GREATER_THAN_EQUALITY;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_arithmetic_add();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_arithmetic_add() {
        auto node = parse_expr_arithmetic_subtract();

        while (optional(lexer::TerminalTokenType::PLUS)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::ARITHMETIC_ADD;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_arithmetic_subtract();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_arithmetic_subtract() {
        auto node = parse_expr_arithmetic_multiply();

        while (optional(lexer::TerminalTokenType::MINUS)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::ARITHMETIC_SUBTRACT;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_arithmetic_multiply();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_arithmetic_multiply() {
        auto node = parse_expr_arithmetic_divide();

        while (optional(lexer::TerminalTokenType::STAR)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::ARITHMETIC_MULTIPLY;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_arithmetic_divide();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_arithmetic_divide() {
        auto node = parse_expr_bitwise_or();

        while (optional(lexer::TerminalTokenType::SLASH)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::ARITHMETIC_DIVIDE;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_bitwise_or();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_bitwise_or() {
        auto node = parse_expr_bitwise_xor();

        while (optional(lexer::TerminalTokenType::BITOR)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::BITWISE_OR;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_bitwise_xor();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_bitwise_xor() {
        auto node = parse_expr_bitwise_and();

        while (optional(lexer::TerminalTokenType::BITXOR)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::BITWISE_XOR;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_bitwise_and();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_bitwise_and() {
        auto node = parse_expr_arithmetic_mod();

        while (optional(lexer::TerminalTokenType::BITAND)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::BITWISE_AND;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_arithmetic_mod();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_arithmetic_mod() {
        auto node = parse_expr_arithmetic_exponent();

        while (optional(lexer::TerminalTokenType::PRECENTAGE)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::ARITHMETIC_MOD;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_arithmetic_exponent();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_arithmetic_exponent() {
        auto node = parse_expr_member_visit();

        while (optional(lexer::TerminalTokenType::STARSTAR)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::ARITHMETIC_EXPONENT;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_member_visit();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_member_visit() {
        auto node = parse_expr_unary();

        while (optional(lexer::TerminalTokenType::DOT)) {
            auto new_node = make_unique<ASTNode_Operator>();
            new_node->operator_type = OperatorType::MEMBER_VISIT;
            new_node->left_oprand = std::move(node);
            new_node->right_oprand = parse_expr_unary();
            node = std::move(new_node);
        }

        return node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_unary() {
        if (lexer::is_unary_token(m_current_token.type)) {
            auto new_node = make_unique<ASTNode_Operator>();
            switch (m_current_token.type) {
                case lexer::TerminalTokenType::MINUS:
                    new_node->operator_type = OperatorType::UNARY_ARITHMETIC_SELF_CHANGE_SIGN;
                    break;
                case lexer::TerminalTokenType::PLUSPLUS:
                    new_node->operator_type = OperatorType::UNARY_ARITHMETIC_SELF_INCREASE;
                    break;
                case lexer::TerminalTokenType::MINUSMINUS:
                    new_node->operator_type = OperatorType::UNARY_ARITHMETIC_SELF_DECREASE;
                    break;
                case lexer::TerminalTokenType::NOT:
                    new_node->operator_type = OperatorType::UNARY_LOGICAL_NOT;
                    break;
                case lexer::TerminalTokenType::BITINV:
                    new_node->operator_type = OperatorType::UNARY_BITWISE_INVERSE;
                    break;
                default:
                    new_node->operator_type = OperatorType::INVALID;
            }
            expected(m_current_token.type);
            new_node->right_oprand = parse_expr_unary();
            return new_node;
        }

        return parse_expr_primary();
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_primary() {
        if (lexer::TerminalTokenType::INT == m_current_token.type) {
            auto res = make_unique<ASTNode_IntegerExpr>();
            res->operator_type = OperatorType::LITERAL_INTEGER;
            expected(lexer::TerminalTokenType::INT);
            return res;
        } else if (lexer::TerminalTokenType::FLOAT == m_current_token.type) {
            auto res = make_unique<ASTNode_FloatExpr>();
            res->operator_type = OperatorType::LITERAL_FLOAT;
            expected(lexer::TerminalTokenType::FLOAT);
            return res;
        } else if (lexer::TerminalTokenType::STRING == m_current_token.type) {
            auto res = make_unique<ASTNode_StringExpr>();
            res->operator_type = OperatorType::LITERAL_STRING;
            expected(lexer::TerminalTokenType::STRING);
            return res;
        } else if (lexer::TerminalTokenType::IDENT == m_current_token.type) {
            UniquePtr<ASTNode_QualifiedName> res = make_unique<ASTNode_QualifiedName>();
            res->operator_type = OperatorType::VARIABLE;
            res->qualified_name = parse_qualifier_name();
            if (lexer::TerminalTokenType::LPAREN == m_current_token.type) {
                // function call, not (expr + expr) etc.
                res->operator_type = OperatorType::FUNCTION_CALL;
                res->passing_parameters = parse_invoke_param_list();
            }
            return res;
        } else if (optional(lexer::TerminalTokenType::LPAREN)) {
            auto node = parse_expression();
            expected(lexer::TerminalTokenType::RPAREN);
            return node;
        } else if (lexer::TerminalTokenType::LBRACE == m_current_token.type) {
            auto node = parse_expr_evaluate_block();
            return node;
        } else if (lexer::TerminalTokenType::IF == m_current_token.type) {
            auto node = parse_expr_conditional_evaluate_block();
            return node;
        }

        return nullptr;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_evaluate_block() {
        auto new_node = make_unique<ASTNode_BlockExpr>();

        new_node->operator_type = OperatorType::BLOCK;
        new_node->left_code_block = parse_code_block();

        return new_node;
    }

    UniquePtr<ASTNode_Operator> Parser::parse_expr_conditional_evaluate_block() {
        // TODO
        error("if-expr doesn't implemented yet");
        return nullptr;
    }

    UniquePtr<ASTNode_StructDecl> Parser::parse_struct_declaration() {
        auto new_node = make_unique<ASTNode_StructDecl>();

        expected(lexer::TerminalTokenType::STRUCT);

        if (m_current_token.type == lexer::TerminalTokenType::IDENT) {
            new_node->identifier = m_current_token.value;
        }
        expected(lexer::TerminalTokenType::IDENT);

        new_node->generic_params = try_parse_generic_params();

        expected(lexer::TerminalTokenType::LBRACE);

        while (!optional(lexer::TerminalTokenType::RBRACE)) {
            new_node->fields.push_back(parse_struct_field_declaration());
        }

        while (optional(lexer::TerminalTokenType::SEMICOLON)) ;

        return new_node;
    }

    UniquePtr<ASTNode_StructField> Parser::parse_struct_field_declaration() {
        auto new_node = make_unique<ASTNode_StructField>();

        if (lexer::TerminalTokenType::IDENT == m_current_token.type) {
            new_node->identifier = m_current_token.value;
        }
        expected(lexer::TerminalTokenType::IDENT);

        expected(lexer::TerminalTokenType::COLON);

        new_node->field_type = parse_type_expr();

        if (!optional(lexer::TerminalTokenType::COMMA)) {
            optional(lexer::TerminalTokenType::SEMICOLON);
        }

        return new_node;
    }

    UniquePtr<ASTNode_TraitDecl> Parser::parse_trait_declaration() {
        auto new_node = make_unique<ASTNode_TraitDecl>();

        expected(lexer::TerminalTokenType::TRAIT);
        if (m_current_token.type == lexer::TerminalTokenType::IDENT) {
            new_node->identifier = m_current_token.value;
        }
        expected(lexer::TerminalTokenType::IDENT);

        new_node->generic_params = try_parse_generic_params();

        // Traits without a body
        if (optional(lexer::TerminalTokenType::SEMICOLON)) {
            return new_node;
        }

        expected(lexer::TerminalTokenType::LBRACE);

        while (!optional(lexer::TerminalTokenType::RBRACE)) {
            if (lexer::TerminalTokenType::TYPE == m_current_token.type) {
                new_node->morphisms_types.push_back(parse_morphisms_type());
                expected(lexer::TerminalTokenType::SEMICOLON);
            } else if (lexer::TerminalTokenType::CONST == m_current_token.type) {
                new_node->morphisms_constants.push_back(parse_morphisms_constant());
                expected(lexer::TerminalTokenType::SEMICOLON);
            } else {
                if (auto func = parse_function_declaration()) {
                    new_node->functions.push_back(func);
                } else {
                    break;
                }
            }

            while (optional(lexer::TerminalTokenType::SEMICOLON)) {}
        }

        return new_node;
    }

    UniquePtr<ASTNode_MorphismsType> Parser::parse_morphisms_type() {
        auto new_node = make_unique<ASTNode_MorphismsType>();

        expected(lexer::TerminalTokenType::TYPE);

        if (lexer::TerminalTokenType::IDENT == m_current_token.type) {
            new_node->identifier = m_current_token.get_value();
        }
        expected(lexer::TerminalTokenType::IDENT);

        if (optional(lexer::TerminalTokenType::SEMICOLON)) {
            return new_node;
        }

        expected(lexer::TerminalTokenType::EQ);

        new_node->value = parse_type_expr();

        return new_node;
    }

    UniquePtr<ASTNode_MorphismsConstant> Parser::parse_morphisms_constant() {
        auto new_node = make_unique<ASTNode_MorphismsConstant>();

        expected(lexer::TerminalTokenType::CONST);

        if (lexer::TerminalTokenType::IDENT == m_current_token.type) {
            new_node->identifier = m_current_token.get_value();
        }
        expected(lexer::TerminalTokenType::IDENT);

        expected(lexer::TerminalTokenType::COLON);

        new_node->type = parse_type_expr();

        if (optional(lexer::TerminalTokenType::SEMICOLON)) {
            return new_node;
        }

        expected(lexer::TerminalTokenType::EQ);

        new_node->value = parse_expression();

        return new_node;
    }
}
}
