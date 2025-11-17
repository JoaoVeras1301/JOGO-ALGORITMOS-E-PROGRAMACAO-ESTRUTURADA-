#ifndef QUIZ_H
#define QUIZ_H

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

#define MAX_QUESTOES_NO_BANCO 50 
#define MAX_BUFFER_ARQUIVO 20480
#define VIDAS_INICIAIS 1
#define MAX_VIDAS 3
#define NUM_NIVEIS 3
#define PERGUNTAS_POR_NIVEL 3
#define BOX_WIDTH 110

#endif