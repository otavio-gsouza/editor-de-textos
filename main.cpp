#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <conio2.h>

#include "molduras.h"
#include "editor.h"


char criaArqTexto(TpEditor *Ed, char *str)
{
	TpLinha *auxLin = Ed->inicioL;
	TpLetra *auxLet;
	FILE *ptr = fopen(str, "w");
	if(ptr==NULL)
		return 0;
	else
	{
		while(auxLin!=NULL)
		{
			auxLet = auxLin->letraInicio;
			while(auxLet!=NULL)
			{
				fputc(auxLet->letra, ptr);
				auxLet = auxLet->prox;
			}
			if(auxLin->prox!=NULL)
				fputc('\n',ptr);
			auxLin = auxLin->prox;
		}
	}
	fclose(ptr);
	return 1;
}

char atualizarEditor(TpEditor *Ed, FILE *ptr)
{
	char c;
	c = fgetc(ptr);
	while(!feof(ptr))
	{
		if(c == '\n')
		{
			inserirNovaLinha(Ed);
			Ed->qntdLinha++;
		}
		else
		{
			inserirCaractere(Ed, c);
		}
		c = fgetc(ptr);		
	}
	return 0;
}

void salvarTexto(TpEditor *Ed)
{
	char str[65];
	gotoxy(3,26); printf("Salvar como (ex: texto.txt): ");
	fflush(stdin);
	gets(str);
	if(criaArqTexto(Ed, str))
	{
		LimparConteudo(2, 26, 81, 26);
		gotoxy(3,26); printf("Salvo com sucesso!");
		Sleep(900);	
	}
	else
	{
		LimparConteudo(2, 26, 81, 26);
		gotoxy(3,26); printf("Erro ao salvar...");
		Sleep(900);
	}
	LimparConteudo(2, 26, 81, 26);
}

void imprimirEditor(TpEditor *Ed)
{
    TpLinha *auxLin = Ed->inicioL;
    TpLetra *auxLet;
    int x, y=4;

    while(auxLin != NULL && y <= 24)
    {
        auxLet = auxLin->letraInicio;
        x = 2;
        while(auxLet != NULL && x <= 81)
        {
            gotoxy(x, y);
            printf("%c", auxLet->letra);
            auxLet = auxLet->prox;
            x++;
        }
        auxLin = auxLin->prox;
        y++;
    }
}

int digitarNumero(int x, int y)
{
	char str[3];
	int i=0, finalizou=0, soma=0;
	char c;
	str[0] = '\0';
	
	while(!finalizou)
	{
		gotoxy(x+i, y);
		c = getch();
		if(c >= 48 && c <= 57 && i < 2) //eh numero e tem espaço?
		{
			str[i] = c;
			printf("%c", c);
			i++;
			str[i] = '\0';
		}
		else if(c == 8 && i > 0) //eh backspace e há oq apagar?
		{
			i--;
			str[i] = '\0';
			gotoxy(x+i,y); printf(" ");
		}
		else if ((c == 13 && i > 0) || c == 27) //eh enter e possui valor?
			finalizou = 1;
	}
	if(c==27)
		return -1;
	if (i == 1)
		return str[0] - 48; //possui somente um caracter, entao devolve a conversão desse mesmo caracter
	else
	{
		soma += (str[0]-48)*10;
		soma += str[1]-48;
		return soma;
	}
}

void teclaAcaoNormal(TpEditor *Ed, char c, int *x, int *y)
{
	char del;
	TpLinha *linhaAntes;
    switch(c)
    {
        case 8: //backspace
        	del = 0;
            excluirCaracter(Ed, &(*x), &(*y), del);
            break;
        case 13: //enter
            inserirNovaLinha(Ed);
            (*y)++;
            *x = 2;
            break;
        default: //caracteres imprimíveis
            if(c >= 32 && c <= 126)
            {
                linhaAntes = Ed->atualL;
                inserirCaractere(Ed, c);
                if(Ed->atualL != linhaAntes) 
                {
                    *x = 3;
                    (*y)++;
                }
                else
                    *x = Ed->pos + 2;
            }
            break;
    }
}

