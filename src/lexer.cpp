#include "lexer.hpp"
#include <fstream>
#include <iostream>

static bimap<token_type, std::string> const &get_token_bimap() {
    static bimap<token_type, std::string> const token_bimap{
            {token_type::WHITESPACE, "WHITESPACE"},
            {token_type::IDENTIFIER, "IDENTIFIER"},
            {token_type::STRING_LITERAL, "STRING_LITERAL"},
            {token_type::INTEGER_LITERAL, "INTEGER_LITERAL"},
            {token_type::FLOAT_LITERAL, "FLOAT_LITERAL"},
            {token_type::DOT, "DOT"},
            {token_type::COMMA, "COMMA"},
            {token_type::PLUS, "PLUS"},
            {token_type::MINUS, "MINUS"},
            {token_type::LESS_THAN, "LESS_THAN"},
            {token_type::GREATER_THAN, "GREATER_THAN"},
            {token_type::EQUAL, "EQUAL"},
            {token_type::SLASH, "SLASH"},
            {token_type::BACKSLASH, "BACKSLASH"},
            {token_type::STAR, "STAR"},
            {token_type::COMMENT, "COMMENT"},
    };
    return token_bimap;
}

static void end_token(std::vector<token> &tokens, token &token) {
    if (!(token.type == token_type::WHITESPACE || token.type == token_type::COMMENT)) {
        tokens.push_back(token);
    }
    token.type = token_type::WHITESPACE;
    token.text.clear();
}

static std::vector<token> lex_stream(std::string const &file_path, std::istream &in_stream) {
    std::vector<token> tokens{};
    token cur_token{.file_path = file_path};

    char ch;
    while (in_stream.get(ch) && !in_stream.eof()) {
        std::cout.put(ch);
        if (cur_token.type == token_type::COMMENT) {
            if (ch == '\n') {
                end_token(tokens, cur_token);
                cur_token.line++;
                cur_token.column = 1;
                continue;
            } else {
                cur_token.text += ch;
            }
        } else if (cur_token.type == token_type::STRING_LITERAL) {
            if (ch == '\"') {
                end_token(tokens, cur_token);
            } else {
                cur_token.text += ch;
            }
        } else {
            switch (ch) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    if (cur_token.type == token_type::WHITESPACE) {
                        cur_token.type = token_type::INTEGER_LITERAL;
                        cur_token.text += ch;
                    } else {
                        cur_token.text += ch;
                    }
                    break;
                case '\"':
                    end_token(tokens, cur_token);
                    cur_token.type = token_type::STRING_LITERAL;
                    break;
                case '.':
                    if (cur_token.type == token_type::INTEGER_LITERAL) {
                        cur_token.type = token_type::FLOAT_LITERAL;
                        cur_token.text += ch;
                    } else {
                        cur_token.type = token_type::DOT;
                    }
                    break;
                case ',':
                    cur_token.type = token_type::COMMA;
                    break;
                case '+':
                    cur_token.type = token_type::PLUS;
                    break;
                case '-':
                    cur_token.type = token_type::MINUS;
                    break;
                case '<':
                    cur_token.type = token_type::LESS_THAN;
                    break;
                case '>':
                    cur_token.type = token_type::GREATER_THAN;
                    break;
                case '=':
                    cur_token.type = token_type::EQUAL;
                    break;
                case ' ':
                case '\t':
                case '\r':
                    end_token(tokens, cur_token);
                    break;

                case '\n':
                    end_token(tokens, cur_token);
                    cur_token.line++;
                    cur_token.column = 1;
                    continue;

                case '/':
                    if (cur_token.type == token_type::WHITESPACE) {
                        cur_token.type = token_type::SLASH;
                    } else if (cur_token.type == token_type::SLASH) {
                        cur_token.type = token_type::COMMENT;
                    }
                    break;

                default:
                    // allow identifiers to start with an integer (e.g., "2dup")
                    if (cur_token.type == token_type::WHITESPACE || cur_token.type == token_type::INTEGER_LITERAL) {
                        cur_token.type = token_type::IDENTIFIER;
                    }
                    cur_token.text += (char) toupper(ch);
                    break;
            }
        }
        cur_token.column++;
    }
    return tokens;
}

std::vector<token> lex_file(std::string const &file_path) {
    std::cout << "***** lexing file *****" << std::endl;

    std::ifstream f{file_path};
    if (!f.is_open()) {
        std::cerr << "[ERR] Unable to open input file" << std::endl;
        std::exit(1);
    }

    auto tokens = lex_stream(file_path, f);

    f.close();

    return tokens;
}

std::string to_string(token_type t) {
    return get_token_bimap().b(t);
}
token_type to_token_type(std::string const &s) {
    return get_token_bimap().a(s);
}
std::string to_string(token const &t) {
    return fmt::format("{}:{}:{} {}: {}", t.file_path, t.line, t.column, to_string(t.type), t.text);
}