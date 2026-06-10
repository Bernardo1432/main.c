void ResetGraph(int mode);
void SetVideoMode(int mode);
void SetDefDrawEnv(void *env, int x, int y, int w, int h);
void SetDefDispEnv(void *env, int x, int y, int w, int h);
void PutDrawEnv(void *env);
void PutDispEnv(void *env);
void InitPAD(void *buf1, int len1, void *buf2, int len2);
void StartPAD(void);
void FntLoad(int tx, int ty);
int FntOpen(int x, int y, int w, int h, int is_bg, int max_char);
int FntFlush(int id);
int FntPrint(const char *fmt, ...);
void DrawSync(int mode);
int VSync(int mode);
int rsin(int a);
int rcos(int a);

#define PADLup      (1<<12)
#define PADLdown    (1<<14)
#define PADLleft    (1<<15)
#define PADLright   (1<<13)
#define PADRup      (1<<4)
#define PADRdown    (1<<6)
#define PADRleft    (1<<7)
#define PADRright   (1<<5)

#define VIDEO_LARGURA 320
#define VIDEO_ALTURA  240
#define MAPA_LARGURA 16
#define MAPA_ALTURA 16
#define TOTAL_CADERNOS 7

unsigned char db_draw[64];
unsigned char db_disp[64];
int db_atual = 0;
unsigned char buffer_controle[34]; 

int idioma = 0; 
int telaAtual = 0; 
int posicaoMenu = 0;

int playerX = 8 << 12; int playerY = 4 << 12; int playerA = 0; 
int cadernosColetados = 0; int stamina = 100; int estaCorrendo = 0;
int frameContador = 0; int minutos = 0; int segundos = 0;
int temBsoda = 0; int temChocolate = 0;

int baldiX = 2 << 12; int baldiY = 2 << 12; int baldiVel = 0;

char* textosMenu[3][4] = {
    {"Iniciar Jogo", "Configuracoes", "Creditos", "Sair Jogo"},
    {"Start Game", "Options", "Credits", "Exit Game"},
    {"Iniciar Juego", "Configuraciones", "Creditos", "Salir Juego"}
};
char* textosOptions[3][2] = {
    {"Idioma: Portugues", "Voltar"}, {"Language: English", "Back"}, {"Idioma: Espanol", "Volver"}
};

char mapa[MAPA_ALTURA][MAPA_LARGURA] = {
    "1111111111111111", "1C000100001000C1", "1000010000100001", "1110110000110111",
    "1000000000000001", "1011111001111101", "1010001001000101", "1000000000000001",
    "1010001001000101", "1011111001111101", "1000000000000001", "1110110000110111",
    "1000010000100001", "1C000100001000C1", "1111111111111111"
};

void InicializarSistemaPS1() {
    SetVideoMode(0); 
    ResetGraph(0);
    SetDefDrawEnv(db_draw, 0, 0, VIDEO_LARGURA, VIDEO_ALTURA);
    SetDefDispEnv(db_disp, 0, 0, VIDEO_LARGURA, VIDEO_ALTURA);
    PutDrawEnv(db_draw); 
    PutDispEnv(db_disp);
    InitPAD(buffer_controle, 34, buffer_controle, 34); 
    StartPAD();
    FntLoad(960, 256); 
    FntOpen(8, 8, 304, 224, 0, 1024);
}

void InverterBuffersPS1() {
    DrawSync(0); 
    VSync(1); 
}

void ResetarPartida() {
    playerX = 8 << 12; playerY = 4 << 12; playerA = 0;
    baldiX = 2 << 12; baldiY = 2 << 12;
    cadernosColetados = 0; stamina = 100; minutos = 0; segundos = 0; frameContador = 0;
    temBsoda = 0; temChocolate = 0;
}

