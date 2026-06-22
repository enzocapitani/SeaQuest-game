#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

// IMPORTANTE! o ponto 0 do eixo Y começa no topo, não em baixo

// CONSTANTES DA TELA INICIAL!

#define LARGURA_LOGO 56
#define ALTURA_LOGO 5
#define LOGO_X 35
#define LOGO_Y 8

const char *MRBX_QUESTLOGO[ALTURA_LOGO] = {
    " __  __ ____  ______  __   ___  _   _ _____ ____ _____ ",
    "|  \\/  |  _ \\| __ ) \\/ /  / _ \\| | | | ____/ ___|_   _|",
    "| |\\/| | |_) |  _ \\\\  /  | | | | | | |  _| \\___ \\ | |  ",
    "| |  | |  _ <| |_) /  \\  | |_| | |_| | |___ ___) || |  ",
    "|_|  |_|_| \\_\\____/_/\\_\\  \\__\\_\\\\___/|_____|____/ |_|  "};

// CONSTANTES LOGO GAME OVER!

#define ALTURA_GAMEOVER 8
#define LARGURA_GAMEOVER 100
#define GAME_OVER_X 13
#define GAME_OVER_Y 3

const wchar_t *GAMEOVER[] = {

    L" .d8888b.         d8888 888b     d888 8888888888      .d88888b.  888     888 8888888888 8888888b. ",
    L"d88P  Y88b       d88888 8888b   d8888 888            d88P   Y88b 888     888 888        888   Y88b",
    L"888    888      d88P888 88888b.d88888 888            888     888 888     888 888        888    888",
    L"888            d88P 888 888Y88888P888 8888888        888     888 Y88b   d88P 8888888    888   d88P",
    L"888  88888    d88P  888 888 Y888P 888 888            888     888  Y88b d88P  888        8888888P  ",
    L"888    888   d88P   888 888  Y8P  888 888            888     888   Y88o88P   888        888 T88b  ",
    L"Y88b  d88P  d8888888888 888       888 888            Y88b. .d88P    Y888P    888        888  T88b ",
    L" Y88888P8  d88P     888 888       888 8888888888      Y8888888P      Y8P     8888888888 888   T88b",

};

// Constantes do buffer
#define LARGURA 126
#define ALTURA 28

#define TELA_INICIAL 0
#define TELA_JOGO 1
#define TELA_GAMEOVER 2

// Constantes do mapa
#define CARACTERE_AGUA ' '
#define CARACTERE_SUPERFICE '~'
#define CARACTERE_PAREDES '|'
#define CARACTERE_CHAO '_'

// Constantes do player
#define ALTURA_PLAYER 2
#define LARGURA_PLAYER 9
#define VELOCIDADE_X 2
#define VELOCIDADE_Y 1
#define TOTAL_FRAMES_JOGADOR 3
#define VELOCIDADE_ANIMACAO_PLAYER 1 // as velocidades de animação são inversamente proporcionais ao seus defines

/*
    Enzo Capitani: Sprites iniciais do submarino, ta uma bosta
*/
const char *PLAYER_ESQUERDA[TOTAL_FRAMES_JOGADOR][ALTURA_PLAYER] = {
    {
        "  |_     ",
        "([___]=|/",
    },
    {
        "  |_     ",
        "([___]=|-",
    },
    {
        "  |_     ",
        "([___]=|\\",
    }};

const char *PLAYER_DIREITA[TOTAL_FRAMES_JOGADOR][ALTURA_PLAYER] = {
    {
        "      _| ",
        "\\|=[___])",
    },
    {
        "      _| ",
        "-|=[___])",
    },
    {
        "      _| ",
        "/|=[___])",
    },
};

const char *(*PLAYER_SPRITE)[ALTURA_PLAYER] = PLAYER_DIREITA;

/*
    Enzo Capitani: Struct do player, ainda tem q colocar o nivel de oxigenio,
    pessoas salvas, score
*/
typedef struct
{
    int x, y, score, nivelOxigenio, frameAtual;
    int vida;
} PLAYER;

