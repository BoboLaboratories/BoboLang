#ifndef BOBO_LANG_COMPILER_LEXER_NUMERIC_LITERAL_H
#define BOBO_LANG_COMPILER_LEXER_NUMERIC_LITERAL_H

typedef enum {
    INTEGER_SIGN,
    INTEGER_PART,
    IMPLICIT_INTEGER_PART,
    INCOMPLETE_DECIMAL,
    DECIMAL_PART,
    EXPONENT_SIGN,
    EXPONENT_PART
} NumericLiteralState;

token *scan_numeric_literal(Lexer *lexer, NumericLiteralState state);

#endif