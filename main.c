#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// IMPORTANTE! o ponto 0 do eixo Y começa no topo, não em baixo

// Constantes do buffer
#define LARGURA 125
#define ALTURA 25

// Constantes do mapa
#define CARACTERE_AGUA ' '
#define CARACTERE_SUPERFICE '='
#define CARACTERE_PAREDES '|'
#define CARACTERE_CHAO '_'

// Constantes do player
#define ALTURA_PLAYER 2
#define LARGURA_PLAYER 9
#define VELOCIDADE_X 2
#define VELOCIDADE_Y 1

/*
    Enzo Capitani: Sprites iniciais do submarino, ta uma bosta
*/
const char *PLAYER_ESQUERDA[ALTURA_PLAYER] = {
    "   ++++ +",
    "==+++++++",
};

const char *PLAYER_DIREITA[ALTURA_PLAYER] = {
    "+ ++++   ",
    "+++++++==",
};

const char **PLAYER_SPRITE = PLAYER_DIREITA;

/*
    Enzo Capitani: Struct do player, ainda tem q colocar o nivel de oxigenio,
    pessoas salvas, score
*/
typedef struct
{
    int x, y;
} PLAYER;

// Inicialização do player
PLAYER player;

/*
    E. Emanoel: Criação do sistema de tiros
*/

// Constantes do tiro

#define TIRO_ICON 'O'
#define MAX_TIROS 10

/*
    E. Emanoel: Struct do tiro, ainda falta adicionar countdown para cada tiro
*/

typedef struct
{
    int x, y;
    int ativo;
    int dx
} TIRO;

// Inicialização de um vetor de tiros

TIRO tiros[MAX_TIROS];

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
CHAR_INFO consoleBuffer[LARGURA * ALTURA];

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
SMALL_RECT consoleWriteArea = {0, 0, LARGURA - 1, ALTURA - 1};

// Enzo Capitani: Função que desenha os caracteres no console
void desenha_tela()
{

    /*
        Enzo Capiani: Esse loop preenche os caracteres da agua e define as cores,
        esse atributte tem q deixar as 3 cores primarias (eu acho que sao), se remover
        um ele cria uma mistura, as cores são em hexadecimal
        Enzo Capitani: Foi adicionada a criação da "caixa" do mapa
    */
    for (int i = 0; i < LARGURA * ALTURA; i++)
    {
        if (i > LARGURA && i < LARGURA * 2)
        {
            consoleBuffer[i].Char.AsciiChar = CARACTERE_SUPERFICE;
            consoleBuffer[i].Attributes = FOREGROUND_BLUE;
            continue;
        }
        if (i % LARGURA == 0 || i % LARGURA == LARGURA - 1)
        {
            consoleBuffer[i].Char.AsciiChar = CARACTERE_PAREDES;
            consoleBuffer[i].Attributes = FOREGROUND_BLUE;
            continue;
        }
        if (i > LARGURA * ALTURA - LARGURA && i < LARGURA * ALTURA)
        {
            consoleBuffer[i].Char.AsciiChar = CARACTERE_CHAO;
            consoleBuffer[i].Attributes = FOREGROUND_BLUE;
            continue;
        }
        consoleBuffer[i].Char.AsciiChar = CARACTERE_AGUA;
        consoleBuffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    }

    /*
        Enzo Capitani: Nesse loop, calcula a posição do player e transfere a posição dele,
        que é baseada em um plano bidimensional, para um vetor unidimensional, ainda nao entendi muito
        bem a formula :P, mas é isso ae
    */
    for (int i = 0; i < ALTURA_PLAYER; i++)
    {
        for (int j = 0; j < LARGURA_PLAYER; j++)
        {
            // Essa formula aq
            int indice = (player.y + i) * LARGURA + (player.x + j);
            consoleBuffer[indice].Char.AsciiChar = PLAYER_SPRITE[i][j];
            consoleBuffer[indice].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        }
    }

    /*
        Enzo Capitani: Aqui desenha as paradas no console, recebe todas as variaveis criadas acima
        só nao entendi o pq de o ultimo ter o &
    */
    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
}

/*
    Enzo Capitani: Parte das acoes do player, movimentação e etc, precisa adicionar a ação de atirar
*/
void acoesPlayer()
{
    if (GetAsyncKeyState(VK_RIGHT))
    {
        player.x += VELOCIDADE_X;
        PLAYER_SPRITE = PLAYER_DIREITA;
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        player.x -= VELOCIDADE_X;
        PLAYER_SPRITE = PLAYER_ESQUERDA;
    }
    if (GetAsyncKeyState(VK_UP))
    {
        player.y -= VELOCIDADE_Y;
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        player.y += VELOCIDADE_Y;
    }
}

/*
    Enzo Capitani: O update() é responsável por atualizar tudo que é necessário nas acoes do jogo
    atualmente ele só verifica se o player saiu do mapa e se saiu, põe ele de volta
*/
void update()
{
    if (player.y < 1)
    {
        player.y = 1;
    }
    if (player.y + ALTURA_PLAYER > ALTURA)
    {
        player.y = ALTURA - ALTURA_PLAYER;
    }
    if (player.x < 1)
    {
        player.x = 1;
    }
    if (player.x + LARGURA_PLAYER > LARGURA - 1)
    {
        player.x = LARGURA - LARGURA_PLAYER - 1;
    }
}

int main()
{
    // Enzo Capitani: aqui define as posições iniciais do player
    player.x = 20;
    player.y = 10;

    // Enzo Capitani: aqui indica a saida padrão do programa, que no caso a saída é o console
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Game loop
    while (1)
    {
        acoesPlayer();
        update();
        desenha_tela();
        Sleep(90);
    }

    return 0;
}