#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

// --- ESTRUTURAS DE DADOS ---

typedef struct {
    char pergunta[256];
    char alternativas[4][100];
    char resposta_correta;
    char dica[128];
    int nivel;
} Questao;

typedef struct {
    int vidas;
    int pontuacao;
    int nivel_atual;
    int usou_pular;
    int usou_trocar;
    int usou_dica;
} Jogador;

// --- CONSTANTES ---
#define MAX_QUESTOES_NO_BANCO 50 
#define MAX_BUFFER_ARQUIVO 20480 // 20KB (para ler o JSON)
#define VIDAS_INICIAIS 1
#define MAX_VIDAS 3
#define NUM_NIVEIS 3
#define PERGUNTAS_POR_NIVEL 3
#define BOX_WIDTH 110

// --- PROTOTIPOS ---
void limpar_tela();
void pausar_tela();
void exibir_menu();
void exibir_instrucoes();
void exibir_interface_jogo(Jogador jogador, Questao questao);
void exibir_feedback(int correto, char resposta_certa, Jogador* jogador);
void exibir_tela_vitoria(Jogador jogador);
void exibir_tela_derrota();
void inicializar_jogador(Jogador *jogador);

int carregar_banco_de_questoes(Questao banco[], int max_questoes);
void box_border();
void box_line(const char *text);
void box_printf(const char *fmt, ...);
void box_title(const char *title);
void shuffle_indices(int *array, int n);

