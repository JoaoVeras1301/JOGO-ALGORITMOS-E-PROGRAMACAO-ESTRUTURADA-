#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

static char* pular_espacos(char* cursor) {
    while (*cursor != '\0' && isspace((unsigned char)*cursor)) {
        cursor++;
    }
    return cursor;
}

static char* parse_string(char* cursor, char* destino, int tam_max) {
    if (*cursor != '"') {
        return cursor;
    }
    cursor++;

    int i = 0;
    while (*cursor != '\0' && *cursor != '"' && i < (tam_max - 1)) {
        if (*cursor == '\\') {
            cursor++;
            switch (*cursor) {
                case 'n':  destino[i] = '\n'; break;
                case 't':  destino[i] = '\t'; break;
                case '"':  destino[i] = '"';  break;
                case '\\': destino[i] = '\\'; break;
                default:   destino[i] = *cursor; break;
            }
        } else {
            destino[i] = *cursor;
        }
        i++;
        cursor++;
    }
    destino[i] = '\0'; 

    if (*cursor == '"') {
        cursor++;
    }
    return cursor;
}

static char* parse_number(char* cursor, int* destino) {
    char num_buffer[20];
    int i = 0;
    if (*cursor == '-') {
        num_buffer[i++] = *cursor;
        cursor++;
    }
    while (*cursor != '\0' && isdigit((unsigned char)*cursor) && i < 19) {
        num_buffer[i++] = *cursor;
        cursor++;
    }
    num_buffer[i] = '\0';
    *destino = atoi(num_buffer);
    return cursor;
}

static char* parse_array_alternativas(char* cursor, Questao* q) {
    if (*cursor != '[') {
        return cursor;
    }
    cursor++;

    int i = 0;
    while (*cursor != '\0' && *cursor != ']' && i < 4) {
        cursor = pular_espacos(cursor);
        if (*cursor == '"') {
            cursor = parse_string(cursor, q->alternativas[i], 100); 
            i++;
        }
        cursor = pular_espacos(cursor);
        if (*cursor == ',') {
            cursor++;
        } else if (*cursor == ']') {
            break;
        }
    }
    if (*cursor == ']') {
        cursor++;
    }
    return cursor;
}

static char* parse_object(char* cursor, Questao* q) {
    if (*cursor != '{') {
        return cursor;
    }
    cursor++;

    char chave[100];
    char resposta_str[10];

    while (*cursor != '\0' && *cursor != '}') {
        cursor = pular_espacos(cursor);
        if (*cursor != '"') {
            break; 
        }
        cursor = parse_string(cursor, chave, 100);
        cursor = pular_espacos(cursor);
        if (*cursor != ':') {
            break;
        }
        cursor++;
        cursor = pular_espacos(cursor);
        
        if (strcmp(chave, "Enunciado") == 0) {
            cursor = parse_string(cursor, q->pergunta, 256);
        } 
        else if (strcmp(chave, "Resposta correta") == 0) {
            cursor = parse_string(cursor, resposta_str, 10);
            q->resposta_correta = resposta_str[0];
        } 
        else if (strcmp(chave, "Dica") == 0) {
            cursor = parse_string(cursor, q->dica, 128);
        } 
        else if (strcmp(chave, "Nivel") == 0) {
            cursor = parse_number(cursor, &q->nivel);
        } 
        else if (strcmp(chave, "Alternativas") == 0) {
            cursor = parse_array_alternativas(cursor, q);
        } 
        else {
             if (*cursor == '"') { 
                 cursor = parse_string(cursor, resposta_str, 10); 
             }
             else if (isdigit((unsigned char)*cursor) || *cursor == '-') { 
                 int temp_int;
                 cursor = parse_number(cursor, &temp_int);
             }
             else if (*cursor == '[') { 
                 int array_level = 0;
                 do {
                     if (*cursor == '[') array_level++;
                     else if (*cursor == ']') array_level--;
                     if (*cursor != '\0') cursor++;
                 } while (array_level > 0 && *cursor != '\0');
             }
             else if (*cursor == '{') { 
                int obj_level = 0;
                 do {
                     if (*cursor == '{') obj_level++;
                     else if (*cursor == '}') obj_level--;
                     if (*cursor != '\0') cursor++;
                 } while (obj_level > 0 && *cursor != '\0');
             }
        }

        cursor = pular_espacos(cursor);
        if (*cursor == ',') {
            cursor++;
        } else if (*cursor == '}') {
            break; 
        }
    }
    if (*cursor == '}') {
        cursor++;
    }
    return cursor;
}

int carregar_banco_de_questoes(Questao banco[], int max_questoes) {
    FILE *f = fopen("perguntas.json", "rb");
    if (f == NULL) {
        return 0;
    }
    
    char* buffer = (char*)malloc(MAX_BUFFER_ARQUIVO);
    if (buffer == NULL) {
        fclose(f);
        return 0;
    }

    size_t tamanho_lido = fread(buffer, 1, MAX_BUFFER_ARQUIVO - 1, f);
    buffer[tamanho_lido] = '\0';
    fclose(f);
    
    int num_questoes = 0;
    char* cursor = buffer;
    
    cursor = pular_espacos(cursor);
    if (*cursor != '[') {
        free(buffer);
        return 0; 
    }
    cursor++;

    while (*cursor != '\0' && *cursor != ']' && num_questoes < max_questoes) {
        cursor = pular_espacos(cursor);
        
        if (*cursor == '{') {
            cursor = parse_object(cursor, &banco[num_questoes]);
            num_questoes++;
            
            cursor = pular_espacos(cursor);
            if (*cursor == ',') {
                cursor++;
            } else if (*cursor == ']') {
                break;
            }
        } else {
             if (*cursor != ']') break;
        }
    }

    free(buffer);
    return num_questoes;
}