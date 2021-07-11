#include <iostream>
#include <vector>

#include "lex.hpp"

namespace cannon {

static char next_char(uint64_t &line, uint32_t &col, std::istream &input) {
    char next;
    input.get(next);
    if (next == '\n') {
        line++;
        col = 0;
    }
    col++; // If newline, sets col to 1
    if (input.eof())
        return '\0'; // Nobody is going to sensibly put a \0 in their code, and if
                     // they do, it should validly be EOF.
    return next;
}

std::vector<token> lex(std::istream &input) {
    auto result = std::vector<token>();
    uint64_t line = 1; // Why no 0 index? AAAA
    uint32_t col = 1;  // AAAA AAAA

    uint64_t cur_line = line;
    uint32_t cur_col = col;
    char c = next_char(line, col, input); // the current char
    while (c != '\0') {
        if ( // <c>
            c == '{' || c == '}' || c == '(' || c == ')' ||
            c == ';' || c == ',' || c == '.' || c == '\'' || c == '"'
        ) {
            std::string data(1, c);
            token t{cur_line, cur_col, data, SYMBOL};
            result.push_back(t);
            cur_line = line;
            cur_col = col;
            c = next_char(line, col, input);
        } else if ( // <c>=, <c>
            c == '=' || c == '*' || c == '%' || c == '^' || c == '!' || c == '~'
        ) {
            std::string tok_str(1, c);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            c = next_char(line, col, input);
            if (c == '=') {
                tok_str.push_back(c);
                cur_line = line;
                cur_col = col;
                c = next_char(line, col, input);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str, SYMBOL};
            result.push_back(t);
        } else if (c == '/') { // /=, //, /
            std::string tok_str(1, c);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            c = next_char(line, col, input);
            if (c == '=') {
                tok_str.push_back(c);
                cur_line = line;
                cur_col = col;
                c = next_char(line, col, input);
            } else if (c == '/') {
                while (
                    c !=
                    '\n') { // Nobody needs to know the contents of the comment
                    cur_line = line;
                    cur_col = col;
                    c = next_char(line, col, input);
                }
                continue; // We don't need to store that there was a comment; that's not helpful.
            } // I have not decided how to tokenize multilines yet
            token t{tmp_cur_line, tmp_cur_col, tok_str, SYMBOL};
            result.push_back(t);
        } else if ( // <c><c>, <c>=, <c>, ->
            c == '&' || c == '|' || c == '+' || c == '-'
        ) {
            char firstc = c;
            std::string tok_str(1, c);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            c = next_char(line, col, input);
            if (c == '=' || c == firstc ||
                (firstc == '-' && c == '>')) {
                tok_str.push_back(c);
                cur_line = line;
                cur_col = col;
                c = next_char(line, col, input);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str, SYMBOL};
            result.push_back(t);
        } else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            cur_line = line;
            cur_col = col;
            c = next_char(line, col, input);
        } else if (c == '>' || c == '<') {
            char firstc = c;
            std::string tok_str(1, c);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            c = next_char(line, col, input);
            if (c == firstc) {
                tok_str.push_back(c);
                cur_line = line;
                cur_col = col;
                c = next_char(line, col, input);
            }
            if (c == '=') {
                tok_str.push_back(c);
                cur_line = line;
                cur_col = col;
                c = next_char(line, col, input);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str, SYMBOL};
            result.push_back(t);
        } else if (c >= '0' && c <= '9') {
            std::string tok_str(1, c);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            c = next_char(line, col, input);
            while (c >= '0' && c <= '9') {
                tok_str.push_back(c);
                cur_line = line;
                cur_col = col;
                c = next_char(line, col, input);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str, NUMBER};
            result.push_back(t);
        } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$') {
            std::string tok_str(1, c);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            c = next_char(line, col, input);
            while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '$') {
                tok_str.push_back(c);
                cur_line = line;
                cur_col = col;
                c = next_char(line, col, input);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str, IDENTIFIER};
            result.push_back(t);
        } else {
            std::cerr << "FATAL: Invalid character '" << c << "'!";
            throw "Invalid character"; // FIXME: Don't use exceptions
        }
    }

    return result;
}

} // namespace cannon
