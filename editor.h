struct tpletra
{
	char letra;
	struct tpletra *prox, *ant;
};
typedef struct tpletra TpLetra;

struct tplinha
{
	int nmr, id;
	struct tplinha *prox, *ant;
	struct tpletra *letraInicio, *letraFim;
};
typedef struct tplinha TpLinha;

struct tpeditor
{
	//geral
	struct tplinha *inicioL, *fimL, *atualL, *linhaNoTopo;
	struct tpletra *cursor;
	int qntdLinha, pos;
	//para f5
	int primeiraLinha, recuoEsq, recuoDir;
	//para insert
	int modoSobrescrita; //0 desativado, 1 ativado
};
typedef struct tpeditor TpEditor;

struct listagen
{
	char letra;
	char final;
	struct listagen *prim;
	struct listagen *prox;
};
typedef struct listagen ListaGen;

TpLetra *criarLetra(char letra);
TpLinha *criarLinha();
TpEditor *criarEditor();
void inserirCaractere(TpEditor *Ed, ListaGen **L, char letra);
void inserirNovaLinha(TpEditor *Ed);
void excluirCaracter(TpEditor *Ed);
void apagarLinha(TpEditor *Ed, TpLinha *Lin);
void limparEditor(TpEditor *Ed);
ListaGen *procuraLetraNaLg(ListaGen *L, char c);
ListaGen *criaLg(char c);
void inserirNaListaGen(ListaGen **L, char *palavra);
int ehUltimaLinha_e_LinhaCheia(int resto, int nmr);
void capturarPalavraAnterior(TpEditor *Ed, char *str);
void recalcularIds(TpEditor *Ed);

TpLetra *criarLetra(char letra)
{
	TpLetra *nc = (TpLetra*)malloc(sizeof(TpLetra));
	nc->letra = letra;
	nc->ant = nc->prox = NULL;
	return nc;
}

TpLinha *criarLinha()
{
	TpLinha *nc = (TpLinha*)malloc(sizeof(TpLinha));
	nc->ant = nc->prox = NULL;
	nc->letraFim = nc->letraInicio = NULL;
	nc->nmr = 0;
	nc->id = 0;
	return nc;
}

TpEditor *criarEditor()
{
	TpEditor *nc = (TpEditor*)malloc(sizeof(TpEditor));
	nc->inicioL = nc->fimL = nc->atualL = criarLinha();
	nc->atualL->id = 1;
	nc->cursor = NULL;
	nc->qntdLinha = 1;
	nc->pos = 0;
	nc->primeiraLinha = nc->recuoEsq = nc->recuoDir = -1;
	nc->modoSobrescrita = 0;
	nc->linhaNoTopo = nc->inicioL;
	return nc;
}

int ehUltimaLinha_e_LinhaCheia(int resto, int nmr)
{
	if(!resto)
		if(nmr == 79)
			return 1;	
	return 0;
}

void capturarPalavraAnterior(TpEditor *Ed, char *str)
{
	TpLetra *auxLet;
	char auxStr[80];
	char flag;
	int i, j;
	
	str[0]='\0';
	i=j=0;
	flag=1;
	auxLet = Ed->cursor->ant;
	
	while(auxLet!=NULL && flag)
	{
		if((auxLet->letra >= 'A' && auxLet->letra <= 'Z') || (auxLet->letra >= 'a' && auxLet->letra <= 'z'))
		{
			auxStr[i++] = auxLet->letra;
			auxLet = auxLet->ant;
		}
		else if (auxLet->letra == 21) //se for um caractere negrito, ignora
            auxLet = auxLet->ant;
		else
			flag=0;
	}
	auxStr[i] = '\0';
	while(i>0) //inverter a string
	{
		i--;
		str[j] = toupper(auxStr[i]);
		j++;
	}
	str[j] = '\0';
}

void recalcularIds(TpEditor *Ed) 
{
    TpLinha *aux = Ed->inicioL;
    int contador = 1;

    while (aux != NULL) 
	{
        aux->id = contador;
        contador++;
        aux = aux->prox;
    }
}

