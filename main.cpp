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

char atualizarEditor(TpEditor *Ed, FILE *ptr, ListaGen **L)
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
			inserirCaractere(Ed, &*L, c);
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
		LimparConteudo(2, 26, 80, 26);
		gotoxy(3,26); printf("Salvo com sucesso!");
		Sleep(900);	
	}
	else
	{
		LimparConteudo(2, 26, 80, 26);
		gotoxy(3,26); printf("Erro ao salvar...");
		Sleep(900);
	}
	LimparConteudo(2, 26, 80, 26);
}

void imprimirEditor(TpEditor *Ed) 
{
    TpLinha *auxL = Ed->linhaNoTopo;
    TpLetra *auxC;
    int x, y=4;
    int negritoAtivo = 0; //0 = normal, 1 = negrito

    while(auxL != NULL && y<=24) 
	{
        auxC = auxL->letraInicio;
        x = 2;
        gotoxy(x, y);
        while(auxC != NULL && x <= 81)
		{
			if(auxC->letra == 21)
			{
				if(negritoAtivo)	
					negritoAtivo = 0;
				else
					negritoAtivo = 1;
					
				if (negritoAtivo)
                    textcolor(15);
                else
                    textcolor(7);
			}
			else
			{
				printf("%c", auxC->letra);
				x++;
			}
            auxC = auxC->prox;
        }
        auxL = auxL->prox;
        y++;
    }
    textcolor(7);
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
	if(i == 1)
		return str[0] - 48; //possui somente um caracter, entao devolve a conversão desse mesmo caracter
	else
	{
		soma += (str[0]-48)*10;
		soma += str[1]-48;
		return soma;
	}
}

void teclaAcaoNormal(TpEditor *Ed, char c, int *x, int *y, ListaGen **L)
{
	int resto;
	char del;
	TpLinha *linhaAntes;
	resto = Ed->qntdLinha%21;
    switch(c)
    {
        case 8: //backspace
        	if(Ed->cursor == NULL && Ed->atualL == Ed->linhaNoTopo && Ed->linhaNoTopo->ant != NULL){ //não deixa apagar se essa condição for atendida
			}
        	else
        	{
        		del = 0;
				excluirCaracter(Ed, &(*x), &(*y), del);
			}
            break;
        case 13: //enter
            if(resto)
            {
            	inserirNovaLinha(Ed);
            	(*y)++;
            	*x = 2;
        	}
            break;
        default: //caracteres imprimíveis
        	if(!ehUltimaLinha_e_LinhaCheia(resto, Ed->atualL->nmr))
        	{
        		if(c >= 32 && c <= 126)
	            {
	                linhaAntes = Ed->atualL;
	                inserirCaractere(Ed, &*L, c);
	                if(Ed->atualL != linhaAntes) 
	                {
	                    *x = 3;
	                    (*y)++;
	                }
	                else
	                    *x = Ed->pos + 2;
	            }
			}
            break;
    }
}

void exibirTextoFormatado(TpEditor *Ed) 
{
    TpLinha *auxL = Ed->inicioL;
    TpLetra *auxC;
    char palavra[81], ultimo;
    int p = 0; 
    int colunaAtual, yImpressao, limiteDireito, margemEsquerdaBase;
    int fimDoTexto = 0;
    int negritoAtivo = 0;
    int ehNovoParagrafo = 1;

    LimparConteudo(2, 4, 80, 24);
    yImpressao = 4;
    
    limiteDireito = 81 - Ed->recuoDir; 
    margemEsquerdaBase = 2 + Ed->recuoEsq;
    
    colunaAtual = 2 + Ed->primeiraLinha;

    while (auxL != NULL && fimDoTexto == 0) 
    {
        auxC = auxL->letraInicio;

        while (auxC != NULL && yImpressao <= 24) 
        {
            if (auxC->letra == 21) 
            {
                negritoAtivo = !negritoAtivo;
                if (negritoAtivo) textcolor(15);
                else textcolor(7);
            }
            else if (auxC->letra == ' ') 
            {
                if (p > 0) 
                {
                    palavra[p] = '\0';

                    if (colunaAtual + p > limiteDireito) 
                    {
                        yImpressao++;
                        colunaAtual = margemEsquerdaBase;
                    }
                    if (yImpressao <= 24) 
                    {
                        gotoxy(colunaAtual, yImpressao);
                        printf("%s ", palavra);
                        
						ultimo = palavra[p-1];
                        if (ultimo == '.' || ultimo == '!' || ultimo == '?') 
                        {
                            yImpressao++;
                            colunaAtual = 2 + Ed->primeiraLinha;
                        }
                        else 
                            colunaAtual += p + 1;
                    }
                    p = 0; 
                }
            }
            else 
                if (p < 80) palavra[p++] = auxC->letra;
            auxC = auxC->prox;
        }
        if (p > 0) 
        {
            palavra[p] = '\0';
            if (colunaAtual + p > limiteDireito) 
            {
                yImpressao++;
                colunaAtual = margemEsquerdaBase;
            }
            if (yImpressao <= 24) 
            {
                gotoxy(colunaAtual, yImpressao);
                printf("%s ", palavra);
                
                char ultimo = palavra[p-1];
                if (ultimo == '.' || ultimo == '!' || ultimo == '?') 
                {
                    yImpressao++;
                    colunaAtual = 2 + Ed->primeiraLinha;
                }
                else 
                    colunaAtual += p + 1;
            }
            p = 0;
        }
        auxL = auxL->prox;
        if (auxL == NULL || yImpressao > 24)
            fimDoTexto = 1;
    }
    textcolor(11); gotoxy(2, 26);
    printf("MODO VISUALIZACAO (Qualquer tecla para voltar)");
    textcolor(7); getch();

    LimparConteudo(2, 4, 80, 24);
	LimparConteudo(2, 26, 80, 26);
    imprimirEditor(Ed);
}

