#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

// IMPORTANTE! o ponto 0 do eixo Y começa no topo, não em baixo

// Constantes do buffer
#define LARGURA 126
#define ALTURA 28

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
#define VELOCIDADE_ANIMACAO 6

/*
    Enzo Capitani: Sprites iniciais do submarino, ta uma bosta
*/
const char *PLAYER_ESQUERDA[TOTAL_FRAMES_JOGADOR][ALTURA_PLAYER] = {
    {    
        "  ()%% 1 ",
        "==%%%%%=/",
    },
    {
        "  ()%% 1 ",
        "==%%%%%=-",
    },
    {
        "  ()%% 1 ",
        "==%%%%%=\\",
    }
};

const char *PLAYER_DIREITA[TOTAL_FRAMES_JOGADOR][ALTURA_PLAYER] = {
    {    
        " 1 %%()  ",
        "\\=%%%%%==",
    },
    {    
        " 1 %%()  ",
        "-=%%%%%==",
    },
    {    
        " 1 %%()  ",
        "/=%%%%%==",
    }
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

#define TIRO_ICON 'O'
#define MAX_TIROS 10

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

#define ALTURA_PEIXE 1
#define LARGURA_PEIXE 3
#define MAX_PEIXE 5

// E. Emanoel: Sprites do peixe

const char *PEIXE_DIREITA[ALTURA_PEIXE] = {
    "><>"};

const char *PEIXE_ESQUERDA[ALTURA_PEIXE] = {
    "<><"};

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

void desenhaVida()
{
    char textoVida[15];
    sprintf(textoVida, "Vida: %d", player.vida);

    int inicio = LARGURA - 15;
    for(int i = 0; textoVida[i] != '\0'; i++){
        consoleBuffer[inicio + i].Char.AsciiChar = textoVida[i];
        consoleBuffer[inicio+i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN;
    }
}

void desenhaBarraOxigenio()
{
    /*
        Enzo Capitani: Aqui ele aloca a quantidade de '/' que representa o nivel de oxigenio
        eu encontrei esse loop ai, nao sei como fiz mas levei em base um codigo do Claude
        mas toda vez ele soma o i em 50 e verifica, se for maior que o nivel de oxigenio -> ' '
        se nao -> '/'
    */
    char barras[22];
    int indiceBarras = 0;
    for (int i = 0; i <= 1000; i += 50)
    {
        if (i < player.nivelOxigenio)
        {
            barras[indiceBarras] = '/';
            indiceBarras++;
            continue;
        }
        barras[indiceBarras] = ' ';
        indiceBarras++;
    }
    barras[21] = '\0';

    char barraOxigenio[51];
    sprintf(barraOxigenio, "OXIGENIO: [%s]", barras);

    int inicio = LARGURA * ALTURA - LARGURA + 40;
    for (int i = 0; barraOxigenio[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = barraOxigenio[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN;
    }
}

/*
        Enzo Capiani: Esse loop preenche os caracteres da agua e define as cores,
        esse atributte tem q deixar as 3 cores primarias (eu acho que sao), se remover
        um ele cria uma mistura, as cores são em hexadecimal
        Enzo Capitani: Foi adicionada a criação da "caixa" do mapa
    */
void desenhaMapa(){
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
    */
   
   int frameAtualPlayer = (relogioGlobal / VELOCIDADE_ANIMACAO) % TOTAL_FRAMES_JOGADOR;
   
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
          if(posX >= 0 && posX < LARGURA && posY >= 0 && posY < ALTURA){
              int indice = posY * LARGURA + posX;
              
                char caractere = PLAYER_SPRITE[frameAtualPlayer][i][j];
                
                if (caractere != ' '){
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
    
    /*
    E. Emanoel: Desenha os peixes na tela e vê se já tão fora do mapa
    */
   
   for (int p = 0; p < MAX_PEIXE; p++) // Int P são os peixes criados
   {
        if (peixes[p].vivo) // Verifica se o peixe está vivo | Se ele está morto, nem executa
        {
            const char **PEIXE_SPRITE = (peixes[p].dx == 1) ? PEIXE_DIREITA : PEIXE_ESQUERDA; // sprite pra direção que o peixe tá indo
            for (int i = 0; i < ALTURA_PEIXE
            ; i++)
            {
                for (int j = 0; j < LARGURA_PEIXE; j++)
                {
                    
                    if (peixes[p].x + j > 0 && peixes[p].x + j < LARGURA)
                    {
                        // Nesta fórmula, somamos o i e o j para que o peixe venha inteiro
                        int indice_peixe = ((peixes[p].y + i) * LARGURA) + (peixes[p].x + j);
                        consoleBuffer[indice_peixe].Char.AsciiChar = PEIXE_SPRITE[i][j];
                        consoleBuffer[indice_peixe].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
                    }
                }
            }
        }
    }
    
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
        if(player.x < LARGURA - LARGURA_PLAYER) player.x += VELOCIDADE_X;
        if(PLAYER_SPRITE != PLAYER_DIREITA){
            PLAYER_SPRITE = PLAYER_DIREITA;
            player.frameAtual = 0;
        }
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        if(player.x > 0) player.x -= VELOCIDADE_X; 
        if(PLAYER_SPRITE != PLAYER_ESQUERDA){
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
    player.x = 63-LARGURA_PLAYER;
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
    // Aqui a gente adiciona uma chance de 10% do peixe nascer a cada frame
    // Evitando que nasça um monte de peixe de uma vez
    if (rand() % 10 != 0)
        return;

    for (int i = 0; i < MAX_PEIXE; i++)
    {
        if (!peixes[i].vivo)
        {
            peixes[i].vivo = 1;

            // Sorteia um lado para o peixe nascer
            // Se for 1, vai nascer na esquerda | se for 0, vai nascer na direita
            int NASCE_ESQUERDA = (rand() % 2 == 0);

            if (NASCE_ESQUERDA)
            {
                // Aqui a gente faz o peixe nascer dentro da área segura de spawn, sem ser 0, se não ele nasce e morre
                peixes[i].x = 1 + LARGURA_PEIXE;
                // Direção do peixe | Indo pra direita
                peixes[i].dx = 1;
            }
            else
            {
                // Mesma coisa
                peixes[i].x = LARGURA - LARGURA_PEIXE;
                peixes[i].dx = -1;
            }

            // Aqui é um spawn aleatório do peixe nas colunas | 5 abaixo da superfície e 5 acima do chão
            peixes[i].y = (rand() % (ALTURA - 10)) + 5;
            break;
        }
    }
}

/*

    E. Emanoel: Adicionei uma função de colisão entre os peixes e os tiros
    ela verifica se o tiro tá ativo, se o peixe tá vivo e se eles colidiram,
    se colidiram, o peixe morre e o tiro é desativado

*/

void colisaoPeixeTiro()
{
    for (int t = 0; t < MAX_TIROS; t++)
    {
        if (tiros[t].ativo)
        {
            for (int p = 0; p < MAX_PEIXE; p++)
            {
                if (peixes[p].vivo)
                {
                    if (peixes[p].x < tiros[t].x + 1 &&
                        peixes[p].x + LARGURA_PEIXE > tiros[t].x &&
                        peixes[p].y < tiros[t].y + 1 &&
                        peixes[p].y + ALTURA_PEIXE
                     > tiros[t].y)
                    {
                        peixes[p].vivo = 0;
                        tiros[t].ativo = 0;
                    }
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

void colisaoPlayerPeixe(){
    for(int i = 0; i < MAX_PEIXE; i++)
    {
        if(player.x + LARGURA_PLAYER > peixes[i].x
        && player.x < peixes[i].x + LARGURA_PEIXE
        && player.y + ALTURA_PLAYER > peixes[i].y
        && player.y < peixes[i].y + ALTURA_PEIXE
        ){
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

    if(relogioGlobal % 3 == 0){
        player.frameAtual += (player.frameAtual + 1) % TOTAL_FRAMES_JOGADOR;
    }

    // Enzo Capitani: Aqui verifica, se o player estiver na superfice, aumenta o ocigenio, se nao diminui
    if (player.y == 1)
    {
        player.nivelOxigenio += 20;
    }
    else
    {
        player.nivelOxigenio -= 5;
    }

    if (player.nivelOxigenio < 0)
        player.nivelOxigenio = 0;

    /*
        E. Emanoel: Atualiza os tiros na tela
    */

    for (int i = 0; i < MAX_TIROS; i++)
    {
        if (tiros[i].ativo)
        {
            tiros[i].x += (tiros[i].dx * VELOCIDADE_X) * 2;

            if (tiros[i].x <= 0 || tiros[i].x >= LARGURA)
            {
                tiros[i].ativo = 0;
            }
        }
    }

    /*
        E. Emanoel: Atualiza os peixes na tela
    */

    for (int i = 0; i < MAX_PEIXE; i++)
    {
        if (peixes[i].vivo)
        {
            peixes[i].x += (peixes[i].dx * VELOCIDADE_X);

            if (peixes[i].x <= 0 || peixes[i].x >= LARGURA)
            {
                peixes[i].vivo = 0;
            }
        }
    }
    colisaoPeixeTiro();
    colisaoPlayerPeixe();
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
    iniciarPlayer();
    iniciarTiros();
    iniciarPeixes();

    // Enzo Capitani: aqui indica a saida padrão do programa, que no caso a saída é o console
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Game loop
    while (1)
    {
        acoesPlayer();
        acaoTiro();
        nascerPeixes();
        
        update();
        desenha_tela();
        Sleep(90);
    }

    return 0;
}