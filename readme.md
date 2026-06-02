Tarea 2 de compiladores

# Analizador Sintáctico JSON

Este proyecto implementa un analizador sintáctico descendente recursivo en C para JSON simplificado, como continuación de la Tarea 1.

## Uso

Compilar con:
```sh
gcc main.c anlex.c parser.c -o json_parser
```

Ejecutar con:
```sh
./json_parser docs/fuente.txt
```

## Integrante

- Alexis Irala

## Descripción

- Integra el analizador léxico de la Tarea 1
- Implementa análisis sintáctico descendente recursivo siguiendo gramática BNF
- Reporta errores sintácticos con manejo Panic Mode
- Lee archivo JSON y determina si es sintácticamente válido