// Inicialização do player
PLAYER player;

/*
    E. Emanoel: Criação do sistema de tiros
*/

// Constantes do tiro

#define TIRO_ICON '='
#define MAX_TIROS 5
#define TIRO_VEL 10

/*
    E. Emanoel: Struct do tiro, ainda falta adicionar countdown para cada tiro
*/

typedef struct
{
    int x, y;
    int ativo;
    int dx;
} TIRO;

// Inicialização de um vetor de tiros

TIRO tiros[MAX_TIROS];

/*
    E. Emanoel: Criação do sistema de peixes
*/

// Constantes dos peixes

#define ALTURA_PEIXE 2
#define LARGURA_PEIXE 3
#define MAX_PEIXE 15
#define TOTAL_FRAMES_PEIXE 2
#define VELOCIDADE_ANIMACAO_PEIXE 12 // as velocidades de animação são inversamente proporcionais ao seus defines
// E. Emanoel: Sprites do peixe

const char *PEIXE_DIREITA[TOTAL_FRAMES_PEIXE][ALTURA_PEIXE] = {
    {"><>",
     "   "},
    {"   ",
     "><>"}};

const char *PEIXE_ESQUERDA[TOTAL_FRAMES_PEIXE][ALTURA_PEIXE] = {
    {
        "<><",
        "   ",
    },
    {"   ",
     "<><"}};

/*
    E. Emanoel: Struct do peixe
*/

typedef struct
{
    int x, y;
    int dx;
    int vivo;
    WORD cor;
} PEIXE;

// Inicialização de vetores de peixes

PEIXE peixes[MAX_PEIXE];

void reset(void);

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

/*
    Henry: inicializa o relogiolobal, cada sprite é modificado de acordo com o tempo
    do relogioGlobal, ele será incrementado em 1 a cada cilco update()
*/

int relogioGlobal = 0;

int tela_atual = 0;

