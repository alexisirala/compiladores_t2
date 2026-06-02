#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función para generar tokens usando tu analizador léxico existente
bool generateTokens(const char* source_file, const char* token_file) {
    FILE *fin = fopen(source_file, "r");
    FILE *fout = fopen(token_file, "w");

    if (!fin || !fout) {
        printf("ERROR: No se pudo abrir archivo %s o %s\n", source_file, token_file);
        if (fin) fclose(fin);
        if (fout) fclose(fout);
        return false;
    }

    char linea[1024];
    int nro_linea = 1;
    
    printf("Ejecutando análisis léxico...\n");
    
    while (fgets(linea, sizeof(linea), fin)) {
        analizar_linea(linea, fout, nro_linea);
        nro_linea++;
    }

    // Al final del archivo, escribir EOF
    fprintf(fout, "EOF\n");

    fclose(fin);
    fclose(fout);
    
    printf("Tokens generados en: %s\n", token_file);
    return true;
}

TokenType stringToTokenType(const char* token_str) {
    if (strcmp(token_str, "L_CORCHETE") == 0) return L_CORCHETE;
    if (strcmp(token_str, "R_CORCHETE") == 0) return R_CORCHETE;
    if (strcmp(token_str, "L_LLAVE") == 0) return L_LLAVE;
    if (strcmp(token_str, "R_LLAVE") == 0) return R_LLAVE;
    if (strcmp(token_str, "COMA") == 0) return COMA;
    if (strcmp(token_str, "DOS_PUNTOS") == 0) return DOS_PUNTOS;
    if (strcmp(token_str, "STRING") == 0) return STRING;
    if (strcmp(token_str, "NUMBER") == 0) return NUMBER;
    if (strcmp(token_str, "PR_TRUE") == 0) return PR_TRUE;
    if (strcmp(token_str, "PR_FALSE") == 0) return PR_FALSE;
    if (strcmp(token_str, "PR_NULL") == 0) return PR_NULL;
    if (strcmp(token_str, "ERR_LEX") == 0) return ERR_LEX;
    if (strcmp(token_str, "EOF") == 0) return EOF_TOKEN;
    return ERR_LEX;
}

Parser* createParser(const char* token_filename) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->token_file = fopen(token_filename, "r");
    if (!parser->token_file) {
        free(parser);
        return NULL;
    }
    
    parser->has_errors = false;
    parser->error_count = 0;
    parser->line_number = 1;
    
    // Leer el primer token
    parser->current_token = readNextToken(parser);
    
    return parser;
}

void destroyParser(Parser* parser) {
    if (parser) {
        if (parser->token_file) {
            fclose(parser->token_file);
        }
        free(parser);
    }
}

Token readNextToken(Parser* parser) {
    Token token;
    char token_str[256];
    
    // Leer el siguiente token del archivo
    if (fscanf(parser->token_file, "%s", token_str) == 1) {
        token.type = stringToTokenType(token_str);
        token.line = parser->line_number;
        token.column = 0;
        
        // Si leemos un token que indica nueva línea en el archivo original
        // (esto es aproximado ya que el formato de salida agrupa tokens por línea)
        if (token.type == EOF_TOKEN) {
            // EOF encontrado
        }
    } else {
        // No se pudo leer más tokens
        token.type = EOF_TOKEN;
        token.line = parser->line_number;
        token.column = 0;
    }
    
    return token;
}

bool match(Parser* parser, TokenType expected) {
    return parser->current_token.type == expected;
}

bool expect(Parser* parser, TokenType expected) {
    if (match(parser, expected)) {
        parser->current_token = readNextToken(parser);
        return true;
    }
    
    char error_msg[256];
    snprintf(error_msg, sizeof(error_msg), 
             "Se esperaba %s pero se encontró %s en línea %d", 
             token_to_string(expected), 
             token_to_string(parser->current_token.type),
             parser->current_token.line);
    reportError(parser, error_msg);
    return false;
}

void reportError(Parser* parser, const char* message) {
    printf("ERROR SINTÁCTICO: %s\n", message);
    parser->has_errors = true;
    parser->error_count++;
}

void panicMode(Parser* parser) {
    // Implementación del Panic Mode
    // Saltamos tokens hasta encontrar un punto de sincronización
    while (!match(parser, EOF_TOKEN)) {
        // Puntos de sincronización: después de comas, llaves, corchetes
        if (match(parser, COMA) || 
            match(parser, R_LLAVE) || 
            match(parser, R_CORCHETE) ||
            match(parser, L_LLAVE) ||
            match(parser, L_CORCHETE)) {
            break;
        }
        parser->current_token = readNextToken(parser);
    }
}