// --- MAIN ---
int main() {
    // Semente para aleatoriedade - mantido como solicitado
    srand(time(0)); 

    Questao banco_de_questoes[MAX_QUESTOES_NO_BANCO];
    int num_questoes_carregadas = carregar_banco_de_questoes(banco_de_questoes, MAX_QUESTOES_NO_BANCO);

    if (num_questoes_carregadas == 0) {
        printf("Erro fatal: Nao foi possivel carregar 'perguntas.json'.\n");
        printf("Verifique se o arquivo esta na mesma pasta do executavel.\n");
        pausar_tela();
        return 1;
    }

    char opcao_menu;
    do {
        exibir_menu();
        scanf(" %c", &opcao_menu);

        switch (toupper((unsigned char)opcao_menu)) {
            case '1': {
                Jogador jogador;
                inicializar_jogador(&jogador);

                // --- LOGICA DE NIVEL (LOOP PRINCIPAL DO JOGO) ---
                while (jogador.vidas > 0 && jogador.nivel_atual <= NUM_NIVEIS) {
                    
                    // --- 1. PREPARAR O POOL DE PERGUNTAS DO NIVEL ---
                    int indices_pool_nivel[MAX_QUESTOES_NO_BANCO]; 
                    int num_questoes_neste_nivel = 0;

                    for (int i = 0; i < num_questoes_carregadas; i++) {
                        if (banco_de_questoes[i].nivel == jogador.nivel_atual) {
                            indices_pool_nivel[num_questoes_neste_nivel] = i;
                            num_questoes_neste_nivel++;
                        }
                    }

                    if (num_questoes_neste_nivel == 0) {
                        jogador.nivel_atual++; 
                        continue; 
                    }

                    // --- 2. EMBARALHAR O POOL ---
                    shuffle_indices(indices_pool_nivel, num_questoes_neste_nivel);

                    // --- 3. DEFINIR QUANTAS PERGUNTAS RESPONDER ---
                    int perguntas_a_responder = PERGUNTAS_POR_NIVEL;
                    if (perguntas_a_responder > num_questoes_neste_nivel) {
                        perguntas_a_responder = num_questoes_neste_nivel; 
                    }

                    int proximo_indice_troca = perguntas_a_responder;
                    int perguntas_respondidas_no_nivel = 0;

                    // --- LOOP DAS 3 PERGUNTAS DO NIVEL ---
                    while (perguntas_respondidas_no_nivel < perguntas_a_responder && jogador.vidas > 0) {
                        
                        int indice_da_questao = indices_pool_nivel[perguntas_respondidas_no_nivel];
                        Questao questao_atual = banco_de_questoes[indice_da_questao];
                        
                        int pergunta_foi_respondida_ou_pulada = 0;

                        // --- LOOP DE UMA UNICA PERGUNTA (espera A,B,C,D,P,T,H) ---
                        while (!pergunta_foi_respondida_ou_pulada && jogador.vidas > 0) {
                            
                            exibir_interface_jogo(jogador, questao_atual);
                            char entrada_jogador;
                            scanf(" %c", &entrada_jogador);
                            entrada_jogador = (char)toupper((unsigned char)entrada_jogador);

                            switch (entrada_jogador) {
                                case 'A': case 'B': case 'C': case 'D':
                                    if (entrada_jogador == questao_atual.resposta_correta) {
                                        jogador.pontuacao += 10;
                                        if (jogador.vidas >= MAX_VIDAS){ 
                                            printf("Voce ja esta com o maximo de vidas!\n");
                                        } else {
                                           jogador.vidas+=1; 
                                        }
                                        exibir_feedback(1, questao_atual.resposta_correta, &jogador);
                                    } else {
                                        jogador.vidas-=1;
                                        exibir_feedback(0, questao_atual.resposta_correta, &jogador);
                                    }
                                    pergunta_foi_respondida_ou_pulada = 1;
                                    break;
                                
                                case 'P': // PULAR
                                    if (jogador.usou_pular == 0) {
                                        jogador.usou_pular = 1;
                                        printf("\n> Voce pulou a pergunta! (Sem pontos/vida). Pressione Enter...");
                                        pausar_tela();
                                        pergunta_foi_respondida_ou_pulada = 1; 
                                    } else {
                                        printf("\n> Voce ja usou 'Pular'!\n");
                                        pausar_tela();
                                    }
                                    break;
                                
                                case 'T': // TROCAR
                                    if (jogador.usou_trocar == 0) {
                                        if (proximo_indice_troca < num_questoes_neste_nivel) {
                                            jogador.usou_trocar = 1;
                                            int indice_nova_questao = indices_pool_nivel[proximo_indice_troca];
                                            questao_atual = banco_de_questoes[indice_nova_questao];
                                            proximo_indice_troca++; 
                                            
                                            printf("\n> Pergunta trocada! Pressione Enter para continuar...");
                                            pausar_tela();
                                        } else {
                                            printf("\n> Nao ha mais perguntas para trocar neste nivel!\n");
                                            pausar_tela();
                                        }
                                    } else {
                                        printf("\n> Voce ja usou 'Trocar'!\n");
                                        pausar_tela();
                                    }
                                    break;
                                
                                case 'H': // DICA
                                    if (jogador.usou_dica == 0) {
                                        jogador.usou_dica = 1;
                                        printf("\n> DICA: %s\n", questao_atual.dica);
                                        pausar_tela();
                                    } else {
                                        printf("\n> Voce ja usou 'Help'!\n");
                                        pausar_tela();
                                    }
                                    break;
                                
                                case 'S': // SAIR
                                    limpar_tela();
                                    printf("\nObrigado por jogar! Ate a proxima.\n");
                                    exit(0);
                               
                                default:
                                    printf("\n> Opcao invalida! Tente novamente.\n");
                                    pausar_tela();
                                    break;
                            }
                        } // Fim do loop de uma unica pergunta

                        perguntas_respondidas_no_nivel++; 

                    } // Fim do loop DAS 3 PERGUNTAS

                    // --- FIM DO NIVEL ---
                    if (jogador.vidas > 0) {
                        jogador.nivel_atual++; 
                        if (jogador.nivel_atual <= NUM_NIVEIS) {
                             printf("\n> PARABENS! Voce avancou para o Nivel %d!\n", jogador.nivel_atual);
                             pausar_tela();
                             jogador.usou_pular = 0;
                             jogador.usou_trocar = 0;
                             jogador.usou_dica = 0;
                        }
                    }
                } // Fim do while (jogo principal)

                if (jogador.vidas > 0) exibir_tela_vitoria(jogador);
                else exibir_tela_derrota();
                break;
            }
            case '2': exibir_instrucoes(); break;
            case '3': printf("\nObrigado por jogar! Ate a proxima.\n"); break;
            default:
                printf("\nOpcao invalida! Pressione Enter para tentar novamente.");
                pausar_tela();
                break;
        }
    } while (opcao_menu != '3');

    return 0;
}

// --- FUNCOES DE INTERFACE ---
void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
    
void pausar_tela() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    getchar();
}

void box_border() {
    putchar('/');
    for (int i = 0; i < BOX_WIDTH; ++i) putchar('=');
    puts("\\");
}

void box_line(const char *text) {
    int len = (int)strlen(text);
    if (len > BOX_WIDTH - 2) len = BOX_WIDTH - 2;
    printf("| %-*.*s |\n", BOX_WIDTH - 2, len, text);
}

void box_printf(const char *fmt, ...) {
    char buffer[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);
    box_line(buffer);
}

void box_title(const char *title) {
    int label_len = (int)strlen(title) + 4;
    int total_eq = BOX_WIDTH - label_len;
    int left = total_eq / 2;
    int right = total_eq - left;

    putchar('/');
    for (int i = 0; i < left; ++i) putchar('=');
    printf("[ %s ]", title);
    for (int i = 0; i < right; ++i) putchar('=');
    puts("\\");
}

// --- TELAS ---
void exibir_menu() {
    limpar_tela();
    box_title("MENU");
    box_line("");
    box_line("BEM VINDO AO JOGO DE QUIZ!");
    box_line("");
    box_line(" [1] Iniciar Jogo");
    box_line(" [2] Instrucoes");
    box_line(" [3] Sair");
    box_line("");
    box_line(" Escolha uma opcao:");
    box_border();
    printf("> ");
}