void inserirCaractere(TpEditor *Ed, ListaGen **L, char letra)
{
	char let;
	char palavraParaAprender[80];
	TpLetra *novaLetra;
	TpLetra *auxLet = NULL;
	TpLinha *auxLin, *nova;
	
	if(!Ed->modoSobrescrita)
	{
		novaLetra = criarLetra(letra);
		if(Ed->atualL->letraInicio == NULL) // linha vazia
			Ed->atualL->letraInicio = Ed->atualL->letraFim = novaLetra;
		else if(Ed->cursor == NULL) //inserir antes da primeira letra
		{
			novaLetra->prox = Ed->atualL->letraInicio;
	        Ed->atualL->letraInicio->ant = novaLetra;
	        Ed->atualL->letraInicio = novaLetra;
		}
		else if(Ed->cursor == Ed->atualL->letraFim) //cursor no fim
		{
			Ed->atualL->letraFim->prox = novaLetra;
			novaLetra->ant = Ed->atualL->letraFim;
			Ed->atualL->letraFim = novaLetra;
		}
		else //cursor no meio
		{
			novaLetra->prox = Ed->cursor->prox;
			novaLetra->ant = Ed->cursor;
			Ed->cursor->prox->ant = novaLetra;
			Ed->cursor->prox = novaLetra;
		}
		Ed->cursor = novaLetra;
		Ed->atualL->nmr++;
		Ed->pos++;
		
		//se o caracter digitado não for uma letra, a palavra anterior acabou
		if (!((letra >= 'A' && letra <= 'Z') || (letra >= 'a' && letra <= 'z')))
		{
            capturarPalavraAnterior(Ed, palavraParaAprender);
            if (strlen(palavraParaAprender) > 2) //sí insere na listagen se a palavra tiver mais de dois caracteres
                inserirNaListaGen(&*L, palavraParaAprender);
        }
		
		auxLin = Ed->atualL;
		if(auxLin->nmr>79)
		{
			auxLet = auxLin->letraFim; //auxLet aponta para a ultima letra da linha atual
			
			auxLin->letraFim = auxLet->ant; //atualiza a ultima letra da linha atual
			auxLin->letraFim->prox = NULL;
			auxLin->nmr--; //decrementa a quantidade de letras na linha atual
			
			auxLet->ant = NULL; //limpa a ultima letra para inserir
			
			nova = criarLinha();
	        nova->ant = auxLin;
			if(auxLin->prox == NULL) //a proxima linha não existe?
			{
	            auxLin->prox = nova;
	            Ed->fimL = nova;
	            Ed->qntdLinha++;
			}
			else //a proxima linha existe, então remaneja
			{
				auxLin->prox->ant = nova;
				nova->prox = auxLin->prox;
				auxLin->prox = nova;
			}
	        if(Ed->cursor == auxLet) //cursor está na ultima letra? se sim, ele cai
	        {
	            Ed->atualL = auxLin->prox;
	            Ed->pos = 1;
	        }
	        //insere a ultima letra na proxima linha atualizando ant e prox 
	        nova->letraInicio = nova->letraFim = auxLet;
	        nova->nmr++;
	        recalcularIds(Ed);
	    }
	}
	else
	{
		if(Ed->atualL != NULL)
		{
			if(Ed->cursor == NULL)
				auxLet = Ed->atualL->letraInicio;
			else if(Ed->cursor != Ed->atualL->letraFim)
				auxLet = Ed->cursor->prox;
			if(auxLet != NULL)
			{
				auxLet->letra = letra;
				Ed->cursor = auxLet;	
				Ed->pos++;
			}
			else
	        {
	            Ed->modoSobrescrita = 0; 
	            inserirCaractere(Ed, &*L, letra); 
	            Ed->modoSobrescrita = 1; 
	        }	
		}
	}
}