void synchronize(Parser* parser) {
    panicMode(parser);
    // Si encontramos una coma, la consumimos para continuar con el siguiente elemento
    if (match(parser, COMA)) {
        parser->current_token = readNextToken(parser);
    }
}

// json => element eof
bool parseJSON(Parser* parser) {
    printf("Iniciando análisis sintáctico...\n");
    
    bool result = parseElement(parser);
    
    if (result && !match(parser, EOF_TOKEN)) {
        reportError(parser, "Se esperaba fin de archivo");
        result = false;
    }
    
    if (parser->has_errors) {
        printf("\nResultado del análisis:\n");
        printf("ANÁLISIS FALLIDO: Se encontraron %d error(es) sintáctico(s).\n", parser->error_count);
        printf("El archivo JSON NO es sintácticamente válido.\n");
        return false;
    } else {
        printf("\nResultado del análisis:\n");
        printf("ANÁLISIS EXITOSO: El archivo JSON es sintácticamente válido.\n");
        printf("No se encontraron errores sintácticos.\n");
        return true;
    }
}

// element => object | array
bool parseElement(Parser* parser) {
    if (match(parser, L_LLAVE)) {
        return parseObject(parser);
    } else if (match(parser, L_CORCHETE)) {
        return parseArray(parser);
    } else {
        reportError(parser, "Se esperaba un objeto '{' o un array '['");
        panicMode(parser);
        return false;
    }
}

// array => [element-list] | []
bool parseArray(Parser* parser) {
    if (!expect(parser, L_CORCHETE)) {
        return false;
    }
    
    // Verificar si es un array vacío
    if (match(parser, R_CORCHETE)) {
        parser->current_token = readNextToken(parser);
        return true;
    }
    
    // Parsear la lista de elementos
    bool result = parseElementList(parser);
    
    if (result) {
        result = expect(parser, R_CORCHETE);
    } else {
        // Modo pánico: buscar el corchete de cierre
        synchronize(parser);
        if (match(parser, R_CORCHETE)) {
            parser->current_token = readNextToken(parser);
        }
    }
    
    return result;
}

// element-list => element-list , element | element
bool parseElementList(Parser* parser) {
    bool result = parseElement(parser);
    
    // Manejar múltiples elementos separados por comas
    while (match(parser, COMA) && result) {
        parser->current_token = readNextToken(parser); // consumir la coma
        
        if (!parseElement(parser)) {
            result = false;
            synchronize(parser);
        }
    }
    
    return result;
}

// object => {attributes-list} | {}
bool parseObject(Parser* parser) {
    if (!expect(parser, L_LLAVE)) {
        return false;
    }
    
    // Verificar si es un objeto vacío
    if (match(parser, R_LLAVE)) {
        parser->current_token = readNextToken(parser);
        return true;
    }
    
    // Parsear la lista de atributos
    bool result = parseAttributesList(parser);
    
    if (result) {
        result = expect(parser, R_LLAVE);
    } else {
        // Modo pánico: buscar la llave de cierre
        synchronize(parser);
        if (match(parser, R_LLAVE)) {
            parser->current_token = readNextToken(parser);
        }
    }
    
    return result;
}

// attributes-list => attribute-list , attribute | attribute
bool parseAttributesList(Parser* parser) {
    bool result = parseAttribute(parser);
    
    // Manejar múltiples atributos separados por comas
    while (match(parser, COMA) && result) {
        parser->current_token = readNextToken(parser); // consumir la coma
        
        if (!parseAttribute(parser)) {
            result = false;
            synchronize(parser);
        }
    }
    
    return result;
}

// attribute => attribute-name : attribute-value
bool parseAttribute(Parser* parser) {
    bool result = parseAttributeName(parser);
    
    if (result) {
        result = expect(parser, DOS_PUNTOS);
    }
    
    if (result) {
        result = parseAttributeValue(parser);
    }
    
    return result;
}

// attribute-name => string
bool parseAttributeName(Parser* parser) {
    if (match(parser, STRING)) {
        parser->current_token = readNextToken(parser);
        return true;
    } else {
        reportError(parser, "Se esperaba una cadena como nombre de atributo");
        return false;
    }
}

// attribute-value => element | string | number | true | false | null
bool parseAttributeValue(Parser* parser) {
    if (match(parser, L_LLAVE) || match(parser, L_CORCHETE)) {
        return parseElement(parser);
    } else if (match(parser, STRING) || 
               match(parser, NUMBER) ||
               match(parser, PR_TRUE) ||
               match(parser, PR_FALSE) ||
               match(parser, PR_NULL)) {
        parser->current_token = readNextToken(parser);
        return true;
    } else {
        reportError(parser, "Se esperaba un valor válido (objeto, array, string, number, true, false, null)");
        return false;
    }
}