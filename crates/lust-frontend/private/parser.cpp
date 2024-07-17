#include "parser.hpp"

#include <string_view>
#include <iostream>
#include <sstream>
#include <utility>

#include "grammar/type_expr.hpp"
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
         * @brief Notify a parser error, set parser error flag and comsume current token
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

        vector<UniquePtr<ASTNode_Attribute>> consume_attributes();
    private:
        lexer::TokenStream& m_token_stream;
        lexer::Token m_current_token{};
        vector<UniquePtr<ASTNode_Attribute>> m_pending_attributes;

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

        UniquePtr<ASTNode_TypeExpr_Trivial> parse_trival_type();

        UniquePtr<ASTNode_TypeExpr_Tuple> parse_tuple_type();

        UniquePtr<ASTNode_TypeExpr_Array> parse_array_type();

        UniquePtr<ASTNode_TypeExpr_Reference> parse_reference_type();

        UniquePtr<ASTNode_TypeExpr_Function> parse_function_type();

        UniquePtr<ASTNode_TypeExpr> parse_type_expr();

        UniquePtr<ASTNode_TypeExpr> create_unit_type();

        UniquePtr<ASTNode_InvokeParam> parse_invoke_param();
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

    vector<UniquePtr<ASTNode_Attribute>> Parser::consume_attributes()
    {
        vector<UniquePtr<ASTNode_Attribute>> res = std::move(m_pending_attributes);

        m_pending_attributes = vector<UniquePtr<ASTNode_Attribute>>();

        return res;
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
                m_pending_attributes.extend(parse_attribute_declaration());
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
        
        default:
            error("Invalid statement");
            break;
        }

        if (statement) {
            statement->attributes.extend(consume_attributes());
            m_pending_attributes.clear();
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
        bool is_let = false;
        bool is_const = false;
        if (optional(lexer::TerminalTokenType::LET)) is_let = true;
        if (optional(lexer::TerminalTokenType::CONST)) is_const = true;

        if ( is_let && is_const ) {
            error("Token 'let' and 'const' is appearing at the same time");
            return nullptr;
        }

        return nullptr;
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
        
        do {

            if (auto param = parse_invoke_param(); param) {
                function->params->params.push_back(param);
            } else {
                break;
            }

            if (optional(lexer::TerminalTokenType::RPAREN)) {
                break;
            } else if (optional(lexer::TerminalTokenType::COMMA)) {
                continue;
            } else {
                error("Function parameter list expected ',' and ')'");
                break;
            }

        } while(true);
        
        if (optional(lexer::TerminalTokenType::ARROW)) {
            function->ret_type = parse_type_expr();
        } else {
            function->ret_type = create_unit_type();
        }

        if (optional(lexer::TerminalTokenType::SEMICOLON)) {
            return function;
        } else if (optional(lexer::TerminalTokenType::LBRACE)) {
            // Parse function body

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

        res->identifier = m_current_token.value;
        expected(lexer::TerminalTokenType::IDENT);

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

            do {
                auto param = parse_generic_param();
                res.push_back(std::move(param));
                if (!optional(lexer::TerminalTokenType::COMMA)) {
                    expected(lexer::TerminalTokenType::GT);
                    break;
                }
            } while (optional(lexer::TerminalTokenType::GT));

            return res;
        }
        return {};
    }

    UniquePtr<ASTNode_TypeExpr_Trivial> Parser::parse_trival_type()
    {

        QualifiedName type_name = parse_qualifier_name();

        if (m_current_token.type == lexer::TerminalTokenType::LT) {
            UniquePtr<ASTNode_TypeExpr_Generic> res = make_unique<ASTNode_TypeExpr_Generic>();
            res->base_type = type_name;
            res->params = try_parse_generic_params();
        }

        UniquePtr<ASTNode_TypeExpr_Trivial> res = make_unique<ASTNode_TypeExpr_Trivial>();

        res->type_name = type_name;

        return res;
    }

    UniquePtr<ASTNode_TypeExpr_Tuple> Parser::parse_tuple_type()
    {
        UniquePtr<ASTNode_TypeExpr_Tuple> res = make_unique<ASTNode_TypeExpr_Tuple>();

        expected(lexer::TerminalTokenType::LPAREN);

        if (!optional(lexer::TerminalTokenType::RPAREN)) {
            do {
                if (auto type_exp = parse_type_expr(); type_exp) {
                    res->composite_types.push_back(std::move(type_exp));
                } else {
                    break;
                }

                if (optional(lexer::TerminalTokenType::COMMA)) {
                    continue;
                } else if (optional(lexer::TerminalTokenType::RPAREN)) {
                    break;
                } else {
                    error("Expected ',' or ')' in tuple list");
                    return nullptr;
                }

            } while (true);
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
            return UniquePtr<ASTNode_TypeExpr>(parse_trival_type());
        }

        error("");
        return nullptr;
    }

    UniquePtr<ASTNode_TypeExpr> Parser::create_unit_type()
    {
        return make_unique<ASTNode_TypeExpr>();
    }

    UniquePtr<ASTNode_InvokeParam> Parser::parse_invoke_param()
    {
        UniquePtr<ASTNode_InvokeParam> res = make_unique<ASTNode_InvokeParam>();

        if (expected(lexer::TerminalTokenType::IDENT)) {
            res->identifier = m_current_token.value;
        }

        expected(lexer::TerminalTokenType::COLON);

        if (auto expr = parse_type_expr(); expr) {
            res->type = std::move(expr);
            return res;
        }

        return nullptr;
    }
}
}