void teclaAcaoEspecial(TpEditor *Ed, char c, int *x, int *y, int modificado)
{
    TpLinha *LiAtual;
    TpLetra *letAux;
    int i, novaPos, numAux;
    char s, str[150], del;
    FILE *ptr;

    switch(c)
    {
        case 60: //F2 abrirTexto(Ed);
            if(modificado)
            {
            	gotoxy(3, 26); 
				printf("Deseja salvar antes de abrir um novo? (s/n): ");
            	do
            	{
            		gotoxy(48, 26); 
					s = getch();
				}while(s!='s' && s!='n');
				LimparConteudo(3,26,48,26);
				if(s=='s')
					teclaAcaoEspecial(Ed,61,x,y,modificado); //salva o txt atual
			}
			//limpa depois de ter certeza que vai conseguir abrir com fopen
			gotoxy(3,24); printf("Ex: C:\\teste\\arquivo.txt");
			gotoxy(3,26); printf("Digite o caminho: ");
			fflush(stdin);
			gets(str);
			LimparConteudo(3,24,48,24);
			LimparConteudo(3,26,81,26);
			ptr = fopen(str, "r");
			if(ptr!=NULL)
			{
				limparEditor(Ed); //limpa o editor para ser alimentado com o que for aberto
				LimparConteudo(2,4,81,Ed->qntdLinha);
				atualizarEditor(Ed, ptr);
				*y = Ed->qntdLinha+3;
				*x = Ed->pos+2;
				fclose(ptr);
			}
			else
			{
				gotoxy(3,26); printf("Caminho invalido ou Erro ao abrir o arquivo!");
				Sleep(1000);
				LimparConteudo(3,26,81,26);
			}	
            break;
        case 61: //F3
            salvarTexto(Ed);
            break;
        case 63: //F5
        	painelF5();
        	gotoxy(85,10); printf("Digite numeros! (ESC-sair)");
			Ed->primeiraLinha = digitarNumero(102,4);
			if(Ed->primeiraLinha!=-1)
			{
				Ed->recuoEsq = digitarNumero(102,6);
				if(Ed->recuoEsq!=-1)
					Ed->recuoDir = digitarNumero(101,8);
			}
			LimparConteudo(85, 10, 113, 10);
			if(Ed->primeiraLinha == -1 || Ed->recuoDir == -1 || Ed->recuoEsq == -1)
			{
				Ed->primeiraLinha = -1;
				Ed->recuoDir = -1;
				Ed->recuoEsq = -1;
				gotoxy(85, 10); printf("Saindo sem salvar...");
				Sleep(900);
			}
			else
			{
				gotoxy(85, 10); printf("Configurado!");
				Sleep(900);	
			}
			LimparConteudo(83, 1, 114, 11);
        	break;
        case 75: //esquerda
            if(Ed->cursor != NULL && *x > 2)
            {
                (*x)--;
                Ed->cursor = Ed->cursor->ant;
                Ed->pos--;
            }
            else if(Ed->atualL->ant != NULL)
            {
                Ed->atualL = Ed->atualL->ant;
                if(Ed->atualL->letraFim != NULL)
                {
                    Ed->cursor = Ed->atualL->letraFim;
                    Ed->pos = Ed->atualL->nmr;
                    *x = Ed->atualL->nmr + 2;
                    (*y)--;
                }
            }
            break;
        case 77: //direita
            if(Ed->cursor != NULL)
            {
                if(Ed->cursor->prox != NULL && *x < 81)
                {
                    (*x)++;
                    Ed->cursor = Ed->cursor->prox;
                    Ed->pos++;
                }
                else if(Ed->atualL->prox != NULL)
                {
                    Ed->atualL = Ed->atualL->prox;
                    Ed->cursor = NULL;
                    (*y)++;
                    *x = 2;
                    Ed->pos = 0;
                }
            }
            else if(Ed->atualL->letraInicio != NULL)
            {
                (*x)++;
                Ed->cursor = Ed->atualL->letraInicio;
                Ed->pos++;
            }
            break;

        case 72: //cima
            if(Ed->atualL->ant != NULL) //a linha de cima existe?
            {
                (*y)--;
                LiAtual = Ed->atualL; 
                Ed->atualL = LiAtual->ant;
                Ed->cursor = Ed->atualL->letraInicio;
                
                if(Ed->pos <= Ed->atualL->nmr) //o cursor esta numa posicao menor ou igual a linha anterior?
                	novaPos = Ed->pos; //se sim, novaPos recebe o mesmo valor de cursor atual
                else
                	novaPos = Ed->atualL->nmr; //se não, novaPos aponta para para a ultima letra
                if(novaPos == 0) //cursor estava antes da primeira letra?
					Ed->cursor = NULL;
                else 
                    for(i = 1; i < novaPos; i++) //caminha até a posição correta da linha de cima
                        Ed->cursor = Ed->cursor->prox;
                Ed->pos = novaPos; //atualiza nova posição
                *x = novaPos + 2; //visual
            }
            break;

        case 80: //baixo
            if(Ed->atualL->prox != NULL) //a linha de baixo existe?
            {
                (*y)++;
                LiAtual = Ed->atualL; //guarda a linha atual
                Ed->atualL = LiAtual->prox; //a linha atual passa a ser a proxima
                Ed->cursor = Ed->atualL->letraInicio;
                
                if(Ed->pos <= Ed->atualL->nmr) //o cursor esta numa posição menor ou igual a proxima linha?
                	novaPos = Ed->pos; //se sim, novaPos mantem a mesma posicao 
                else
                	novaPos = Ed->atualL->nmr; //se não, novaPos aponta para o ultimo caracter
                
                if(novaPos == 0) 
					Ed->cursor = NULL; //cursor estava antes da primeira letra
                else 
                    for(i = 1; i < novaPos; i++) //caminha até a posição correta da linha de baixo
                        Ed->cursor = Ed->cursor->prox;
                Ed->pos = novaPos; //atualiza nova posição
                *x = novaPos + 2; //visual
            }
            break;
        
        case 71: //Home
        	Ed->cursor = NULL;
        	Ed->pos = 0;
        	*x = 2;
        	break;
        
        case 79: //End
        	Ed->cursor = Ed->atualL->letraFim;
        	Ed->pos = Ed->atualL->nmr;
        	*x = Ed->pos+2;
        	break;
        
        case 73: //Page up
        	
        	break;
        
        case 81: //Page down
        	
		    break;
        	
        case 82: //insert
        	if(Ed->modoSobrescrita)
        		Ed->modoSobrescrita = 0;
        	else
        		Ed->modoSobrescrita = 1;
        	break;
        
        case 83: //del
        	del = 1;
        	excluirCaracter(Ed, &(*x), &(*y), del);
        	break;
    }
}

void executar()
{
	TpEditor *Editor = criarEditor();
	char tecla=0;
	int x,y, modificado=0;
	
	x=2;
	y=4;
	while(tecla != 62) //F4
    {
        gotoxy(x, y);
        tecla = getch(); 
        if(tecla == 0 || (unsigned char)tecla == 224) //"unsigned char" força o PC a ler 224 como 224, e nao como -32, para as setas funcionarem
        {
            tecla = getch(); 
            teclaAcaoEspecial(Editor, tecla, &x, &y, modificado); //teclas especiais vem em dois códigos: 0/224 + codigo real
        }
        else if(tecla != 62)
        {
        	teclaAcaoNormal(Editor, tecla, &x, &y);
        	modificado=1;
		}
        LimparConteudo(2, 4, 81, Editor->qntdLinha+3); 
        imprimirEditor(Editor);
    }
	imprimirEditor(Editor);
}

int main()
{
	painel();
	executar();
	gotoxy(1,27);
	return 0;
}
