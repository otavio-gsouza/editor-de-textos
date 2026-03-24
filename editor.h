struct tpletra
{
	char letra;
	struct tpletra *prox, *ant;
};
typedef struct tpletra TpLetra;

struct tplinha
{
	int nmr;
	struct tplinha *prox, *ant;
	struct tpletra *letraInicio, *letraFim;
};
typedef struct tplinha TpLinha;

struct tpeditor
{
	struct tplinha *inicioL, *fimL, *atualL;
	struct tpletra *cursor;
	int qntdLinha, pos, primeiraLinha, recuoEsq, recuoDir;
};
typedef struct tpeditor TpEditor;

TpLetra *criarLetra(char letra);
TpLinha *criarLinha();
TpEditor *criarEditor();
void inserirCaractere(TpEditor *Ed, char letra, int *x, int *y);
void inserirNovaLinha(TpEditor *Ed);
void excluirCaracter(TpEditor *Ed);
void apagarLinha(TpEditor *Ed, TpLinha *Lin);
void limparEditor(TpEditor *Ed);

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
	return nc;
}

TpEditor *criarEditor()
{
	TpEditor *nc = (TpEditor*)malloc(sizeof(TpEditor));
	nc->inicioL = nc->fimL = nc->atualL = criarLinha();
	nc->cursor = NULL;
	nc->qntdLinha = 1;
	nc->pos = 0;
	nc->primeiraLinha = nc->recuoEsq = nc->recuoDir = -1;
	return nc;
}

void inserirCaractere(TpEditor *Ed, char letra)
{
	char let;
	TpLetra *novaLetra = criarLetra(letra);
	TpLetra *auxLet;
	TpLinha *auxLin, *nova;
	
	if(Ed->atualL->letraInicio == NULL)// linha vazia
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
	
	auxLin = Ed->atualL;
	while(auxLin != NULL && auxLin->nmr>80)
	{
		auxLet = auxLin->letraFim; //auxLet aponta para a ultima letra da linha atual
		
		auxLin->letraFim = auxLet->ant; //atualiza a ultima letra da linha atual
		auxLin->letraFim->prox = NULL;
		auxLin->nmr--; //decrementa a quantidade de letras na linha atual
		
		auxLet->ant = NULL; //limpa a ultima letra para inserir
		if(auxLin->prox == NULL) //a proxima linha não existe?
		{
			nova = criarLinha();
            nova->ant = auxLin;
            auxLin->prox = nova;
            Ed->fimL = nova;
            Ed->qntdLinha++;
		}
        if(Ed->cursor == auxLet) //cursor está na ultima letra? se sim, ele cai
        {
            Ed->atualL = auxLin->prox;
            Ed->pos = 1;
        }
        //insere a ultima letra na proxima linha atualizando ant e prox 
        auxLet->prox = auxLin->prox->letraInicio; 
		if(auxLin->prox->letraInicio != NULL) //linha não vazia?
            auxLin->prox->letraInicio->ant = auxLet;
        else //linha vazia, só atualiza o fim
            auxLin->prox->letraFim = auxLet;

        auxLin->prox->letraInicio = auxLet; //atualiza novo inicio da proxima linha (letra que caiu)
        auxLin->prox->nmr++; //atualiza a quantidade de letras da proxima linha

        auxLin = auxLin->prox; //verifica se a proxima linha também estourou
    }
}

void inserirNovaLinha(TpEditor *Ed) 
{
	TpLetra *auxLet;
	TpLinha *auxLin = criarLinha();
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
	Ed->pos = 0;
	Ed->cursor = NULL;
}

void apagarLinha(TpEditor *Ed, TpLinha *Lin)
{
	if(Lin->ant!=NULL) //verifica se não é a primeira linha
	{
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
	}
}

void excluirCaracter(TpEditor *Ed, int *x, int *y)
{
	TpLetra *auxLet, *pedacoQueSobe;
	TpLinha *auxLin;
	int resto, i;
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
	else if(Ed->atualL->ant!=NULL) //não eh a primeira linha e cursor esta antes da primeira letra?
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
		else if(Ed->atualL->letraInicio == NULL) //linha atual está vazia?
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
		else if(Ed->atualL->ant->nmr == 80) //a linha anterior possui 80 caracteres
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
			resto = 80 - Ed->atualL->ant->nmr; //operação que calcula o restante de letras que ainda cabem na linha anterior
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
