#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "ui.h"

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

char exibir_menu() {
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
    
    char opcao;
    scanf(" %c", &opcao);
    return (char)toupper((unsigned char)opcao);
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