/*
 * MIT License
 *
 * Copyright (C) 2024 BoboLabs.net
 * Copyright (C) 2024 Mattia Mignogna (https://stami.bobolabs.net)
 * Copyright (C) 2024 Fabio Nebbia (https://glowy.bobolabs.net)
 * Copyright (C) 2024 Third party contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <malloc.h>

#include "meta.h"
#include "lexer/lexer.h"
#include "lang/binary/module.h"
#include "translator/translator.h"


static void compile(char *pathname) {
    Meta meta = {
            .pathname = pathname
    };

    FILE *file = fopen(pathname, "rb");

    fseek(file, 0L, SEEK_END);
    meta.code_size = ftell(file);
    rewind(file);


    meta.code = malloc(meta.code_size + 1);
    size_t bytes_read = fread(meta.code, sizeof(char), meta.code_size, file);
    meta.code[bytes_read] = '\0';

    fclose(file);

    Lexer *lexer = init_lexer(&meta);
    Token *token;
    do {
        token = scan(lexer);
        TOK_PRINT(token);
    } while (token->tag != EOP);

    return;

    Translator *translator = init_translator(&meta);
    BinaryModule *module = translate(translator);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.bobo>\n", argv[0]);
    } else {
        compile(argv[1]);
    }
}