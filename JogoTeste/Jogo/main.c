#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "quiz.h"
#include "ui.h"
#include "parser.h"
#include "utils.h"

static void inicializar_jogador(Jogador *jogador) {
    jogador->vidas = VIDAS_INICIAIS; 
    jogador->pontuacao = 0;
    jogador->nivel_atual = 1;
    jogador->usou_pular = 0;
    jogador->usou_trocar = 0;
    jogador->usou_dica = 0;
}

static void executar_loop_principal(Questao banco[], int num_questoes_carregadas) {
    Jogador jogador;
    inicializar_jogador(&jogador);

    while (jogador.vidas > 0 && jogador.nivel_atual <= NUM_NIVEIS) {
        
        int indices_pool_nivel[MAX_QUESTOES_NO_BANCO]; 
        int num_questoes_neste_nivel = 0;

        for (int i = 0; i < num_questoes_carregadas; i++) {
            if (banco[i].nivel == jogador.nivel_atual) {
                indices_pool_nivel[num_questoes_neste_nivel] = i;
                num_questoes_neste_nivel++;
            }
        }

        if (num_questoes_neste_nivel == 0) {
            jogador.nivel_atual++; 
            continue; 
        }

        shuffle_indices(indices_pool_nivel, num_questoes_neste_nivel);

        int perguntas_a_responder = PERGUNTAS_POR_NIVEL;
        if (perguntas_a_responder > num_questoes_neste_nivel) {
            perguntas_a_responder = num_questoes_neste_nivel; 
        }

        int proximo_indice_troca = perguntas_a_responder;
        int perguntas_respondidas_no_nivel = 0;
        
        int acertos_neste_nivel = 0;

        while (perguntas_respondidas_no_nivel < perguntas_a_responder && jogador.vidas > 0) {
            
            int indice_da_questao = indices_pool_nivel[perguntas_respondidas_no_nivel];
            Questao questao_atual = banco[indice_da_questao];
            
            int pergunta_foi_respondida_ou_pulada = 0;

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
                            acertos_neste_nivel++;
                        } else {
                            jogador.vidas-=1;
                            exibir_feedback(0, questao_atual.resposta_correta, &jogador);
                        }
                        pergunta_foi_respondida_ou_pulada = 1;
                        break;
                    
                    case 'P': 
                        if (jogador.usou_pular == 0) {
                            jogador.usou_pular = 1;
                            
                            jogador.pontuacao += 10;
                            acertos_neste_nivel++;

                            if (jogador.vidas >= MAX_VIDAS){ 
                                printf("Voce ja esta com o maximo de vidas!\n");
                            } else {
                            jogador.vidas+=1; 
                            }
                            
                            exibir_feedback(1, questao_atual.resposta_correta, &jogador);

                            pergunta_foi_respondida_ou_pulada = 1; 
                        } else {
                            printf("\n> Voce ja usou 'Pular'!\n");
                            pausar_tela();
                        }
                        break;
                    
                    case 'T':
                        if (jogador.usou_trocar == 0) {
                            if (proximo_indice_troca < num_questoes_neste_nivel) {
                                jogador.usou_trocar = 1;
                                int indice_nova_questao = indices_pool_nivel[proximo_indice_troca];
                                questao_atual = banco[indice_nova_questao];
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
                    
                    case 'H':
                        if (jogador.usou_dica == 0) {
                            jogador.usou_dica = 1;
                            printf("\n> DICA: %s\n", questao_atual.dica);
                            pausar_tela();
                        } else {
                            printf("\n> Voce ja usou 'Help'!\n");
                            pausar_tela();
                        }
                        break;
                    
                    case 'S':
                        limpar_tela();
                        printf("\nObrigado por jogar! Ate a proxima.\n");
                        exit(0);
                   
                    default:
                        printf("\n> Opcao invalida! Tente novamente.\n");
                        pausar_tela();
                        break;
                }
            } 
            perguntas_respondidas_no_nivel++; 
        } 

        if (jogador.vidas > 0) {
            if (acertos_neste_nivel >= PERGUNTAS_POR_NIVEL) {
                jogador.nivel_atual++; 
                if (jogador.nivel_atual <= NUM_NIVEIS) {
                     printf("\n> PARABENS! Voce acertou %d/%d e avancou para o Nivel %d!\n", acertos_neste_nivel, PERGUNTAS_POR_NIVEL, jogador.nivel_atual);
                     pausar_tela();
                     jogador.usou_pular = 0;
                     jogador.usou_trocar = 0;
                     jogador.usou_dica = 0;
                }
            } else {
                printf("\n> Voce acertou %d/%d. E preciso acertar %d para avancar.\n", acertos_neste_nivel, PERGUNTAS_POR_NIVEL, PERGUNTAS_POR_NIVEL);
                printf("> Repetindo o Nivel %d. Pressione Enter...\n", jogador.nivel_atual);
                pausar_tela();
            }
        }
    } 

    if (jogador.vidas > 0) exibir_tela_vitoria(jogador);
    else exibir_tela_derrota();
}

static void iniciar_jogo() {
    Questao banco_de_questoes[MAX_QUESTOES_NO_BANCO];
    int num_questoes_carregadas = carregar_banco_de_questoes(banco_de_questoes, MAX_QUESTOES_NO_BANCO);

    if (num_questoes_carregadas == 0) {
        printf("Erro fatal: Nao foi possivel carregar 'perguntas.json'.\n");
        printf("Verifique se o arquivo esta na mesma pasta do executavel.\n");
        pausar_tela();
        return;
    }

    char opcao_menu;
    do {
        opcao_menu = exibir_menu();

        switch (opcao_menu) {
            case '1':
                executar_loop_principal(banco_de_questoes, num_questoes_carregadas);
                break;
            case '2': 
                exibir_instrucoes(); 
                break;
            case '3': 
                printf("\nObrigado por jogar! Ate a proxima.\n"); 
                break;
            default:
                printf("\nOpcao invalida! Pressione Enter para tentar novamente.");
                pausar_tela();
                break;
        }
    } while (opcao_menu != '3');
}

int main() {
    srand(time(0)); 

    iniciar_jogo();

    return 0;
}