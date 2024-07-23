const char test_data[] = R"LUST(

/// This is a COMMENT
#[label::label, label2()] // Comment here
pub(crate) async fn foo(val: XXX<u8>, val2: Abc) -> () { // Comment here
    let a: u8 = 123;
    let b: Option<AnyType> = None;
}

const TEST = 123.5;

#[derive(Debug)]
struct Foo<T> {
    val: T,
}

impl Display for Foo<u8> {

    fn display(self) -> String {
    }

}

)LUST";

#include "single_file_test.hpp"
#include "lust/parser.hpp"
#include "lust/lexer.hpp"


void entry() {
    lust::lexer::TokenStream lexer = lust::lexer::ITokenizer::create(std::string_view(test_data, sizeof(test_data)));
    lust::UniquePtr<lust::grammar::IParser> parser = lust::grammar::IParser::create(lexer);
    parser->parse();
    // while (lexer->is_cursor_valid()) {
    //     lust::lexer::Token token = lexer->next_token();
    //     if (token.type == lust::lexer::TerminalTokenType::ERROR) {
    //         break;
    //     }
    //     std::cout << lust::lexer::token_type_to_string(token.type) << ": " << token.get_value() << std::endl;
    // }
}