void teclaAcaoEspecial(TpEditor *Ed, char c, int *x, int *y, int modificado, ListaGen **L, int *pagina)
{
    TpLinha *LiAtual, *auxLin, *linhaAntes;
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
				LimparConteudo(3,26,47,26);
				if(s=='s')
					teclaAcaoEspecial(Ed,61,x,y,modificado, &*L, &*pagina); //salva o txt atual
			}
			//limpa depois de ter certeza que vai conseguir abrir com fopen
			gotoxy(3,24); printf("Ex: C:\\teste\\arquivo.txt");
			gotoxy(3,26); printf("Digite o caminho: ");
			fflush(stdin);
			gets(str);
			LimparConteudo(3,24,47,24);
			LimparConteudo(3,26,80,26);
			ptr = fopen(str, "r");
			if(ptr!=NULL)
			{
				limparEditor(Ed); //limpa o editor para ser alimentado com o que for aberto
				LimparConteudo(2,4,80,24);
				atualizarEditor(Ed, ptr, &*L);
				*y = Ed->qntdLinha+3;
				*x = Ed->pos+2;
				fclose(ptr);
			}
			else
			{
				gotoxy(3,26); printf("Caminho invalido ou Erro ao abrir o arquivo!");
				Sleep(1000);
				LimparConteudo(3,26,80,26);
			}	
            break;
        case 61: //F3
            salvarTexto(Ed);
            break;
        case 63: //F5
        	painelF5();
        	gotoxy(85,10); printf("Digite numeros! (ESC-sair)");
			numAux = digitarNumero(102, 4); //primeira linha
		    if(numAux != -1) 
		    {
		        Ed->primeiraLinha = numAux;
		        numAux = digitarNumero(102, 6); //recuo esquerdo
		        if (numAux != -1) 
		        {
		            Ed->recuoEsq = numAux;
		            numAux = digitarNumero(101, 8); //recuo direito
		            if (numAux != -1)
		                Ed->recuoDir = numAux;
		        }
		    }
			LimparConteudo(85, 10, 113, 10);
			if (numAux == -1) //vericica se cancelou com ESC em qualquer etapa
		    {
		        Ed->primeiraLinha = -1;
		        Ed->recuoDir = -1;
		        Ed->recuoEsq = -1;
		        gotoxy(85, 10); printf("Cancelado...");
		        Sleep(900);
		    }
			else
			{
				gotoxy(85, 10); printf("Configurado!");
		    	Sleep(900);
		        exibirTextoFormatado(Ed);
			}
			LimparConteudo(83, 1, 114, 11);
        	break;
        	
        case 68: //F10 - Negrito
        	ListaGen *l;
        	l = NULL;
            linhaAntes = Ed->atualL;
            inserirCaractere(Ed, &l, 21);
            if(Ed->atualL != linhaAntes) 
            {
                *x = 3;
                (*y)++;
            }
            else
                *x = Ed->pos + 2;
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
            if(Ed->atualL->ant != NULL && *y > 4) //a linha de cima existe e linha não é a primeira?
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
            if(Ed->atualL->prox != NULL && *y < 24) //a linha de baixo existe e linha não é a ultima?
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
        
        case 73: //page up
		    i = 0;
		    while(Ed->linhaNoTopo->ant != NULL && i < 21) 
			{
		        Ed->linhaNoTopo = Ed->linhaNoTopo->ant;
		        i++;
		    }
		    if(i > 0) 
			{
		        Ed->atualL = Ed->linhaNoTopo;
		        Ed->cursor = NULL;
		        Ed->pos = 0;
		        *x = 2;
		        *y = 4;
		        if(*pagina > 21)
		        	*pagina -= 21;
		    }
		    break;
        
        case 81: //page down
	    i = 0;
	    auxLin = Ed->linhaNoTopo;
	    while(auxLin->prox != NULL && i < 20) //conta quantas linhas existem até este topo
		{
	        auxLin = auxLin->prox;
	    	i++;
	    }
	    if(i == 20) 
	    {
	        if(auxLin->prox == NULL)
	        {
	            Ed->atualL = auxLin;
	            inserirNovaLinha(Ed);
	            Ed->linhaNoTopo = Ed->atualL;
	        }
	        else
	        {
	            Ed->linhaNoTopo = auxLin->prox;
	            Ed->atualL = Ed->linhaNoTopo;
	        }
	        Ed->cursor = NULL;
	        Ed->pos = 0;
	        *x = 2;
	        *y = 4;
	        *pagina+=21;
	    }
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

