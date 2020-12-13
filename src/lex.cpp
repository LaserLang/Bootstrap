#include <iostream>
#include <vector>

#include "lex.hpp"

using namespace laserc;

static char nextc(uint64_t &line, uint32_t &col, std::istream &file) {
    char next;
    file.get(next);
    if (next == '\n') {
        line++;
        col = 0;
    }
    col++; // If newline, sets col to 1
    if (file.eof())
        return 0; // Nobody is going to sensibly put a \0 in their code, and if
                  // they do, it should validly be EOF.
    return next;
}

std::vector<token> *laserc::lex(std::istream &file) {
    auto result = new std::vector<token>();
    uint64_t line = 1; // Why no 0 index? AAAA
    uint32_t col = 1;  // AAAA AAAA

    uint64_t cur_line = line;
    uint32_t cur_col = col;
    char curc = nextc(line, col, file);
    while (curc) {
        if ( // <c>
            curc == '{' || curc == '}' || curc == '(' || curc == ')' ||
            curc == ';' || curc == ',' || curc == '.' || curc == '\'' ||
            curc == '"') {
            std::string data(1, curc);
            token t{cur_line, cur_col, data};
            result->push_back(t);
            cur_line = line;
            cur_col = col;
            curc = nextc(line, col, file);
        } else if ( // <c>=, <c>
            curc == '=' || curc == '*' || curc == '%' || curc == '^' ||
            curc == '!' || curc == '~') {
            std::string tok_str(1, curc);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            curc = nextc(line, col, file);
            if (curc == '=') {
                tok_str.push_back(curc);
                cur_line = line;
                cur_col = col;
                curc = nextc(line, col, file);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str};
            result->push_back(t);
        } else if (curc == '/') { // /=, //, /
            std::string tok_str(1, curc);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            curc = nextc(line, col, file);
            if (curc == '=') {
                tok_str.push_back(curc);
                cur_line = line;
                cur_col = col;
                curc = nextc(line, col, file);
            } else if (curc == '/') {
                tok_str.push_back(curc);
                while (
                    curc !=
                    '\n') { // Nobody needs to know the contents of the comment
                    cur_line = line;
                    cur_col = col;
                    curc = nextc(line, col, file);
                }
            } // I have not decided how to tokenize multilines yet
            token t{tmp_cur_line, tmp_cur_col, tok_str};
            result->push_back(t);
        } else if ( // <c><c>, <c>=, <c>, ->
            curc == '&' || curc == '|' || curc == '+' || curc == '-') {
            char firstc = curc;
            std::string tok_str(1, curc);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            curc = nextc(line, col, file);
            if (curc == '=' || curc == firstc ||
                (firstc == '-' && curc == '>')) {
                tok_str.push_back(curc);
                cur_line = line;
                cur_col = col;
                curc = nextc(line, col, file);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str};
            result->push_back(t);
        } else if (curc == ' ' || curc == '\t' ||
                   curc == '\r' || // If the C++ stl does it's job, this
                                   // shouldn't be a problem.
                   curc == '\n') {
            cur_line = line;
            cur_col = col;
            curc = nextc(line, col, file);
        } else if (curc == '>' || curc == '<') {
            char firstc = curc;
            std::string tok_str(1, curc);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            curc = nextc(line, col, file);
            if (curc == firstc) {
                tok_str.push_back(curc);
                cur_line = line;
                cur_col = col;
                curc = nextc(line, col, file);
            }
            if (curc == '=') {
                tok_str.push_back(curc);
                cur_line = line;
                cur_col = col;
                curc = nextc(line, col, file);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str};
            result->push_back(t);
        } else if (curc >= '0' && curc <= '9') {
            std::string tok_str(1, curc);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            curc = nextc(line, col, file);
            while (curc >= '0' && curc <= '9') {
                tok_str.push_back(curc);
                cur_line = line;
                cur_col = col;
                curc = nextc(line, col, file);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str};
            result->push_back(t);
        } else if ((curc >= 'a' && curc <= 'z') ||
                   (curc >= 'A' && curc <= 'Z') || (curc == '_') ||
                   (curc == '$')) {
            std::string tok_str(1, curc);
            uint64_t tmp_cur_line = cur_line;
            uint32_t tmp_cur_col = cur_col;
            cur_line = line;
            cur_col = col;
            curc = nextc(line, col, file);
            while ((curc >= 'a' && curc <= 'z') ||
                   (curc >= 'A' && curc <= 'Z') ||
                   (curc >= '0' && curc <= '9') || (curc == '_') ||
                   (curc == '$')) {
                tok_str.push_back(curc);
                cur_line = line;
                cur_col = col;
                curc = nextc(line, col, file);
            }
            token t{tmp_cur_line, tmp_cur_col, tok_str};
            result->push_back(t);
        } else {
            std::cerr << "FATAL: Invalid character '" << curc << "'!";
            throw "Invalid character"; // FIXME: Don't use exceptions
        }
    }

    return result;
}
