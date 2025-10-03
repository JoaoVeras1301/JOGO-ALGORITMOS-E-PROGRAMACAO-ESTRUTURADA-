#include <stdio.h> 

int vida = 1; // Valor inicial da vida
int opcao; // variavel para o switch funcionar

int main(){

printf("Digite uma das opções: \n 1) \n 2) \n 3) \n");
scanf("%d", &opcao); // puxa o valor digitado e adicona no case

if (vida > 0) // caso a vida for menor que 0 encerra o switch
{

   switch (opcao)
    {
    case 1:
        printf("Opcao errada! \n");
        vida -= 1; // remove 1 no valor da vida
        break;
    case 2:
        printf("Opcao errada! \n");
        vida -= 1;
        break;
    case 3:
        printf("Opcao certa! \n");
        vida += 1; // adiciona 1 no valor da vida
        break;    
    default:
        printf("Opcao invalida \n");
        break;
    } 
}

printf("Vida final: %d\n", vida); // mostra saldo final de vidas

getchar();
getchar();
    
}