int buscarSugestao(ListaGen *L, char *incompleta, char *sugestao) 
{
    ListaGen *atual = L;
    int i, pos;
	
	i = 0;
    while (incompleta[i] != '\0' && atual != NULL) 
	{
        atual = procuraLetraNaLg(atual, incompleta[i]);
        if (atual != NULL) 
		{
            if (incompleta[i + 1] != '\0')
                atual = atual->prim;
            i++;
        }
    }
    if (atual == NULL || atual->prim == NULL)//palavra nao encontrada na lista gen
		return 0;

    strcpy(sugestao, incompleta);
    pos = i;
    atual = atual->prim;

    while (atual != NULL) 
	{
        if (atual->prox != NULL) //se tem um prox, quer dizer que não tem certeza
			return 0;
        sugestao[pos++] = atual->letra;
        if (atual->final == 't') 
		{
            sugestao[pos] = '\0';
            return 1; //encontrou uma palavra completa e unica
        }
        atual = atual->prim;
    }
    return 0;
}

void capturarPalavraIncompleta(TpEditor *Ed, char *palavra) 
{
    TpLetra *aux = Ed->cursor;
    char temp[80];
    int i, j;
    
	i=j=0;
    while(aux != NULL && ((aux->letra >= 'A' && aux->letra <= 'Z') || (aux->letra >= 'a' && aux->letra <= 'z') || aux->letra == 21))
	{
        if (aux->letra != 21)
            temp[i++] = aux->letra;
        aux = aux->ant;
    }
    temp[i] = '\0';
    
    while (i > 0) //inverte a palavra invertida e deixa em maiusculo
        palavra[j++] = toupper(temp[--i]);
    palavra[j] = '\0';
}

void executar()
{
	TpEditor *Editor = criarEditor();
	TpLinha *linhaAntes;
	ListaGen *palavras = NULL;
	char tecla, temSugestao;
	char palavraIncompleta[50], sugestao[50];
	int x,y, modificado, pagina, teste, i;
	
	temSugestao = 0;
	tecla = 0;
	pagina = 21;
	modificado=0;
	x=2; 
	y=4;
	while(tecla != 62) //F4
    {
    	PainelDados(pagina/21, x-1, Editor->atualL->id, Editor->qntdLinha);
    	gotoxy(x, y);
        tecla = getch(); 
        if(tecla == 13) //verifica se aceita sugestao, ou insere uma nova linha
        {
        	if(temSugestao) //aceita sugestao
        	{
        		i = strlen(palavraIncompleta);
        		while (sugestao[i] != '\0') 
				{
					linhaAntes = Editor->atualL;
	                inserirCaractere(Editor, &palavras, tolower(sugestao[i]));
	                if(Editor->atualL != linhaAntes) 
	                {
	                    x = 3;
	                    y++;
	                }
	                else
	                    x = Editor->pos + 2;
		            i++;
		        }
		        LimparConteudo(2, 26, 80, 26); //limpa o espaço de sugestão
		        temSugestao = 0;
			}
			else //apenas insere uma nova linha
				teclaAcaoNormal(Editor, tecla, &x, &y, &palavras);
		}
        else if(tecla == 0 || (unsigned char)tecla == 224) //"unsigned char" força o PC a ler 224 como 224, e nao como -32, para as setas funcionarem
        {
        	if(temSugestao)
        		LimparConteudo(2, 26, 80, 26); //limpa o espaço de sugestão
            tecla = getch(); 
            if(tecla == 73 || tecla == 81)
            	LimparConteudo(2, 4, 80, 24);
            teclaAcaoEspecial(Editor, tecla, &x, &y, modificado, &palavras, &pagina); //teclas especiais vem em dois códigos: 0/224 + codigo real
        }
        else if(tecla != 62)
        {
        	if(temSugestao)
        		LimparConteudo(2, 26, 80, 26); //limpa o espaço de sugestão
        	teclaAcaoNormal(Editor, tecla, &x, &y, &palavras);
        	capturarPalavraIncompleta(Editor, palavraIncompleta);
        	if(strlen(palavraIncompleta) >= 3 && buscarSugestao(palavras, palavraIncompleta, sugestao)) //so sugere se digitou mais de 3 letras, e se existe na lista gen
        	{
        		gotoxy(2, 26);
		        textcolor(5);
		        printf("Sugestao: %s (ENTER para aceitar)", sugestao);
		        textcolor(7);
		        temSugestao = 1;
			}
			else
				temSugestao = 0;
        	if(tecla == 8 || tecla == 81 || tecla == 73)
        		LimparConteudo(2, 4, 80, 24);
        	else
        		LimparConteudo(2, y, 80, 4 + (Editor->atualL->id - Editor->linhaNoTopo->id));
        	modificado=1;
		}
		LimparConteudo(83,23,98,26); //limpa o painel de dados
        imprimirEditor(Editor);
    }
}

int main()
{
	painelPrincipal();
	executar();
	gotoxy(1,27);
	return 0;
}
