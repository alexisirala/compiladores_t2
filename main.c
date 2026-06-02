#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Analizador Sintáctico JSON Simplificado\n");
        printf("Uso: %s <archivo_json>\n", argv[0]);
        printf("Ejemplo: %s docs/fuente.txt\n", argv[0]);
        printf("\nEste programa:\n");
        printf("1. Ejecuta análisis léxico\n");
        printf("2. Ejecuta análisis sintáctico\n");
        printf("3. Reporta si el JSON es válido o tiene errores\n");
        return 1;
    }
    
    const char* source_file = argv[1];
    const char* token_file = "tokens_temp.txt";

    printf("Analizador Sintáctico JSON Simplificado\n");
    printf("Archivo fuente: %s\n\n", source_file);

    printf("Paso 1: Análisis Léxico\n");
    if (!generateTokens(source_file, token_file)) {
        printf("Error en el análisis léxico.\n");
        return 1;
    }
    
    printf("\n=== PASO 2: ANÁLISIS SINTÁCTICO ===\n");
    Parser* parser = createParser(token_file);
    if (!parser) {
        printf("Error: No se pudo inicializar el analizador sintáctico.\n");
        return 1;
    }
    
    bool result = parseJSON(parser);
    
    destroyParser(parser);
    remove(token_file);

    printf("\nFin del análisis\n");
    return result ? 0 : 1;
}