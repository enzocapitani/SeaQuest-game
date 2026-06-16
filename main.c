#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//IMPORTANTE! o ponto 0 do eixo Y começa no topo, não em baixo

//Constantes do buffer
#define LARGURA 100
#define ALTURA 20

//Constantes do mapa
#define CARACTERE_AGUA ' '

// Constantes do player
#define ALTURA_PLAYER 2
#define LARGURA_PLAYER 9


/*
    Enzo Capitani: Sprite inicial do submarino, ta uma bosta
*/
const char *PLAYER_SPRITE[ALTURA_PLAYER] ={
    "+ ++++   ",
    "+++++++==",
};

/*
    Enzo Capitani: Struct do player, ainda tem q colocar o nivel de oxigenio,
    pessoas salvas, score
*/
typedef struct{
    int x, y;
}PLAYER;

//Inicialização do player
PLAYER player;


/*
    IMPORTANTE!!!
*/

/*
    Enzo Capitani: hConsole vai definir onde a gente vai desenhar (no terminal)
    no caso ja instanciei lá no main()

*/
HANDLE hConsole;

/*
    Enzo capitani: Essa variável armazena os caracteres que são desenhados no buffer
    aqui onde a magica acontece, cuidado se forem testar criar os personagens, tem q seguir uma
    formula q tem la no desenhaTela()
*/
CHAR_INFO consoleBuffer[LARGURA*ALTURA];

// Enzo Capitani: esse buffersize, só fala o tamanho do buffer kkkkk
COORD bufferSize = {LARGURA, ALTURA};

/*
    Enzo Capitani: esse buffercoord diz o ponto 0 de onde o nosso buffer vai desenhar, o submarino
    quebra em alguns valores, recebe X e Y
*/
COORD bufferCoord = {0, 0};

/*
    Enzo Capitani: esse consoleWriteArea em específico, dita a área de desenho no console
    os 2 primeiros parâmetros diz o inicio, que começa do canto superior esquerdo
    e os outros 2 diz o fim, que não faço ideia do pq sao subtraidos por 2
*/
SMALL_RECT consoleWriteArea = {0, 0, LARGURA- 1, ALTURA-1};

// Enzo Capitani: Função que desenha os caracteres no console
void desenha_tela(){

    /*
        Enzo Capiani: Esse loop preenche os caracteres da agua e define as cores,
        esse atributte tem q deixar as 3 cores primarias (eu acho que sao), se remover
        um ele cria uma mistura, as cores são em hexadecimal
    */
    for(int i = 0; i < LARGURA*ALTURA; i++ ){
        consoleBuffer[i].Char.AsciiChar = CARACTERE_AGUA;
        consoleBuffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    }

    /*
        Enzo Capitani: Nesse loop, calcula a posição do player e transfere a posição dele,
        que é baseada em um plano bidimensional, para um vetor unidimensional, ainda nao entendi muito
        bem a formula :P, mas é isso ae
    */
    for(int i = 0; i < ALTURA_PLAYER; i ++){
        for(int j = 0; j < LARGURA_PLAYER; j++){
            //Essa formula aq
            int indice = (player.y + i) * LARGURA + (player.x + j);
            consoleBuffer[indice].Char.AsciiChar = PLAYER_SPRITE[i][j];
        }
    }

    /*
        Enzo Capitani: Aqui desenha as paradas no console, recebe todas as variaveis criadas acima
        só nao entendi o pq de o ultimo ter o &
    */
    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);

}


int main(){
    //Enzo Capitani: aqui define as posições iniciais do player
    player.x = 20;
    player.y = 10;

    // Enzo Capitani: aqui indica a saida padrão do programa, que no caso a saída é o console
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    //Game loop
    while(1){
        player.x++;
        
        desenha_tela();
        Sleep(100);
    }

    return 0;
}