void inserirNovaLinha(TpEditor *Ed) 
{
	TpLetra *auxLet;
	TpLinha *auxLin = criarLinha();
	auxLin->id = Ed->qntdLinha+1;
	if(Ed->cursor==NULL) //cursor esta atras da primeira letra, então cria uma linha atrás dessa linha
	{
		//tudo que está na atual passa a ser da nova linha
		auxLin->letraInicio = Ed->atualL->letraInicio;
		auxLin->letraFim = Ed->atualL->letraFim;
		auxLin->nmr = Ed->atualL->nmr;
		//a linha atual é zerada
		Ed->atualL->letraInicio = Ed->atualL->letraFim = NULL;
		Ed->atualL->nmr = 0;
	}
	else if(Ed->cursor->prox==NULL) //cursor está na ultima letra da linha atual
		auxLin->nmr = 0;
	else //cursor está entre letras, deve-se quebrar a linha
	{
		auxLet = Ed->cursor->prox; //auxLet salva o novo começo da linha que vai descer
		
		auxLin->letraInicio = auxLet;
		auxLin->letraFim = Ed->atualL->letraFim;
		auxLin->nmr = Ed->atualL->nmr - Ed->pos; //atualiza quantidade de letras da nova linha
		
		auxLet->ant = NULL; //limpa a letra antes dela ser a nova primeira letra da linha de baixo
		Ed->cursor->prox = NULL;
		Ed->atualL->letraFim = Ed->cursor;
		Ed->atualL->letraFim->prox = NULL;
		Ed->atualL->nmr = Ed->pos; //atualiza quantidade de letras da linha atual
	}
	
	auxLin->prox = Ed->atualL->prox;
	auxLin->ant = Ed->atualL;
	
	if(Ed->atualL->prox!=NULL)
		Ed->atualL->prox->ant = auxLin;
	else
		Ed->fimL = auxLin;
	Ed->atualL->prox = auxLin;
	
	
	Ed->atualL = auxLin;
	Ed->qntdLinha++;
	recalcularIds(Ed);
	Ed->pos = 0;
	Ed->cursor = NULL;
}

ListaGen *criaLg(char c)
{
	ListaGen *nc = (ListaGen*)malloc(sizeof(ListaGen));
	nc->final = 'f';
	nc->letra = c;
	nc->prim = nc->prox = NULL;
	return nc;
}

ListaGen *procuraLetraNaLg(ListaGen *L, char c)
{
	ListaGen *aux = L;
    while(aux != NULL && c > aux->letra)
        aux = aux->prox;
        
    if(aux != NULL && aux->letra == c)
        return aux;
    return NULL;
}

void inserirNaListaGen(ListaGen **L, char *palavra)
{
	char flag;
	int i;
	ListaGen *achou, *atual, *nova;
	
	i = 0;
	while(palavra[i] != '\0')
	{
		if(i == 0)
			achou = procuraLetraNaLg(*L, palavra[i]);
		else
			achou = procuraLetraNaLg(atual->prim, palavra[i]);
		
		if(achou == NULL)
		{
			nova = criaLg(palavra[i]);
			if(i == 0) //inserção na primeira camada
			{
				if(*L == NULL || (*L)->letra > nova->letra) //deve inserir na primeira posição
				{
					nova->prox = *L;
					*L = nova;
				}
				else //insere no meio ou fim
				{
					atual = *L;
				    while (atual->prox != NULL && atual->prox->letra < nova->letra)
				        atual = atual->prox;
				    nova->prox = atual->prox;
				    atual->prox = nova;
				}
				achou = nova;
			}
			else
			{
				if(atual->prim == NULL || nova->letra > atual->prim->letra)
				{
					nova->prox = atual->prim;
					atual->prim = nova;
				}
				else
				{
					atual = atual->prim;
					while (atual->prox != NULL && atual->prox->letra < nova->letra)
			            atual = atual->prox;
			        nova->prox = atual->prox;
			        atual->prox = nova;
				}
				achou = nova;
			}
		}
		
		if(palavra[i+1] == '\0')
			achou->final = 't';
		
		i++;
		atual = achou;
	}
}