int main() {
    int direcionalTrava = 0; int botaoTrava = 0;
    InicializarSistemaPS1();
    
    while (1) {
        FntFlush(-1); 
        unsigned short pad = ~((unsigned short*)(buffer_controle))[1];
        
        if (telaAtual == 0 || telaAtual == 1) {
            FntPrint("     BALDI'S BASICS - PSX MENU     \n");
            FntPrint("===================================\n\n");
            int limite = (telaAtual == 0) ? 4 : 2;
            int i;
            if (telaAtual == 0) {
                for(i=0; i<4; i++) FntPrint(i==posicaoMenu ? " -> [ %s ] <-\n" : "    %s\n", textosMenu[idioma][i]);
            } else {
                for(i=0; i<2; i++) FntPrint(i==posicaoMenu ? " -> [ %s ] <-\n" : "    %s\n", textosOptions[idioma][i]);
            }
            if (!direcionalTrava) {
                if (pad & PADLup) { posicaoMenu = (posicaoMenu - 1 + limite) % limite; direcionalTrava = 1; }
                if (pad & PADLdown) { posicaoMenu = (posicaoMenu + 1) % limite; direcionalTrava = 1; }
            }
            if (!(pad & (PADLup | PADLdown))) direcionalTrava = 0;
            if (!botaoTrava && (pad & PADRdown)) { 
                botaoTrava = 1;
                if (telaAtual == 0) {
                    if (posicaoMenu == 0) { telaAtual = 3; ResetarPartida(); }
                    else if (posicaoMenu == 1) { telaAtual = 1; posicaoMenu = 0; }
                    else if (posicaoMenu == 2) telaAtual = 2;
                    else if (posicaoMenu == 3) ResetGraph(3);
                } else {
                    if (posicaoMenu == 0) idioma = (idioma + 1) % 3;
                    else { telaAtual = 0; posicaoMenu = 1; }
                }
            }
            if (!botaoTrava && (pad & PADRup)) { botaoTrava = 1; if (telaAtual == 1) { telaAtual = 0; posicaoMenu = 0; } }
            if (!(pad & (PADRdown | PADRup))) botaoTrava = 0;
        }
        else if (telaAtual == 2) {
            FntPrint("     BALDI'S BASICS - CREDITS      \n");
            FntPrint("===================================\n\n");
            FntPrint(idioma == 0 ? " Criado para rodar no Game Stick\n" : " Made for Game Stick PS1!\n");
            FntPrint("\n Pressione TRIANGULO para voltar...");
            if (pad & PADRup) { telaAtual = 0; posicaoMenu = 2; }
        }
        else if (telaAtual == 3) {
            frameContador++;
            if (frameContador >= 60) { frameContador = 0; segundos++; if (segundos >= 60) { segundos = 0; minutos++; } }
            
            baldiVel++;
            if (baldiVel >= (8 - cadernosColetados)) {
                baldiVel = 0;
                if (baldiX < playerX && mapa[baldiY >> 12][(baldiX + (1 << 12)) >> 12] != '1') baldiX += (1 << 12);
                else if (baldiX > playerX && mapa[baldiY >> 12][(baldiX - (1 << 12)) >> 12] != '1') baldiX -= (1 << 12);
                if (baldiY < playerY && mapa[(baldiY + (1 << 12)) >> 12][baldiX >> 12] != '1') baldiY += (1 << 12);
                else if (baldiY > playerY && mapa[(baldiY - (1 << 12)) >> 12][baldiX >> 12] != '1') baldiY -= (1 << 12);
            }
            
            if ((playerX >> 12) == (baldiX >> 12) && (playerY >> 12) == (baldiY >> 12)) { telaAtual = 4; }
            if (cadernosColetados >= TOTAL_CADERNOS && ((playerX >> 12) < 2 || (playerX >> 12) > 14 || (playerY >> 12) < 2 || (playerY >> 12) > 14)) { telaAtual = 5; }
            
            if ((pad & PADRright) && stamina > 5) { estaCorrendo = 1; stamina -= 2; } 
            else { estaCorrendo = 0; if (stamina < 100) stamina++; }
            
            int velFisica = estaCorrendo ? 256 : 128; 
            if (pad & PADLleft)  playerA = (playerA - 64) & 4095;
            if (pad & PADLright) playerA = (playerA + 64) & 4095;
            
            if (pad & PADLup) {
                int nx = playerX + ((rsin(playerA) * velFisica) >> 12); 
                int ny = playerY + ((rcos(playerA) * velFisica) >> 12);
                if (mapa[ny >> 12][nx >> 12] != '1') { playerX = nx; playerY = ny; }
            }
            if (pad & PADLdown) {
                int nx = playerX - ((rsin(playerA) * velFisica) >> 12); 
                int ny = playerY - ((rcos(playerA) * velFisica) >> 12);
                if (mapa[ny >> 12][nx >> 12] != '1') { playerX = nx; playerY = ny; }
            }
            
            if ((pad & PADRleft) && temBsoda) { temBsoda = 0; baldiX = 2 << 12; baldiY = 2 << 12; }
            if ((pad & PADRup) && temChocolate) { temChocolate = 0; stamina = 100; }
            
            char bloco = mapa[playerY >> 12][playerX >> 12];
            if (bloco == 'C') { mapa[playerY >> 12][playerX >> 12] = '0'; cadernosColetados++; }
            else if (bloco == 'B') { mapa[playerY >> 12][playerX >> 12] = '0'; temBsoda = 1; }
            else if (bloco == 'H') { mapa[playerY >> 12][playerX >> 12] = '0'; temChocolate = 1; }

            FntPrint("Cadernos: %d/%d  Stamina: %d%%\n", cadernosColetados, TOTAL_CADERNOS, stamina);
            FntPrint("Tempo: %02d:%02d\n", minutos, segundos);
        }
        else if (telaAtual == 4) {
            FntPrint("\n\n    GAME OVER! O BALDI TE PEGOU!    \n");
            FntPrint("===================================\n\n");
            FntPrint(" Pressione TRIANGULO para voltar ao Menu...");
            if (pad & PADRup) { telaAtual = 0; posicaoMenu = 0; }
        }
        else if (telaAtual == 5) {
            FntPrint("\n\n    VOCE ESCAPOU DA ESCOLA!!!      \n");
            FntPrint("===================================\n\n");
            FntPrint(" Pressione TRIANGULO para o Menu...");
            if (pad & PADRup) { telaAtual = 0; posicaoMenu = 0; }
        }

        InverterBuffersPS1();
    }
    return 0;
}
