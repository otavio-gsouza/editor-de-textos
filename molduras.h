void Moldura(int Ci, int Li, int Cf, int Lf, int CorT, int CorF);
void PainelPrincipal(void);
void LimparConteudo(int Ci, int Li, int Cf, int Lf);
void painelF5(void);
void PainelDados(int pagina, int coluna, int linha, int qntd_linha);

void LimparConteudo(int Ci, int Li, int Cf, int Lf)
{
	int i, j;
	textcolor(0);
	for(i = Ci; i < Cf + 1; i++)
		for(j = Li; j < Lf + 1; j++)
		{
			gotoxy(i, j);
			printf(" ");
		}
	textcolor(7);
	textbackground(0);
}

void Moldura(int Ci, int Li, int Cf, int Lf, int CorT, int CorF)
{	
	int i;
	textcolor(CorT);
	textbackground(CorF);
	gotoxy(Ci, Li); printf("%c", 201);
	gotoxy(Cf, Li); printf("%c", 187);
	gotoxy(Ci, Lf); printf("%c", 200);
	gotoxy(Cf, Lf); printf("%c", 188);
	for(i=Ci+1; i<Cf; i++)
	{
		gotoxy(i, Li); printf("%c", 205);
		gotoxy(i, Lf); printf("%c", 205);
	}
	for(i=Li+1; i<Lf; i++)
	{
		gotoxy(Ci, i); printf("%c", 186);
		gotoxy(Cf, i); printf("%c", 186);
	}
	textcolor(7);
	textbackground(0);
}

void linhaReta(int Ci, int Lin, int Cf, int CorT, int CorF)
{
	int i;
	textcolor(CorT);
	textbackground(CorF);
	for(i=Ci; i<Cf; i++)
	{
	    gotoxy(i, Lin);
	    printf("%c", 205);
	}
	textcolor(7);
	textbackground(0);
}

void painelF5(void)
{ 
	Moldura(83,1,114,11,7,0); //Moldura externa
	gotoxy(83, 3);  printf("%c", 204);
    gotoxy(114, 3); printf("%c", 185);
	linhaReta(84,3,114,7,0);
	gotoxy(85,2); printf("Configuracao dos paragrafos");
	gotoxy(86,4); printf("Primeira Linha: ");
	gotoxy(86,6); printf("Recuo Esquerdo: ");
	gotoxy(86,8); printf("Recuo Direito: ");
	gotoxy(83, 9);  printf("%c", 204);
    gotoxy(114, 9); printf("%c", 185);
	linhaReta(84,9,114,7,0);
}

void PainelDados(int pagina, int coluna, int linha, int qntd_linha)
{
	gotoxy(83, 23); printf("Linha: %d", linha);
	gotoxy(83,24); printf("Coluna: %d", coluna);
	gotoxy(83, 25); printf("Qtd_Linha: %d", qntd_linha);
	gotoxy(83, 26); printf("Pagina Atual: %d", pagina);
    Moldura(82,22,99,27,7,0);
}

void painelPrincipal(void)
{ 
    Moldura(1, 1, 81, 27, 7, 0); 

    gotoxy(5, 2);  printf("F2-Abrir");
    gotoxy(26, 2); printf("F3-Salvar");
    gotoxy(48, 2); printf("F4-Sair");
    gotoxy(70, 2); printf("F5-Exibir");
    
    gotoxy(1, 3);  printf("%c", 204);
    gotoxy(81, 3); printf("%c", 185);
    linhaReta(2, 3, 81, 7, 0);    

    gotoxy(1, 25);  printf("%c", 204);
    gotoxy(81, 25); printf("%c", 185);
    linhaReta(2, 25, 81, 7, 0);
}