void apagarLinha(TpEditor *Ed, TpLinha *Lin)
{
	TpLinha *aux;
	if(Lin->ant!=NULL) //verifica se não é a primeira linha
	{
		aux = Lin->prox;
		while(aux!=NULL)
		{
			aux->id--;
			aux = aux->prox;
		}
		if(Ed->atualL == Lin) //verificação de segurança
			Ed->atualL = Ed->atualL->ant;
		if(Lin->prox==NULL) //é a ultima linha?
		{
			Ed->fimL = Lin->ant;
			Ed->fimL->prox = NULL;
		}
		else //linha está no meio
		{
			Lin->ant->prox = Lin->prox;
			Lin->prox->ant = Lin->ant;
		}
		free(Lin);
		Ed->qntdLinha--;
		recalcularIds(Ed);
	}
}

void excluirCaracter(TpEditor *Ed, int *x, int *y, char del)
{
	TpLetra *auxLet, *pedacoQueSobe;
	TpLinha *auxLin;
	int resto, i;
	char flag = 0;
	if(!del)
	{
		if(Ed->cursor!=NULL)
		{
			auxLet = Ed->cursor;
			auxLin = Ed->atualL;
			if(auxLin->nmr==1) //linha so possui um caracter
			{
				auxLin->letraInicio = auxLin->letraFim = NULL;
				Ed->cursor = NULL;
			}
			else //linha possui mais de um caracter
			{
				if(auxLet == auxLin->letraFim) //cursor esta apontando para a ultima letra
				{
					Ed->cursor = auxLet->ant;
					Ed->cursor->prox = NULL;
					auxLin->letraFim = Ed->cursor;
				}
				else if(auxLet->ant == NULL) //cursor esta apontando para a primeira letra
				{
					Ed->cursor = NULL;
					Ed->atualL->letraInicio = Ed->atualL->letraInicio->prox;
					Ed->atualL->letraInicio->ant = NULL;
					if(Ed->atualL->letraInicio->prox==NULL) //verifica se só restara um caracter e atualiza o fim
						auxLin->letraFim = Ed->atualL->letraInicio;
				}
				else //cursor esta entre dois caracteres
				{
					Ed->cursor = auxLet->ant;
					Ed->cursor->prox = auxLet->prox;
					auxLet->prox->ant = Ed->cursor;
				}
			}
			Ed->pos--;
			auxLin->nmr--;
			auxLet->prox = auxLet->ant = NULL; //limpa o caracter isolado para exclui-lo
			free(auxLet);
			if(*x > 2)
				(*x)--;
		}
		else if(Ed->atualL->ant!=NULL) //não eh a primeira linha e cursor esta antes da primeira letra
		{
			if(Ed->atualL->ant->letraInicio == NULL) //linha anterior está vazia
			{
				Ed->atualL->ant->letraInicio = Ed->atualL->letraInicio;
				Ed->atualL->ant->letraFim = Ed->atualL->letraFim;
				Ed->atualL->ant->nmr = Ed->atualL->nmr;
				Ed->atualL = Ed->atualL->ant;
				//Ed->pos, Ed->cursor e X mantém os mesmos valores
				apagarLinha(Ed, Ed->atualL->prox);
			}
			else if(Ed->atualL->letraInicio == NULL) //linha atual está vazia
			{
				Ed->atualL = Ed->atualL->ant;
				Ed->cursor = Ed->atualL->letraFim;
				Ed->pos = Ed->atualL->nmr;
				*x = Ed->atualL->nmr + 2;
				apagarLinha(Ed, Ed->atualL->prox);
			}
			else if(Ed->atualL->nmr + Ed->atualL->ant->nmr <= 80) //união dá para ser feita na mesma linha, sem quebrar a linha atual (linha atual possui caracteres)
			{
				Ed->atualL->ant->letraFim->prox = Ed->atualL->letraInicio;
				Ed->atualL->letraInicio->ant = Ed->atualL->ant->letraFim;
				
				Ed->cursor = Ed->atualL->ant->letraFim;
				
	        	Ed->atualL->ant->letraFim = Ed->atualL->letraFim;
				Ed->atualL = Ed->atualL->ant;
				Ed->pos = Ed->atualL->nmr;
				*x = Ed->atualL->nmr+2;
				Ed->atualL->nmr += Ed->atualL->prox->nmr;
				
				apagarLinha(Ed, Ed->atualL->prox);
			}
			else if(Ed->atualL->ant->nmr == 79) //a linha anterior possui 80 caracteres
			{
				auxLet = Ed->atualL->ant->letraFim; //salva a ULTIMA letra da linha anterior para isolar e dar free
				
				auxLet->ant->prox = Ed->atualL->letraInicio; //a penultima letra da linha anterior aponta para a primeira letra da linha de baixo
				Ed->atualL->letraInicio->ant = auxLet->ant; //atualiza novo anterior
				Ed->atualL->ant->letraFim = Ed->atualL->letraInicio; //atualiza novo fim da linha anterior, isolando auxLet
				Ed->atualL->letraInicio = Ed->atualL->letraInicio->prox; //atualiza inicio da linha atual
				if(Ed->atualL->letraInicio!=NULL)
					Ed->atualL->letraInicio->ant = NULL; //atualiza ant da primieira letra da linha atual
				else
					Ed->atualL->letraFim = NULL; //so havia mais um caracter
				Ed->atualL->nmr--;
				Ed->atualL->ant->letraFim->prox = NULL; //atualiza prox da ultima letra da linha anterior
				Ed->atualL = Ed->atualL->ant;
				Ed->cursor = Ed->atualL->letraFim;
				Ed->pos = Ed->atualL->nmr;
				free(auxLet);
				*x = Ed->atualL->nmr+2;
			}
			else //união quebrada
			{
				resto = 79 - Ed->atualL->ant->nmr; //operação que calcula o restante de letras que ainda cabem na linha anterior
	            auxLet=Ed->atualL->letraInicio;
	            
	            pedacoQueSobe = Ed->atualL->letraInicio; //guarda inicio
	
	            for(i=1; i<resto; i++)
	                auxLet = auxLet->prox;
	
	            Ed->atualL->letraInicio = auxLet->prox; //atualiza a letra incial para cortar
	            Ed->atualL->letraInicio->ant = NULL;
	
	            Ed->atualL->ant->letraFim->prox = pedacoQueSobe; //faz os ligamentos
	            pedacoQueSobe->ant = Ed->atualL->ant->letraFim;
	
	            auxLet->prox = NULL; //limpa auxLet
	            Ed->atualL->ant->letraFim = auxLet; //atualiza fim da linha de cima
	            
	            Ed->pos = Ed->atualL->ant->nmr;
	            Ed->cursor = Ed->atualL->ant->letraFim;
	            Ed->atualL->ant->nmr += i; //soma nova quantidade de caracteres que subiram
	            Ed->atualL->nmr -= i; //subtrai a quantidade de caracteres que subiram para atualizar a linha de baixo
	
	            Ed->atualL = Ed->atualL->ant;
	            *x = Ed->pos+2;
			}
			(*y)--;
		}
	}
	else
	{
		if(Ed->cursor == NULL)
		{
			auxLet = Ed->atualL->letraInicio;
			if(auxLet!=NULL) //há algo para apagar a direita
			{
				if(auxLet->prox == NULL)
					Ed->atualL->letraInicio = Ed->atualL->letraFim = NULL;
				else
				{
					Ed->atualL->letraInicio = auxLet->prox; //atualiza novo inicio da linha
					Ed->atualL->letraInicio->ant = NULL;
				} 
			}
			else
				flag = 1;
		}
		else if(Ed->cursor->prox == Ed->atualL->letraFim) //a proxima letra é a ultima da linha
		{
			auxLin = Ed->atualL->prox;
			auxLet = Ed->cursor->prox;
			if(Ed->atualL->prox != NULL)
			{
				if(Ed->atualL->prox->letraInicio != NULL)
				{
					Ed->cursor->prox = auxLin->letraInicio;
					auxLin->letraInicio->ant = Ed->cursor;
					Ed->atualL->letraFim = auxLin->letraInicio;
					auxLin->letraInicio = auxLin->letraInicio->prox;
					if(auxLin->letraInicio != NULL)
						auxLin->letraInicio->ant = NULL;
					else
						auxLin->letraFim = NULL;
					Ed->atualL->nmr++;
				}
				else
				{
					Ed->cursor->prox = NULL;
					Ed->atualL->letraFim = Ed->cursor;
				}
			}
			else
			{
				Ed->cursor->prox = NULL;
				Ed->atualL->letraFim = Ed->cursor;
			}
		}
		else if(Ed->cursor->prox == NULL) //cursor esta na ultima letra da linha
		{
			if(Ed->atualL->nmr == 80) //linha atual ja esta cheia
			{
				if(Ed->atualL->prox != NULL) //proxima linha existe
				{
					if(Ed->atualL->prox->letraInicio != NULL) //proxima linha tem letra
					{
						Ed->atualL = Ed->atualL->prox;
						auxLet = Ed->atualL->letraInicio;
						Ed->atualL->letraInicio = auxLet->prox; //atualiza inicio da linha
						if(Ed->atualL->letraInicio != NULL)
							Ed->atualL->letraInicio->ant = NULL;
						Ed->cursor = NULL;
						Ed->pos = 0;
						*x = 2;
						(*y)++;
					}
					else
					{
						apagarLinha(Ed, Ed->atualL->prox); //apaga a proxima linha vazia
						flag = 1;
					}
				}
				else
					flag = 1;
			}
			else //linha atual nao esta cheia, então a linha debaixo tera que se juntar com a atual
			{
				flag = 1;
				if(Ed->atualL->prox != NULL) //proxima linha existe
				{
					auxLin = Ed->atualL->prox;
					if(Ed->atualL->nmr + auxLin->nmr <= 80) //união pode ser feita sem quebrar
					{
						if(auxLin->letraInicio!=NULL)
						{
							Ed->atualL->letraFim->prox = auxLin->letraInicio;
							auxLin->letraInicio->ant = Ed->atualL->letraFim;
							Ed->atualL->letraFim = auxLin->letraFim;
							Ed->atualL->nmr += auxLin->nmr;
						}
						apagarLinha(Ed, Ed->atualL->prox);
					}
					else //união deve ser quebrada
					{
						resto = 80 - Ed->atualL->nmr; //calcula o restante de caracteres que faltam para completar a linha atual (para subir a quantidade correta da linha debaixo)
						
						pedacoQueSobe = auxLin->letraInicio;
						auxLet = pedacoQueSobe;
						
						for(i=1; i<resto && auxLet != NULL;i++)
							auxLet = auxLet->prox;
						
						auxLin->letraInicio = auxLet->prox; //atualiza novo inicio da proxima linha
						auxLin->letraInicio->ant = NULL;
						
						Ed->atualL->letraFim->prox = pedacoQueSobe;
						pedacoQueSobe->ant = Ed->atualL->letraFim;
						Ed->atualL->letraFim = auxLet;
						auxLet->prox = NULL;
						Ed->atualL->nmr += i;
						auxLin->nmr -= i;
					}
				}
			}
		}
		else
		{
			auxLet = Ed->cursor->prox;
			Ed->cursor->prox = auxLet->prox;
			auxLet->prox->ant = Ed->cursor;
		}
		if(!flag)
		{
			Ed->atualL->nmr--;
			free(auxLet);
		}
	}
}

void limparEditor(TpEditor *Ed)
{
	TpLetra *antLet, *auxLet;
	TpLinha *antLin = Ed->inicioL, *proxLin;
	
	while(antLin!=NULL)
	{
		auxLet = antLin->letraFim;
		while(auxLet!=NULL)
		{
			antLet = auxLet->ant;
			free(auxLet);
			auxLet = antLet;
		}
		proxLin = antLin->prox;
		free(antLin);
		antLin = proxLin;
	}
	Ed->inicioL = Ed->fimL = Ed->atualL = criarLinha();
	Ed->cursor = NULL;
	Ed->primeiraLinha = Ed->recuoDir = Ed->recuoEsq = -1;
	Ed->qntdLinha = 1;
	Ed->pos = 0;
	Ed->atualL->nmr = 0;
}
