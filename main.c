#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define LARGURA 100
#define ALTURA 20
#define CARACTERE_AGUA ' '

#define ALTURA_PLAYER 3
#define LARGURA_PLAYER 9

const char *PLAYER_SPRITE[ALTURA_PLAYER] ={
    "     ++  ",
    "+===+++++",
    "+        "};

typedef struct{
    int x, y;
}PLAYER;

PLAYER player;

HANDLE hConsole;
CHAR_INFO consoleBuffer[LARGURA*ALTURA];
COORD bufferSize = {LARGURA, ALTURA};
COORD bufferCoord = {0,0};
SMALL_RECT consoleWriteArea = {0, 0, LARGURA- 1, ALTURA-1};

void desenha_tela(){
    for(int i = 0; i < LARGURA*ALTURA; i++ ){
        consoleBuffer[i].Char.AsciiChar = CARACTERE_AGUA;
        consoleBuffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    }

    for(int i = 0; i < ALTURA_PLAYER; i ++){
        for(int j = 0; j < LARGURA_PLAYER; j++){
            int indice = (player.y + i) * LARGURA + (player.x + j);
            consoleBuffer[indice].Char.AsciiChar = PLAYER_SPRITE[i][j];
        }
    }

    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);

}   

int main(){
    player.x = 20;
    player.y = 10;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    while(1){
        desenha_tela();
        player.x++;
        Sleep(500);
    }

    return 0;
}