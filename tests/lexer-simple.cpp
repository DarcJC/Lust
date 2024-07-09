#include "single_file_test.hpp"
#include "lexer.hpp"

const char test_data[] = 
R"LUST(

/// This is a COMMENT
#[label::label]
fn main(val: XXX<u8>, val2: Abc) -> ! {
    let a: u8 = 123;
    let b: Option<AnyType> = None;
}

const TEST = 123.5;

#[derive(Debug)]
struct Foo<T> {
    val: T
}

impl Display for Foo<u8> {

    fn display(self) -> String {
    }

}

)LUST";

void entry() {
    lust::lexer::ITokenizer* lexer = lust::lexer::ITokenizer::create(std::string_view(test_data, sizeof(test_data)));
    while (lexer->is_cursor_valid()) {
        lust::lexer::Token token = lexer->next_token();
        if (token.type == lust::lexer::TerminalTokenType::ERROR) {
            break;
        }
        std::cout << lust::lexer::token_type_to_string(token.type) << ": " << token.value << std::endl;
    }

    delete lexer;
}
