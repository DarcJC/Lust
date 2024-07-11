#include "parser.hpp"

#include <string_view>
#include <iostream>
#include <sstream>
#include <utility>

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

        Visibility consume_visibility();

        vector<UniquePtr<ASTNode_Attribute>> consume_attributes();
    private:
        lexer::TokenStream& m_token_stream;
        lexer::Token m_current_token{};
        vector<UniquePtr<ASTNode_Attribute>> m_pending_attributes;
        Visibility m_scope_visibility = Visibility::DEFAULT;

        bool m_error_occurred = false;

    private:
        UniquePtr<ASTNode_Program> parse_program();
    
        UniquePtr<ASTNode_Statement> parse_statement();

        UniquePtr<ASTNode_VarDecl> parse_variable_declaration();

        UniquePtr<ASTNode_FunctionDecl> parse_function_declaration();

        vector<UniquePtr<ASTNode_Attribute>> parse_attribute_declaration();

        QualifierName parse_qualifier_name();

        Visibility parse_visibility();

        UniquePtr<ASTNode_GenericParam> parse_generic_param();

        UniquePtr<ASTNode_Generic> try_parse_generic();
    };

    lust::UniquePtr<IParser> IParser::create(lexer::TokenStream &token_stream)
    {
        return UniquePtr<IParser>(new Parser(token_stream));
    }

    Parser::Parser(lexer::TokenStream &token_stream)
        : m_token_stream(token_stream)
        , m_current_token(next_token())
        , m_scope_visibility(Visibility::DEFAULT)
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

    void Parser::error(std::string_view msg)
    {
        m_error_occurred = true;
        lexer::SourceLoc loc = lexer::pos_to_line_and_row(m_token_stream->original_text(), m_current_token.pos);
        std::cerr << "Error occurred while parsing at L" << loc.line << ":" << loc.row << " :\n\t" << msg << "\n";
        
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

    Visibility Parser::consume_visibility()
    {
        Visibility res = m_scope_visibility;

        m_scope_visibility = Visibility::DEFAULT;

        return res;
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
        while (m_current_token.type != lexer::TerminalTokenType::END && m_current_token.type != lexer::TerminalTokenType::ERROR) {
            switch (m_current_token.type)
            {
            case lexer::TerminalTokenType::GLOBAL_ATTRIBUTE_START:
                node->attributes.extend(parse_attribute_declaration());
                break;
            case lexer::TerminalTokenType::ATTRIBUTE_START:
                m_pending_attributes.extend(parse_attribute_declaration());
                break;
            case lexer::TerminalTokenType::PUB:
                m_scope_visibility = parse_visibility();
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
        function->generic = try_parse_generic();

        expected(lexer::TerminalTokenType::LPAREN);
        
        expected(lexer::TerminalTokenType::RPAREN);

        return function;
    }

    vector<UniquePtr<ASTNode_Attribute>> Parser::parse_attribute_declaration()
    {
        // #[label::label, label2(ident, ident)]

        vector<UniquePtr<ASTNode_Attribute>> attributes;

        expected(lexer::TerminalTokenType::ATTRIBUTE_START);

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

    QualifierName Parser::parse_qualifier_name()
    {
        vector<simple_string> parts;
        parts.push_back(m_current_token.value);
        expected(lexer::TerminalTokenType::IDENT);

        while (m_current_token.type == lexer::TerminalTokenType::COLONCOLON) {
            expected(lexer::TerminalTokenType::COLONCOLON);
            parts.push_back(m_current_token.value);
            expected(lexer::TerminalTokenType::IDENT);
        }
        
        return QualifierName {
            parts.back(),
            parts.slice(0, parts.size() - 1),
        };
    }

    Visibility Parser::parse_visibility()
    {
        Visibility res = Visibility::PUBLIC;

        if (m_scope_visibility != Visibility::DEFAULT) {
            error("Duplicated visibility specifier.");
        }

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

    UniquePtr<ASTNode_Generic> Parser::try_parse_generic()
    {
        if (optional(lexer::TerminalTokenType::LT)) {
            UniquePtr<ASTNode_Generic> res = make_unique<ASTNode_Generic>();

            do {
                auto param = parse_generic_param();
                res->params.push_back(std::move(param));
                if (!optional(lexer::TerminalTokenType::COMMA))
                    break;
            } while (lexer::TerminalTokenType::GT != m_current_token.type);
        }
        return nullptr;
    }
}
}
