#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cgi.h"
#include "estado.h"


#define MAX_BUFFER		10240
#define TAM				10
#define ESCALA			60

int posicao_valida(int x, int y) {

	return x >= 0 && y >= 0 && x < TAM && y < TAM;

}

void imprime_mapa(){

	IMAGEM(0, 0, 600, "prisao.png");

}

void imprime_casa(int x, int y) {

	char *cor[] = {"rgba(0,0,0,0.1)", "rgba(0,0,0,0.1)"};
	int idx = (x + y) % 2;
	QUADRADO(x, y,ESCALA, cor[idx]);

}

int posicao_igual(POSICAO p, int x, int y){

	return p.x == x && p.y == y;

}

int tem_jogador(ESTADO e, int x, int y){

	return posicao_igual(e.jog, x , y);

}

int tem_inimigo(ESTADO e, int x, int y){
	int i;

	for(i=0; i<e.num_inimigos; i++)
		if (posicao_igual(e.inimigo[i], x, y))
			return 1;
	
	return 0;
}

int tem_obstaculo(ESTADO e, int x, int y){
	int i;
	
	for(i=0; i<e.num_obstaculos; i++)
		if (posicao_igual(e.obstaculo[i], x, y))
			return 1;
	
	return 0;
}

int posicao_ocupada(ESTADO e, int x, int y){

	return tem_jogador(e, x, y) || tem_inimigo(e, x, y) || tem_obstaculo(e, x, y);

}

int chegou_a_saida(ESTADO e){

	return e.jog.x == e.saida.x && e.jog.y == e.saida.y;

}

ESTADO inicializar_inimigo(ESTADO e){
	int x, y;
	do {
		x = random() % TAM;
		y = random() % TAM;
	} while(posicao_ocupada(e, x, y));

	e.inimigo[(int)e.num_inimigos].x = x;
	e.inimigo[(int)e.num_inimigos].y = y;

	e.num_inimigos++;
	return e;

}

ESTADO inicializar_inimigos(ESTADO e, int num){
	int i;

	for (i=0; i < num; i++)
		e = inicializar_inimigo(e);
	
	return e;
}

ESTADO inicializar_obstaculo(ESTADO e){
	int x, y;
	
	do {
		x = random() % TAM;
		y = random() % TAM;
	} while(posicao_ocupada(e, x, y));

	e.obstaculo[(int)e.num_obstaculos].x = x;
	e.obstaculo[(int)e.num_obstaculos].y = y;

	e.num_obstaculos++;
	return e;

}

ESTADO inicializar_saida(ESTADO e){
	int x, y;
	
	do {
		x = random() % TAM;
		y = random() % TAM;
	} while(posicao_ocupada(e, x, y));

	e.saida.x = x;
	e.saida.y = y;

	return e;

}

ESTADO inicializar_obstaculos(ESTADO e, int num){
	int i;
	for (i=0; i < num; i++)
		e = inicializar_obstaculo(e);
	return e;
}

ESTADO inicializar() {
	ESTADO e = {{0}};
	e.jog.x = random() % TAM;
	e.jog.y = random() % TAM;
	e = inicializar_inimigos(e, 20);

	e = inicializar_obstaculos(e, 30);

	e = inicializar_saida(e);
	
	return e;
}


ESTADO ler_estado(char *args) {
	if(strlen(args) == 0 || chegou_a_saida(str2estado(args)))
		return inicializar();

	return str2estado(args);
}


void imprime_movimento(ESTADO e, int dx, int dy) {
	ESTADO novo = e;
	int x = e.jog.x + dx;
	int y = e.jog.y + dy;
	char link[MAX_BUFFER];
	if(!posicao_valida(x, y))
		return;
	if(posicao_ocupada(e, x, y))
		return;


	novo.jog.x = x;
	novo.jog.y = y;
	sprintf(link, "http://localhost/cgi-bin/exemplo?%s", estado2str(novo));
	ABRIR_LINK(link);
	imprime_casa(x, y);
	FECHAR_LINK;
}

void imprime_movimentos(ESTADO e) {
	int dx, dy;

	for(dx = -1; dx <= 1; dx++)
		for(dy = -1; dy <= 1; dy++)
	 		imprime_movimento(e,  dx, dy);
	
}

void imprime_jogador(ESTADO e) {
	IMAGEM(e.jog.x, e.jog.y, ESCALA, "emoji2.png");
	imprime_movimentos(e);
}


void imprime_inimigos(ESTADO e) {
	int i;
	for(i = 0; i < e.num_inimigos; i++)
		IMAGEM(e.inimigo[i].x, e.inimigo[i].y, ESCALA, "tomoffinland.png");
}

void imprime_obstaculos(ESTADO e) {
	int i;

	for(i = 0; i < e.num_obstaculos; i++)
		IMAGEM(e.obstaculo[i].x, e.obstaculo[i].y, ESCALA, "8791b9d4efb4a980b8a1d6ded0792769.png");
}


void imprime_saida(ESTADO e) {
	IMAGEM(e.saida.x, e.saida.y, ESCALA, "colombia.png");


}

int main() {
	int x, y;
	
	srandom(time(NULL));
	ESTADO e = ler_estado(getenv("QUERY_STRING"));
	
	COMECAR_HTML;
	ABRIR_SVG(600, 600);
	imprime_mapa();
	for(y = 0; y < 10; y++)
		for(x = 0; x < 10; x++)
			imprime_casa(x, y);

	
	imprime_inimigos(e);
	imprime_obstaculos(e);
	imprime_saida(e);
	imprime_jogador(e);
	

	FECHAR_SVG;

	return 0;
}
