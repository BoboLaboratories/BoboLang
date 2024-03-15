#ifndef BOBO_LANG_COMPILER_TRANSLATOR_H
#define BOBO_LANG_COMPILER_TRANSLATOR_H

#include "lang/binary/module.h"

typedef struct translator Translator;

Translator *init_translator(Meta *meta);
BinaryModule *translate(Translator *translator);

#endif