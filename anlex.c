#include "anlex.h"
#include <string.h>
#include <ctype.h>

void imprimir_token(TokenType tipo, FILE *fout) {
    switch (tipo) {
        case L_CORCHETE: fprintf(fout, "L_CORCHETE "); break;
        case R_CORCHETE: fprintf(fout, "R_CORCHETE "); break;
        case L_LLAVE: fprintf(fout, "L_LLAVE "); break;
        case R_LLAVE: fprintf(fout, "R_LLAVE "); break;
        case COMA: fprintf(fout, "COMA "); break;
        case DOS_PUNTOS: fprintf(fout, "DOS_PUNTOS "); break;
        case STRING: fprintf(fout, "STRING "); break;
        case NUMBER: fprintf(fout, "NUMBER "); break;
        case PR_TRUE: fprintf(fout, "PR_TRUE "); break;
        case PR_FALSE: fprintf(fout, "PR_FALSE "); break;
        case PR_NULL: fprintf(fout, "PR_NULL "); break;
        case ERR_LEX: fprintf(fout, "ERR_LEX "); break;
        default: break;
    }
}

const char* token_to_string(TokenType tipo) {
    switch (tipo) {
        case L_CORCHETE: return "L_CORCHETE";
        case R_CORCHETE: return "R_CORCHETE";
        case L_LLAVE: return "L_LLAVE";
        case R_LLAVE: return "R_LLAVE";
        case COMA: return "COMA";
        case DOS_PUNTOS: return "DOS_PUNTOS";
        case STRING: return "STRING";
        case NUMBER: return "NUMBER";
        case PR_TRUE: return "PR_TRUE";
        case PR_FALSE: return "PR_FALSE";
        case PR_NULL: return "PR_NULL";
        case ERR_LEX: return "ERR_LEX";
        case EOF_TOKEN: return "EOF";
        default: return "UNKNOWN";
    }
}

void analizar_linea(const char *linea, FILE *fout, int nro_linea) {
    int i = 0;
    int len = strlen(linea);

    while (i < len) {
        if (isspace(linea[i])) {
            i++;
            continue;
        }
        
        switch (linea[i]) {
            case '[': imprimir_token(L_CORCHETE, fout); i++; continue;
            case ']': imprimir_token(R_CORCHETE, fout); i++; continue;
            case '{': imprimir_token(L_LLAVE, fout); i++; continue;
            case '}': imprimir_token(R_LLAVE, fout); i++; continue;
            case ',': imprimir_token(COMA, fout); i++; continue;
            case ':': imprimir_token(DOS_PUNTOS, fout); i++; continue;
            case '"': {
                int j = i + 1;
                while (j < len && linea[j] != '"') j++;
                if (j < len && linea[j] == '"') {
                    imprimir_token(STRING, fout);
                    i = j + 1;
                } else {
                    fprintf(stderr, "Error léxico en línea %d: cadena sin cerrar\n", nro_linea);
                    imprimir_token(ERR_LEX, fout);
                    i = len;
                }
                continue;
            }
            default: break;
        }
        
        if (isdigit(linea[i]) || (linea[i] == '-' && isdigit(linea[i+1]))) {
            int j = i;
            if (linea[j] == '-') j++;
            while (j < len && isdigit(linea[j])) j++;
            if (j < len && linea[j] == '.') {
                j++;
                while (j < len && isdigit(linea[j])) j++;
            }
            if (j < len && (linea[j] == 'e' || linea[j] == 'E')) {
                j++;
                if (linea[j] == '+' || linea[j] == '-') j++;
                while (j < len && isdigit(linea[j])) j++;
            }
            imprimir_token(NUMBER, fout);
            i = j;
            continue;
        }
        
        if (!strncmp(&linea[i], "true", 4) || !strncmp(&linea[i], "TRUE", 4)) {
            imprimir_token(PR_TRUE, fout);
            i += 4;
            continue;
        }
        if (!strncmp(&linea[i], "false", 5) || !strncmp(&linea[i], "FALSE", 5)) {
            imprimir_token(PR_FALSE, fout);
            i += 5;
            continue;
        }
        if (!strncmp(&linea[i], "null", 4) || !strncmp(&linea[i], "NULL", 4)) {
            imprimir_token(PR_NULL, fout);
            i += 4;
            continue;
        }
        
        fprintf(stderr, "Error léxico en línea %d: carácter inesperado '%c'\n", nro_linea, linea[i]);
        imprimir_token(ERR_LEX, fout);
        i++;
    }
    fprintf(fout, "\n");
}