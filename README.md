# Jogo Quiz — C

Fizemos esse jogo por terminal em lingugem C. O jogador responde perguntas divididas em níveis (Fácil, Médio, Difícil), tem um número limitado de vidas, pontua por acertos e pode usar ajudas uma vez cada (Pular, Trocar, Dica).

---

## Recursos

* Perguntas organizadas por nível
* Vidas e pontuação
* Ajudas: **P** (Pular), **T** (Trocar), **H** (Dica)
* Interface em caixa de texto simples (ASCII)
* Banco de perguntas embutido no código (arquivo `jogo.c`)

---

## Requisitos

* Compiler C (GCC, clang, MinGW, etc.)
* Terminal/console

---

## Compilação

No Linux/macOS (terminal):

```bash
gcc -o jogo jogo.c
./jogo
```

No Windows (MinGW/msys):

```powershell
gcc -o jogo.exe jogo.c
.\\jogo.exe
```

> Dica: se o compilador emitir `warning` sobre tamanho de buffer, revise strings das perguntas/alternativas ou use `-Wall` para ver avisos.

---

## Como jogar

1. Execute o binário compilado.
2. No menu principal escolha `1` para iniciar, `2` para instruções ou `3` para sair.
3. Durante o jogo, você verá a pergunta e quatro alternativas (A, B, C, D).
4. Digite a letra correspondente à sua resposta e pressione Enter.
5. Comandos de ajuda (cada um pode ser usado apenas uma vez por jogo):

   * `P` — Pular a pergunta
   * `T` — Trocar por outra pergunta do mesmo nível
   * `H` — Mostrar uma dica
   * `S` — Sair imediatamente do jogo
6. Você começa com uma vida, ao decorrer do jogo voce pode chegar até três 

---

## Créditos

Autor original do código: *Erick Teixeira, Guilherme Augusto, João Veras. Alexandre Alves*

---
