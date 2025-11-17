#ifndef UI_H
#define UI_H

#include "quiz.h"

void limpar_tela();
void pausar_tela();
void box_border();
void box_line(const char *text);
void box_printf(const char *fmt, ...);
void box_title(const char *title);
char exibir_menu();
void exibir_instrucoes();
void exibir_interface_jogo(Jogador jogador, Questao questao);
void exibir_feedback(int correto, char resposta_certa, Jogador* jogador);
void exibir_tela_vitoria(Jogador jogador);
void exibir_tela_derrota();

#endif