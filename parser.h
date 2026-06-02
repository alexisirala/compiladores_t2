#ifndef PARSER_H
#define PARSER_H

#include "anlex.h"
#include <stdbool.h>

typedef struct {
    FILE* token_file;
    Token current_token;
    bool has_errors;
    int error_count;
    int line_number;
} Parser;

Parser* createParser(const char* token_filename);
void destroyParser(Parser* parser);
bool parseJSON(Parser* parser);

bool parseElement(Parser* parser);
bool parseArray(Parser* parser);
bool parseElementList(Parser* parser);
bool parseObject(Parser* parser);
bool parseAttributesList(Parser* parser);
bool parseAttribute(Parser* parser);
bool parseAttributeName(Parser* parser);
bool parseAttributeValue(Parser* parser);

Token readNextToken(Parser* parser);
bool match(Parser* parser, TokenType expected);
bool expect(Parser* parser, TokenType expected);
void reportError(Parser* parser, const char* message);
void panicMode(Parser* parser);
void synchronize(Parser* parser);

bool generateTokens(const char* source_file, const char* token_file);

#endif