/*
    Enzo Capitani: Parte que desenha a tela do game over, mesma coisa da que desenha a tela do jogo
*/
void desenhaTelaGameOver()
{

    for (int i = 0; i < LARGURA * ALTURA; ++i)
    {
        consoleBuffer[i].Char.AsciiChar = ' ';
        consoleBuffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    }

    for (int i = 0; i < ALTURA_GAMEOVER; ++i)
    {
        for (int j = 0; j < LARGURA_GAMEOVER; ++j)
        {
            int indice = (GAME_OVER_Y + i) * LARGURA + (GAME_OVER_X + j);
            consoleBuffer[indice].Char.AsciiChar = GAMEOVER[i][j];
            consoleBuffer[indice].Attributes = FOREGROUND_RED;
        }
    }

    char textoIniciar[35];
    sprintf(textoIniciar, "PRESSIONE SPACE PARA VOLTAR AO MENU");

    int inicio = (ALTURA_GAMEOVER + GAME_OVER_Y + 1) * (LARGURA) + 35;
    for (int i = 0; textoIniciar[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoIniciar[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_BLUE;
    }

    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
}

/*
    Enzo Capitani: Parte que desenha a tela incial, mesma coisa da que desenha a tela do jogo
*/
void desenhaPeixeTela()
{

    /*
   E. Emanoel: Desenha os peixes na tela e vê se já tão fora do mapa
   */
    for (int p = 0; p < MAX_PEIXE; p++) // Int P são os peixes criados
    {
        if (peixes[p].vivo) // Verifica se o peixe está vivo | Se ele está morto, nem executa
        {
            const char *(*PEIXE_SPRITE)[ALTURA_PEIXE] = (peixes[p].dx == 1) ? PEIXE_DIREITA : PEIXE_ESQUERDA; // sprite pra direção que o peixe tá indo

            int frameAtualPeixe = (relogioGlobal / VELOCIDADE_ANIMACAO_PEIXE) % TOTAL_FRAMES_PEIXE;

            for (int i = 0; i < ALTURA_PEIXE; i++)
            {
                for (int j = 0; j < LARGURA_PEIXE; j++)
                {
                    int posX = peixes[p].x + j;
                    int posY = peixes[p].y + i;

                    if (posX >= 0 && posX < LARGURA && posY >= 0 && posY < ALTURA)
                    {
                        // Henry: verifica se o peixe está indo pra direita ou esquerda, e seu respectivo
                        char caractere = (peixes[p].dx == 1) ? PEIXE_DIREITA[frameAtualPeixe][i][j] : PEIXE_ESQUERDA[frameAtualPeixe][i][j];

                        int indice_peixe = (posY * LARGURA) + posX;
                        consoleBuffer[indice_peixe].Char.AsciiChar = caractere;
                        consoleBuffer[indice_peixe].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
                    }
                }
            }
        }
        // Henry: esse if verifica se o peixe morreu pela extremidade do mapa, faz com que o sprite de morte apenas apareça quando o peixe morre pelo jogador
        else if (!(peixes[p].x <= 0 || peixes[p].x >= LARGURA))
        {
            consoleBuffer[peixes[p].y * LARGURA + peixes[p].x].Char.AsciiChar = 'X';
            consoleBuffer[peixes[p].y * LARGURA + peixes[p].x].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        }
    }
}

void updateTelaInicial(){
    for (int p = 0; p < MAX_PEIXE; p++)
    {
        if (peixes[p].vivo)
        {
            peixes[p].x += peixes[p].dx;

            if (peixes[p].x <= 0 - LARGURA_PEIXE || peixes[p].x >= LARGURA + LARGURA_PEIXE)
            {
                peixes[p].vivo = 0;
            }
        }
    }
}

void desenhaTelaInicial()
{

    for (int i = 0; i < LARGURA * ALTURA; ++i)
    {
        consoleBuffer[i].Char.AsciiChar = ' ';
        consoleBuffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    }

    desenhaPeixeTela();

    for (int i = 0; i < ALTURA_LOGO; ++i)
    {
        for (int j = 0; j < LARGURA_LOGO; ++j)
        {
            int indice = (LOGO_Y + i) * LARGURA + (LOGO_X + j);
            consoleBuffer[indice].Char.UnicodeChar = MRBX_QUESTLOGO[i][j];
            consoleBuffer[indice].Attributes = FOREGROUND_BLUE;
        }
    }

    char textoIniciar[35];
    sprintf(textoIniciar, "PRESSIONE SPACE PARA INICIAR");

    int inicio = (ALTURA_LOGO + LOGO_Y + 1) * (LARGURA) + 48;
    for (int i = 0; textoIniciar[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoIniciar[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_BLUE;
    }

    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
}

/*
    Enzo Capitani: esse desenha score ele desenha o score na pos 5 do vetor
    unidimensional do buffer
*/
void desenhaScore()
{
    char textoScore[30];
    sprintf(textoScore, "Score: %d", player.score);

    int inicio = 5;
    for (int i = 0; textoScore[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoScore[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN;
    }
}

/*
    Enzo Capitani: Parte que desenha a vida do player na tela
*/
void desenhaVida()
{
    char textoVida[15];
    sprintf(textoVida, "Vida: %d", player.vida);

    int inicio = LARGURA - 15;
    for (int i = 0; textoVida[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoVida[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN;
    }
}

void desenhaBarraOxigenio()
{
    /*
        Enzo Capitani: Aqui ele aloca a quantidade de '/' que representa o nivel de oxigenio
        eu encontrei esse loop ai, toda vez ele soma o i em 50 e verifica, se for maior que o nivel de oxigenio -> ' '
        se nao -> '/'
    */
    int frameAtual = relogioGlobal % 3;

    WORD corBarraOx = FOREGROUND_RED | FOREGROUND_GREEN;

    if (frameAtual == 0 && player.nivelOxigenio < 250)
    {
        corBarraOx = FOREGROUND_RED;
    }
    else if (player.nivelOxigenio < 250)
    {
        corBarraOx = FOREGROUND_RED | FOREGROUND_GREEN;
    }

    char barras[25];
    int indiceBarras = 0;
    for (int i = 0; i <= 1000; i += 50)
    {
        if (i <= player.nivelOxigenio)
        {
            barras[indiceBarras] = 'H';
            indiceBarras++;
            continue;
        }
        barras[indiceBarras] = ' ';
        indiceBarras++;
    }
    barras[21] = '\0';

    char barraOxigenio[51];
    sprintf(barraOxigenio, "OXIGENIO: [%s]", barras);

    int inicio = 40;
    for (int i = 0; barraOxigenio[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = barraOxigenio[i];
        consoleBuffer[inicio + i].Attributes = corBarraOx;
    }
}

/*
        Enzo Capiani: Esse loop preenche os caracteres da agua e define as cores,
        esse atributte tem q deixar as 3 cores primarias (eu acho que sao), se remover
        um ele cria uma mistura, as cores são em hexadecimal
        Enzo Capitani: Foi adicionada a criação da "caixa" do mapa
    */
void desenhaMapa()
{
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

        if (i > LARGURA * ALTURA - LARGURA * 2 && i < (LARGURA) * (ALTURA - 1))
        {
            consoleBuffer[i].Char.AsciiChar = CARACTERE_CHAO;
            consoleBuffer[i].Attributes = FOREGROUND_BLUE;
            continue;
        }
        consoleBuffer[i].Char.AsciiChar = CARACTERE_AGUA;
        consoleBuffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    }
}

// Enzo Capitani: Função que desenha os caracteres no console
void desenha_tela()
{
    desenhaMapa();

    /*
        Enzo Capitani: Nesse loop, calcula a posição do player e transfere a posição dele,
        que é baseada em um plano bidimensional, para um vetor unidimensional, ainda nao entendi muito
        bem a formula :P, mas é isso ae
        Enzo Capitani: Henry mudou e nao falou esse safado
    */

    int frameAtualPlayer = (relogioGlobal / VELOCIDADE_ANIMACAO_PLAYER) % TOTAL_FRAMES_JOGADOR;

    for (int i = 0; i < ALTURA_PLAYER; i++)
    {
        for (int j = 0; j < LARGURA_PLAYER; j++)
        {
            /*
            Henry: posX e posY calculam onde cada caractere do player está.
            o if garante que essas posições estão no limite da tela
            */
            int posX = player.x + j;
            int posY = player.y + i;
            if (posX >= 0 && posX < LARGURA && posY >= 0 && posY < ALTURA)
            {
                int indice = posY * LARGURA + posX;

                char caractere = PLAYER_SPRITE[frameAtualPlayer][i][j];

                if (caractere != ' ')
                {
                    consoleBuffer[indice].Char.AsciiChar = PLAYER_SPRITE[frameAtualPlayer][i][j];
                    consoleBuffer[indice].Attributes = FOREGROUND_RED;
                }
            }
        }
    }

    /*
        E. Emanoel: Desenha os tiros na tela e verifica se eles não estão fora do mapa
        */

    for (int i = 0; i < MAX_TIROS; i++)
    {
        if (tiros[i].ativo)
        {
            if (tiros[i].x > 0 && tiros[i].x < LARGURA)
            {
                int indice_tiro = (tiros[i].y * LARGURA) + tiros[i].x;

                consoleBuffer[indice_tiro].Char.AsciiChar = TIRO_ICON;
                consoleBuffer[indice_tiro].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
        }
    }

    //Enzo Capitani: Criei uma função para eu conseguir desenhar os peixes na tela inicial também
    desenhaPeixeTela();

    /*
    Enzo Capitani: Aqui desenha as paradas no console, recebe todas as variaveis criadas acima
        só nao entendi o pq de o ultimo ter o &
        Enzo Capitani: Aqui coloca o desenharScore() antes de desenhar as coisas no console e a barra de oxigenio tb
        */
    desenhaBarraOxigenio();
    desenhaVida();
    desenhaScore();
    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
}

/*
    Enzo Capitani: Esse método muda a tela atual ao apertar espaço, ele recebe a tela em que
    se quer mudar, EX: tela atual -> inicial se espaço apertado tela atual -> tela jogo
*/
void acoesTela(int tela)
{
    if (GetAsyncKeyState(VK_SPACE))
    {
        tela_atual = tela;
        reset();
        Sleep(800);
    }
}

/*
Enzo Capitani: Parte das acoes do player, movimentação e etc, precisa adicionar a ação de atirar
*/
void acoesPlayer()
{
    /*
        Henry: Verificação para o sprite do jogador não vazar, e alteração automática do sprite quando
        muda de direção e recomeçar o sprite
    */
    if (GetAsyncKeyState(VK_RIGHT))
    {
        if (player.x < LARGURA - LARGURA_PLAYER)
            player.x += VELOCIDADE_X;
        if (PLAYER_SPRITE != PLAYER_DIREITA)
        {
            PLAYER_SPRITE = PLAYER_DIREITA;
            player.frameAtual = 0;
        }
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        if (player.x > 0)
            player.x -= VELOCIDADE_X;
        if (PLAYER_SPRITE != PLAYER_ESQUERDA)
        {
            PLAYER_SPRITE = PLAYER_ESQUERDA;
            player.frameAtual = 0;
        }
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
    E. Emanoel: Ação do tiro + direção para esquerda e direita.
    Mas, antes, iniciei todos os tiros inativos (ativos = 0)
*/

void iniciarTiros()
{
    for (int i = 0; i < MAX_TIROS; i++)
    {
        tiros[i].ativo = 0;
    }
}

void acaoTiro()
{
    if (GetAsyncKeyState(VK_SPACE))
    {
        for (int i = 0; i < MAX_TIROS; i++)
        {
            if (!tiros[i].ativo)
            {

                tiros[i].ativo = 1;
                tiros[i].x = (PLAYER_SPRITE == PLAYER_DIREITA) ? player.x + 6 : player.x + 2;
                tiros[i].y = player.y + 1;
                tiros[i].dx = (PLAYER_SPRITE == PLAYER_DIREITA) ? 1 : -1;
                break;
            }
        }
    }
}

void iniciarPlayer()
{
    // Enzo Capitani: aqui define as posições iniciais do player
    player.x = 63 - LARGURA_PLAYER;
    player.y = 14;
    player.score = 0;
    player.nivelOxigenio = 1000;
    player.vida = 5;
}

/*
    E. Emanoel: Função pra fazer aparecer os peixes, mas, antes, inicio eles
*/

void iniciarPeixes()
{
    for (int i = 0; i < MAX_PEIXE; i++)
    {
        peixes[i].vivo = 0;
    }
}

void nascerPeixes()
{

    // E. Emanoel:
    // Aqui a gente adiciona uma chance de 50% do peixe nascer a cada frame
    // Evitando que nasça um monte de peixe de uma vez
    if (rand() % 2 != 0)
    {
        return;
    }

    // E. Emanoel: define as características globais do cardume

    // sorteia o tamanho do cardume (1 a 6 peixes)
    int TAM_CARDUME = (rand() % 5) + 1;

    // nasce na esquerda = 1, nasce na direita = 0
    int NASCE_ESQUERDA = (rand() % 2 == 0);

    // altura exata pros peixes nascerem dentro do mapa
    int BASE_ALTURA = (rand() % (ALTURA - 8)) + 6;

    // se o espaço tiver livre pro peixe nascer, ele vai nascer
    // mas antes tem um check
    int LINHA_LIVRE = 1;

    for (int p = 0; p < MAX_PEIXE; p++)
    {
        // so fazemos com peixes vivos
        if (peixes[p].vivo)
        {
            // espaco necessario pra cada peixe
            int ESPACO_NECESSARIO = TAM_CARDUME * ALTURA_PEIXE;

            // ve se ja tem peixe naquela linha
            if (peixes[p].y >= BASE_ALTURA && peixes[p].y <= BASE_ALTURA + ESPACO_NECESSARIO)
            {
                // se tiver, ele da como linha livre = 0
                LINHA_LIVRE = 0;
                break;
            }
        }
    }

    // se nao tiver, volta o loop
    if (!LINHA_LIVRE)
    {
        return;
    }

    // instancia o cardume
    int PEIXES_CARDUME = 0;

    for (int p = 0; p < MAX_PEIXE; p++)
    {
        if (!peixes[p].vivo)
        {
            // revive os peixes
            peixes[p].vivo = 1;

            // altura base pros peixes não nascerem mortos pelo limite do mapa
            peixes[p].y = BASE_ALTURA + (PEIXES_CARDUME * ALTURA_PEIXE);

            if (peixes[p].y > ALTURA - 5)
            {
                // se os peixes nascerem abaixo da linha, eles morrem
                peixes[p].vivo = 0;
            }

            if (NASCE_ESQUERDA)
            {
                // Aqui a gente faz o peixe nascer dentro da área segura de spawn, sem ser 0, se não ele nasce e morre
                peixes[p].x = 1 + LARGURA_PEIXE - (PEIXES_CARDUME * 2);
                peixes[p].dx = 1;
            }
            else
            {
                // Mesma coisa
                peixes[p].x = LARGURA - LARGURA_PEIXE + (PEIXES_CARDUME * 2);
                peixes[p].dx = -1;
            }

            PEIXES_CARDUME++;

            // se já spawnou a quantidade de peixe pro cardume, para o laço
            if (PEIXES_CARDUME >= TAM_CARDUME)
            {
                break;
            }
        }
    }
}

/*

    E. Emanoel: Adicionei uma função de colisão entre os peixes e os tiros
    ela verifica se o tiro tá ativo, se o peixe tá vivo e se eles colidiram,
    se colidiram, o peixe morre e o tiro é desativado
    E. Emanoel: Agora a função de atualizar os tiros está aqui também

*/

void colisaoPeixeTiro()
{
    for (int t = 0; t < MAX_TIROS; t++)
    {
        if (tiros[t].ativo)
        {
            // a linha de tiro esta aqui agora
            for (int steps = 0; steps < TIRO_VEL; steps++)
            {
                tiros[t].x += tiros[t].dx; // anda uma casa na direção do tiro

                //  check de colisao entre peixe e a linha do tiro
                for (int p = 0; p < MAX_PEIXE; p++)
                {
                    if (peixes[p].vivo)
                    {
                        // verifica se o peixe tá na altura do tiro
                        if (peixes[p].y < tiros[t].y + 1 && peixes[p].y + ALTURA_PEIXE > tiros[t].y &&
                            peixes[p].x < tiros[t].x + 1 && peixes[p].x + LARGURA_PEIXE > tiros[t].x)
                        {
                            peixes[p].vivo = 0; // peixem morto
                            tiros[t].ativo = 0; // tiro some
                            player.score += 50;
                            // se bateu em um peixe, já era, reinicia
                            break;
                        }
                    }
                }

                // Função de atualizar os tiros está aqui, também
                // Verifica se os tiros sairam do mapa
                // Essa função estava lá em update, mas apaguei (E. Emanoel)

                if (!tiros[t].ativo)
                {
                    break;
                }
                if (tiros[t].x <= 0 || tiros[t].x >= LARGURA)
                {
                    tiros[t].ativo = 0;
                    break;
                }
            }
        }
    }
}

/*

    Enzo Capitani: Adicionei uma função de colisão entre o player e os peixes
    ela verifica se o peixe e o player estão dentro um do outro,
    se sim, o peixe morre e o player perde vida

*/

void colisaoPlayerPeixe()
{
    for (int i = 0; i < MAX_PEIXE; i++)
    {
        if (player.x + LARGURA_PLAYER > peixes[i].x && player.x < peixes[i].x + LARGURA_PEIXE && player.y + ALTURA_PLAYER > peixes[i].y && player.y < peixes[i].y + ALTURA_PEIXE && peixes[i].vivo == 1)
        {
            peixes[i].vivo = 0;
            peixes[i].x = 0;
            player.vida--;
        }
    }
}

/*
    Enzo Capitani: O update() é responsável por atualizar tudo que é necessário nas acoes do jogo
    atualmente ele só verifica se o player saiu do mapa e se saiu, põe ele de volta

    E. Emanoel: Vou adicionar as atualizações do tiro aqui também.
    Faz mais sentido do que criar uma nova função pra isso :P
*/

void update()
{
    if (player.y < 1)
    {
        player.y = 1;
    }
    if (player.y + ALTURA_PLAYER > ALTURA - 1)
    {
        player.y = ALTURA - ALTURA_PLAYER - 1;
    }
    if (player.x < 1)
    {
        player.x = 1;
    }
    if (player.x + LARGURA_PLAYER > LARGURA - 1)
    {
        player.x = LARGURA - LARGURA_PLAYER - 1;
    }

    // Henry: Aqui é onde fica as mudanças relativas ao relogioGlobal
    relogioGlobal++;

    // Enzo Capitani: Aqui verifica, se o player estiver na superfice, aumenta o ocigenio, se nao diminui
        if (player.y == 1)
        {
            player.nivelOxigenio += 20;
        }
        else
        {
            player.nivelOxigenio -= 5;
        }

        if (player.nivelOxigenio < 0 || player.vida < 1)
            tela_atual = TELA_GAMEOVER;

        if (player.nivelOxigenio > 1000)
            player.nivelOxigenio = 1000;
    /*
         E. Emanoel: Atualiza os peixes na tela
     */

    for (int p = 0; p < MAX_PEIXE; p++)
    {
        if (peixes[p].vivo)
        {
            peixes[p].x += peixes[p].dx;

            if (peixes[p].x <= 0 - LARGURA_PEIXE || peixes[p].x >= LARGURA + LARGURA_PEIXE)
            {
                peixes[p].vivo = 0;
            }
        }
    }

    /*
        E. Emanoel: Agora a colisaoPeixeTiro() também atualiza os tiros na tela (achei melhor assim)
    */

    colisaoPeixeTiro();
    colisaoPlayerPeixe();
}

void reset(void)
{
    iniciarPlayer();
    iniciarTiros();
    iniciarPeixes();
}

int main()
{
    /*
        E. Emanoel: Inicia os tiros para não dar Bug
        Não pode iniciar no loop, se não ele sempre teria tiros inativos
        Adicionamos a lib <time.h> e o srand(time(NULL))
        Garantimos que as sequencias de nascimento dos peixes não sejam iguais lá em "nascerPeixes()"
    */
    srand(time(NULL));

    // E. Emanoel: Inicia os tiros para não dar Bug
    // Não pode iniciar no loop, se não ele sempre teria tiros inativos
    /*
        Enzo Capitani: Joguei tudo dentro dessa funcao
    */
    reset();

    // Enzo Capitani: aqui indica a saida padrão do programa, que no caso a saída é o console
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Game loop
    while (1)
    {
        if (tela_atual == 0)
        {
            nascerPeixes();
            desenhaTelaInicial();
            updateTelaInicial();
            acoesTela(TELA_JOGO);
            Sleep(90);
        }

        if (tela_atual == 1)
        {
            acoesPlayer();
            acaoTiro();
            nascerPeixes();
            update();
            desenha_tela();
            Sleep(90);
        }

        if (tela_atual == 2)
        {
            desenhaTelaGameOver();
            acoesTela(TELA_INICIAL);
        }
    }

    return 0;
}