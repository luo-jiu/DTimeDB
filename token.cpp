#include <iostream>

#include <lexer/lexer.h>
using namespace dt::lexer;

int main() {
    auto lexer = new Lexer("./../sql.txt");
    Json tokens;
    while (true)
    {
        auto token = lexer->next_token();
        token.show();
        tokens.append(token.json());
        if(token.type() == dt::token::Token::TOKEN_EOF)
        {
            break;
        }
    }
}
