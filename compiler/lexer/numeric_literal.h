#ifndef BOBO_LANG_COMPILER_LEXER_NUMERIC_LITERAL_H
#define BOBO_LANG_COMPILER_LEXER_NUMERIC_LITERAL_H

typedef enum {
    INTEGER_SIGN,
    INTEGER,
    IMPLICIT_INTEGER_PART,
    INCOMPLETE_DECIMAL,
    DECIMAL,
    EXPONENT_SIGN,
    EXPONENT,
    ACCEPTED,
    REJECTED
} NumericLiteralState;

token *scan_numeric_literal(Lexer *lexer, NumericLiteralState state);

#endif