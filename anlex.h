#ifndef ANLEX_H
#define ANLEX_H

#include <stdio.h>

typedef enum {
    L_CORCHETE,
    R_CORCHETE,
    L_LLAVE,
    R_LLAVE,
    COMA,
    DOS_PUNTOS,
    STRING,
    NUMBER,
    PR_TRUE,
    PR_FALSE,
    PR_NULL,
    ERR_LEX,
    EOF_TOKEN
} TokenType;

typedef struct {
    TokenType type;
    int line;
    int column;     
} Token;

void analizar_linea(const char *linea, FILE *fout, int nro_linea);

void imprimir_token(TokenType tipo, FILE *fout);
const char* token_to_string(TokenType tipo);

#endif