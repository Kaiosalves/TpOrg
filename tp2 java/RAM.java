package bcc266TP2.toy;

import java.util.Random;

public class RAM {
	
	private BlocoMemoria[] memoria;
			
	void criarRAM_vazia(int tamanho) {
		memoria = new BlocoMemoria[tamanho];
		for(int i=0; i<tamanho; i++) {
			memoria[i] = new BlocoMemoria();
			memoria[i].endBloco = i;
			for(int j=0; j<4; j++)
				memoria[i].palavras[j] =0;
		}
	}
	
	void criarRAM_aleatoria(int tamanho) {
		memoria = new BlocoMemoria[tamanho];
		Random r = new Random();
		for(int i=0; i<tamanho; i++) {
			memoria[i] = new BlocoMemoria();
			memoria[i].endBloco = i;
			for(int j=0; j<4; j++)
				memoria[i].palavras[j] =r.nextInt();
		}
	}
		
	void setDado(int endereco, BlocoMemoria conteudo) {
		memoria[endereco] = conteudo;
	}
	
	BlocoMemoria getDado(int endereco) {
		return memoria[endereco];
	}
	
	void imprimir() {
		System.out.println("Conteudo da RAM");
		for(int i=0; i<memoria.length; i++)
			imprimirUmBloco(memoria[i]);
		System.out.println();
	}
	
	void imprimirUmBloco(BlocoMemoria bloco) {
		for(int i=0; i<bloco.palavras.length; i++)
			System.out.print(bloco.palavras[i] + ",");
	}

}
