## Jogo Quiz.C

Fizemos esse jogo por terminal em lingugem C. O jogador responde perguntas divididas em níveis (Fácil, Médio, Difícil), tem um número limitado de vidas, pontua por acertos e pode usar ajudas.

O jogo carrega todas as suas perguntas de um arquivo perguntas.json externo, permitindo fácil edição e expansão do quiz.

---

## Recursos

* Banco de Perguntas Externo: Todas as perguntas, respostas e dicas são lidas do arquivo perguntas.json.
* Sistema de Nível: O jogador deve responder 3 perguntas aleatórias por nível para avançar.
* Seleção Aleatória: As perguntas são embaralhadas a cada execução, garantindo que o jogo seja diferente toda vez.
* Vidas e Pontuação: Começa com 1 vida e pode acumular até 3.
* Ajudas por Nível: P (Pular), T (Trocar), H (Dica). As ajudas são resetadas a cada novo nível.
* Lógica de "Trocar": A opção "Trocar" puxa uma pergunta reserva do mesmo nível que ainda não foi vista.
* Interface Limpa: O jogo é apresentado em uma interface de caixa de texto simples (ASCII).

---

## Requisitos

* Compilador C (GCC, clang, MinGW, etc.).
* Terminal/console para execução.
* O arquivo perguntas.json deve estar na mesma pasta do executável para que o jogo funcione.

---

## Compilação

No Linux/macOS (terminal):

    gcc main.c parser.c ui.c utils.c -o output/Jogo_do_C

No Windows (usando MinGW/msys):

    gcc main.c parser.c ui.c utils.c -o output\Jogo_do_C

---

## Como jogar

1. Compile o jogo.c (conforme instruções acima).
2. Coloque o executável (Jogo_do_C.exe) e o perguntas.json na mesma pasta.
3. Execute o binário.
4. No menu principal escolha 1 para iniciar, 2 para instruções ou 3 para sair.
5. Você deve responder a 3 perguntas sorteadas aleatoriamente para avançar de nível.
6. Comandos de ajuda (cada um pode ser usado apenas uma vez por nível):
    * P — Pular a pergunta (não ganha pontos/vida).
    * T — Trocar por outra pergunta do mesmo nível (se houver).
    * H — Mostrar uma dica.
    * S — Sair imediatamente do jogo.
7. Você começa com 1 vida e pode alcançar até 3. Respostas certas aumentam sua pontuação e podem recuperar vidas. Se suas vidas chegarem a 0, o jogo termina.

---

## Créditos

Autor original do código: Alexandre Alves, Diogo Souza, Erick Teixeira, Guilherme Augusto, João Veras e Lucas Souza