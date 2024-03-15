#include <stdio.h>

#include "meta.h"
#include "lang/binary/module.h"
#include "translator/translator.h"


static void compile(char *pathname) {
    Meta meta = {
            .pathname = pathname
    };

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