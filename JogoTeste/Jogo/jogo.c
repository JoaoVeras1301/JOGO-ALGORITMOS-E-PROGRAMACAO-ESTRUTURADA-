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
#define NUM_TOTAL_QUESTOES 16
#define VIDAS_INICIAIS 3
#define NUM_NIVEIS 3

#define BOX_WIDTH 110

// --- PROTÓTIPOS ---
void limpar_tela();
void pausar_tela();
void exibir_menu();
void exibir_instrucoes();
void exibir_interface_jogo(Jogador jogador, Questao questao);
void exibir_feedback(int correto, char resposta_certa);
void exibir_tela_vitoria(Jogador jogador);
void exibir_tela_derrota();
void inicializar_jogador(Jogador *jogador);
void carregar_banco_de_questoes(Questao banco[]);
Questao obter_questao_por_nivel(Questao banco[], int nivel, int indice_questao_nivel);
Questao obter_nova_questao_aleatoria(Questao banco[], int nivel_atual, Questao questao_antiga);
void box_border();
void box_line(const char *text);
void box_printf(const char *fmt, ...);
void box_title(const char *title);

// --- MAIN ---
int main() {
    srand((unsigned)time(NULL));

    Questao banco_de_questoes[NUM_TOTAL_QUESTOES];
    carregar_banco_de_questoes(banco_de_questoes);

    char opcao_menu;
    do {
        exibir_menu();
        scanf(" %c", &opcao_menu);

        switch (toupper((unsigned char)opcao_menu)) {
            case '1': {
                Jogador jogador;
                inicializar_jogador(&jogador);
                int indice_questao_nivel = 0;

                while (jogador.vidas > 0 && jogador.nivel_atual <= NUM_NIVEIS) {
                    Questao questao_atual = obter_questao_por_nivel(banco_de_questoes, jogador.nivel_atual, indice_questao_nivel);
                    int pergunta_respondida = 0;

                    while (!pergunta_respondida) {
                        exibir_interface_jogo(jogador, questao_atual);
                        char entrada_jogador;
                        scanf(" %c", &entrada_jogador);
                        entrada_jogador = (char)toupper((unsigned char)entrada_jogador);

                        switch (entrada_jogador) {
                            case 'A': case 'B': case 'C': case 'D':
                                if (entrada_jogador == questao_atual.resposta_correta) {
                                    jogador.pontuacao += 10;
                                    if (jogador.vidas >= 3){
                                        printf("Voce Alcançou o limite de vidas!!/n");
                                    } else {
                                       jogador.vidas+=1; 
                                    }
                                    exibir_feedback(1, questao_atual.resposta_correta);
                                } else {
                                    jogador.vidas-=1;
                                    exibir_feedback(0, questao_atual.resposta_correta);
                                }
                                pergunta_respondida = 1;
                                break;
                            case 'P':
                                if (jogador.usou_pular == 0) {
                                    jogador.usou_pular = 1;
                                    printf("\n> Voce pulou a pergunta! Pressione Enter para continuar...");
                                    pausar_tela();
                                    pergunta_respondida = 1;
                                } else {
                                    printf("\n> Voce ja usou 'Pular'!\n");
                                    pausar_tela();
                                }
                                break;
                            case 'T':
                                if (jogador.usou_trocar == 0) {
                                    jogador.usou_trocar = 1;
                                    questao_atual = obter_nova_questao_aleatoria(banco_de_questoes, jogador.nivel_atual, questao_atual);
                                    printf("\n> Pergunta trocada! Pressione Enter para continuar...");
                                    pausar_tela();
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
                                system("cls");
                                printf("\nObrigado por jogar! Ate a proxima.\n");
                                exit(0);
                           
                            default:
                                printf("\n> Opcao invalida! Tente novamente.\n");
                                pausar_tela();
                                break;
                        }
                    }

                    indice_questao_nivel++;
                    if (indice_questao_nivel >= (NUM_TOTAL_QUESTOES / NUM_NIVEIS)) {
                        jogador.nivel_atual++;
                        indice_questao_nivel = 0;
                    }
                }

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

// --- FUNÇÕES DE INTERFACE ---

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
    box_line("BEM VINDO AO JOGO DE QUIZ!"); // vai tomar no cu
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
    box_printf("- Voce comeca com %d vidas.", VIDAS_INICIAIS);
    box_line("- Ajudas (uma vez cada): [P] Pular  [T] Trocar  [H] Dica  [S] Sair" );
    box_line("");
    box_line("Pressione Enter para voltar ao menu...");
    box_border();
    pausar_tela();
}

void exibir_interface_jogo(Jogador jogador, Questao questao) {
    limpar_tela();
    box_title("NIVEL ATUAL");
    box_printf("Nivel: %d", jogador.nivel_atual);
    box_printf("Vidas: %d/%d", jogador.vidas, VIDAS_INICIAIS);
    char ajudas[128];
    snprintf(ajudas, sizeof(ajudas),
             "[P] Pular%s  [T] Trocar%s  [H] Dica%s  [S] Sair",
             jogador.usou_pular ? " (usado)" : "",
             jogador.usou_trocar ? " (usado)" : "",
             jogador.usou_dica ? " (usado)" : "");
    box_line(ajudas);
    box_border();

    box_title("PERGUNTA");
    box_line(questao.pergunta);
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

void exibir_feedback(int correto, char resposta_certa) {
    if (correto)
        printf("\n> RESPOSTA CORRETA! +10 pontos.\n");
    else
        printf("\n> RESPOSTA INCORRETA! Resposta certa: %c.\n", resposta_certa);
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
    jogador->vidas = 1;
    jogador->pontuacao = 0;
    jogador->nivel_atual = 1;
    jogador->usou_pular = 0;
    jogador->usou_trocar = 0;
    jogador->usou_dica = 0;
}

// --- BANCO DE QUESTOES ---
void carregar_banco_de_questoes(Questao banco[]) {
    // Nível 1 - Fácil
    banco[0] = (Questao){"Qual e a forma correta de declarar uma variavel inteira em C?",
        {"int numero;", "integer numero;", "numero int;", "var numero;"},
        'A', "Em C, sempre comeca com o tipo.", 1};
    banco[1] = (Questao){"O que acontece se voce declarar uma variavel local sem inicializa-la?",
        {"Ela recebe automaticamente o valor zero.", "Ela recebe um valor fixo do compilador.", "Ela pode conter qualquer valor lixo que já estava na memória.", "Ela não pode ser usada até ser inicializada."},
        'C', "Variaveis locais em C não tem valor padrao.", 1};
    banco[2] = (Questao){"Qual das alternativas abaixo é incorreta sobre variáveis em C?",
        {"Uma variável precisa ter tipo e nome.", "Mesmo nome pode repetir no escopo.", "Pode declarar varias na mesma linha.", "Tipo define espaco em memoria."},
        'B', "Nomes nao se repetem no mesmo bloco.", 1};
    banco[3] = (Questao){"Suponha char letra = 'A';. Qual imprime a letra corretamente?",
        {"printf(\"%s\", letra);", "printf(\"%c\", letra);", "printf(\"%d\", letra);", "printf(\"%f\", letra);"},
        'B', "%d e inteiro, %f e ponto flutuante.", 1};
     banco[4] = (Questao){"Sobre o uso do scanf para leitura de strings em C, qual afirmação é correta?",
        {" scanf(''%s'', nome); lê a string inteira, mesmo que tenha espaços.", "scanf(''%s'', nome); interrompe a leitura no primeiro espaço encontrado.", "Sempre é necessário usar &nome em scanf(''%s'', ...)", " Não existe risco de buffer overflow ao usar scanf para strings."},
        'B', "%s le ate espaco ou tabulacao.", 1};

    // Nível 2 - Médio
    banco[5] = (Questao){"Qual e a saida do codigo: int x; printf(\"%d\", x); return 0;}",
        {"Imprime 0.", "Imprime 1.", "Valor indefinido (lixo).", "Nao compila."},
        'C', "Variaveis locais nao inicializadas tem valor indeterminado.", 2};
    banco[6] = (Questao){"Qual printf exibe corretamente Nome e Idade?, ome [ ] = ''Maria''; int idade = 22; ",
        {"printf(\"Nome: %s, Idade: %d\", nome, idade);", "printf(\"Nome: %c, Idade: %f\", nome, idade);", "printf(\"%d, %s\", nome, idade);", "printf(\"Maria, 22\");"},
        'A', "%s imprime string e %d imprime inteiro.", 2};
   
    banco[7] = (Questao){"char nome[20]; scanf(''%19s'', nome);, Qual a vantagem de usar %19s no scanf?",
        {" Impede que o usuário digite menos de 19 caracteres", " Permite armazenar apenas números e não letras.", "Limita a quantidade de caracteres lidos e evita estouro do vetor.", "Torna a leitura mais rapida."},
        'C', "Limita quantidade de caracteres lidos.", 2};
    banco[8] = (Questao){"Sobre a função fgets em C, marque a alternativa correta:",
        {" Lê apenas uma palavra, assim como o scanf(''%s'').", "Lê apenas uma palavra, assim como o scanf(''%s'').", "Não funciona para strings, apenas números.", "Precisa de & antes da variavel."},
        'B', "fgets e mais segura que scanf para strings.", 2};
    banco[9] = (Questao){"Qual biblioteca deve ser incluida para usar rand()em c?",
        {"#include <math.h>", "#include <stdlib.h>", "#include <stdio.h>", "#include <time.h>"},
        'B', "A função rand() esta na mesma biblioteca de malloc e free.", 2};
    banco[10] = (Questao){"O que rand() retorna por padrao?",
        {"Um número real entre 0 e 1.", "Um número inteiro entre 0 e RAND_MAX.", "Um número inteiro entre -RAND_MAX e RAND_MAX.", "Sempre 0."},
        'B', "O limite máximo é definido por uma constante do C chamada RAND_MAX", 2};

    // Nível 3 - Difícil
    banco[11] = (Questao){ "Considere dois vetores 2D: ( [u = (2, -1), \ quad v = (1, 3)]O produto interno é dado por:[u \ cdot v = u_x v_x + u_y v_y] ), Com base nisso, qual é o sinal do ângulo entre os vetores e, portanto, o bit recebido?",
        {"Bit=0", "Bit=1", "Bit=-1 (erro)", "Bit=indefinido"},
        'A', "Produto interno positivo -> bit=0.", 3};
    banco[12] = (Questao){"O programa em C utiliza o cálculo do produto interno para decidir o bit recebido:( int a1 = -2, a2 = 4;int b1 = 3, b2 = 1;int produto = a1*b1 + a2*b2; ), Se produto == 0, o sistema deve registrar erro. Qual será o valor do bit calculado?",
        {"0", "1", "-1 (erro)", "Indefinido"},
        'B', "Calcule o produto: (-1)+2*1.", 3};
    banco[13] = (Questao){"Qual é o problema de usar rand() sem configurar a semente com srand()?",
        {"O programa pode trava.", "O compilador não aceita compilar.", " A sequência de números será sempre a mesma a cada execução", "Não há problema, sempre será totalmente aleatório."},
        'C', "Pense no comportamento padrão quando não se inicializa algo em C.", 3};
    banco[14] = (Questao){"Qual das alternativas gera corretamente um número inteiro aleatório entre 0 e 9?",
        {"rand() % 10;", "rand(10);", "rand() / 10;", "rand(0,9);"},
        'A', " O operador % (módulo) ajuda a limitar o intervalo", 3};
    banco[15] = (Questao){"Suponha o seguinte código:( #include <stdio.h>, #include <stdlib.h>, #include <time.h>  int main() { srand(time(NULL)); printf(''%d\n'', rand() % 2); return 0;} ), O que esse programa imprime?",
        {"Sempre 0.", "Sempre 1.", "Valores aleatorios entre 0 e 1.", "Valores aleatorios entre 1 e 2."},
        'C', "%2 gera resto 0 ou 1.", 3};
 }

Questao obter_questao_por_nivel(Questao banco[], int nivel, int indice_questao_nivel) {
    int contador_nivel = 0;
    for (int i = 0; i < NUM_TOTAL_QUESTOES; i++) {
        if (banco[i].nivel == nivel) {
            if (contador_nivel == indice_questao_nivel) return banco[i];
            contador_nivel++;
        }
    } 
    return banco[0];
}

Questao obter_nova_questao_aleatoria(Questao banco[], int nivel_atual, Questao questao_antiga) {
    Questao nova_questao;
    int mesma_pergunta;
    do {
        mesma_pergunta = 0;
        int indice_aleatorio = rand() % NUM_TOTAL_QUESTOES;
        nova_questao = banco[indice_aleatorio];

        if (nova_questao.nivel == nivel_atual && strcmp(nova_questao.pergunta, questao_antiga.pergunta) != 0) {
            return nova_questao;
        } else {
            mesma_pergunta = 1;
        }
    } while (mesma_pergunta == 1);
    return nova_questao;
}