void exibir_instrucoes() {
    limpar_tela();
    box_title("INSTRUCOES");
    box_line("");
    box_printf("- O jogo tem %d niveis (Facil, Medio e Dificil).", NUM_NIVEIS);
    box_printf("- Voce deve acertar %d perguntas para avancar de nivel.", PERGUNTAS_POR_NIVEL);
    box_printf("- Voce comeca com %d vida(s) e pode acumular ate %d.", VIDAS_INICIAIS, MAX_VIDAS);
    box_line("- Ajudas (uma vez POR NIVEL): [P] Pular  [T] Trocar  [H] Dica");
    box_line("- [S] Sair (a qualquer momento)");
    box_line("");
    box_line("Pressione Enter para voltar ao menu...");
    box_border();
    pausar_tela();
}

void exibir_interface_jogo(Jogador jogador, Questao questao) {
    limpar_tela();
    box_title("NIVEL ATUAL");
    box_printf("Nivel: %d", jogador.nivel_atual);
    box_printf("Vidas: %d/%d", jogador.vidas, MAX_VIDAS);
    box_printf("Pontuacao: %d", jogador.pontuacao);
    char ajudas[128];
    snprintf(ajudas, sizeof(ajudas),
             "[P] Pular%s  [T] Trocar%s  [H] Dica%s  [S] Sair",
             jogador.usou_pular ? " (usado)" : "",
             jogador.usou_trocar ? " (usado)" : "",
             jogador.usou_dica ? " (usado)" : "");
    box_line(ajudas);
    box_border();

    box_title("PERGUNTA");
    
    char* pergunta_formatada = strdup(questao.pergunta); 
    char* token = strtok(pergunta_formatada, "\n"); 
    
    while (token != NULL) {
        box_line(token);
        token = strtok(NULL, "\n"); 
    }
    free(pergunta_formatada); 
    
    box_printf("A) %s", questao.alternativas[0]);
    box_printf("B) %s", questao.alternativas[1]);
    box_printf("C) %s", questao.alternativas[2]);
    box_printf("D) %s", questao.alternativas[3]);
    box_border();

    box_title("SUA VEZ");
    box_line("Digite sua resposta (A, B, C, D) ou uma ajuda (P, T, H):");
    box_border();
    printf("> ");
}

void exibir_feedback(int correto, char resposta_certa, Jogador* jogador) {
    if (correto)
        printf("\n> RESPOSTA CORRETA! +10 pontos.\n");
    else
        printf("\n> RESPOSTA INCORRETA! Resposta certa: %c. (Vidas: %d)\n", resposta_certa, jogador->vidas);
    printf("Pressione Enter para continuar...");
    pausar_tela();
}

void exibir_tela_vitoria(Jogador jogador) {
    limpar_tela();
    box_title("VITORIA");
    box_line("Parabens! Voce completou todos os niveis!");
    box_printf("Pontuacao final: %d pontos", jogador.pontuacao);
    box_line("Pressione Enter para voltar ao menu...");
    box_border();
    pausar_tela();
}

void exibir_tela_derrota() {
    limpar_tela();
    box_title("GAME OVER");
    box_line("Voce ficou sem vidas!");
    box_line("Tente novamente!");
    box_line("Pressione Enter para voltar ao menu...");
    box_border();
    pausar_tela();
}

void inicializar_jogador(Jogador *jogador) {
    jogador->vidas = VIDAS_INICIAIS; 
    jogador->pontuacao = 0;
    jogador->nivel_atual = 1;
    jogador->usou_pular = 0;
    jogador->usou_trocar = 0;
    jogador->usou_dica = 0;
}

// --- FUNCOES DO PARSER JSON MANUAL ---

char* pular_espacos(char* cursor) {
    while (*cursor != '\0' && isspace((unsigned char)*cursor)) {
        cursor++;
    }
    return cursor;
}

char* parse_string(char* cursor, char* destino, int tam_max) {
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

char* parse_number(char* cursor, int* destino) {
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

char* parse_array_alternativas(char* cursor, Questao* q) {
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

char* parse_object(char* cursor, Questao* q) {
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


// --- BANCO DE QUESTOES ---
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


/**
 * @brief Embaralha um array de inteiros.
 * --- CORRECAO AQUI ---
 * Algoritmo de Fisher-Yates (versao moderna, com modulo).
 * Esta versao e mais robusta que a anterior.
 */
void shuffle_indices(int *array, int n) {
    if (n > 1) {
        // Itera do ultimo elemento (n-1) ate o segundo (1)
        for (int i = n - 1; i > 0; i--) {
            // Escolhe um indice aleatorio 'j' entre 0 e i (inclusive)
            int j = rand() % (i + 1);
            
            // Troca array[i] e array[j]
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }
}