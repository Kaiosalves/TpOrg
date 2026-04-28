package bcc266TP1.toy;

import java.util.Random;

public class RAM {
	
	int[] memoria;
	
	void criarRAM(int tamanho) {
		memoria = new int[tamanho];
	}
	
	void criarRAM_vazia(int tamanho) {
		criarRAM(tamanho);
		for(int i=0; i<tamanho; i++)
			memoria[i] =0;
	}
	
	void criarRAM_aleatoria(int tamanho) {
		Random r = new Random();
		criarRAM(tamanho);
		for(int i=0; i<tamanho; i++)
			memoria[i] =r.nextInt();
	}
	
	void setDado(int endereco, int conteudo) {
		memoria[endereco] = conteudo;
	}
	
	int getDado(int endereco) {
		return memoria[endereco];
	}
	
	void imprimir() {
		System.out.println("Conteudo da RAM");
		for(int i=0; i<memoria.length; i++)
			System.out.print(memoria[i] + ",");
		System.out.println();
	}

}
