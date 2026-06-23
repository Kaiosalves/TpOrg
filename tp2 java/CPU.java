package bcc266TP2.toy;

public class CPU {
	//a cpu pode ter dezenas de registradores
	BlocoMemoria registrador1;
	BlocoMemoria registrador2;
	BlocoMemoria registrador3;
		
	int PC;
	Instrucao[] programa;
	int opcode;
	
	int custo=0;
	//caches 1 e 2 Hit e miss
	int missC1 = 0;
	int hitC1 = 0;
	int missC2 = 0;
	int hitC2 = 0;
	
	BlocoMemoria[] cache1;
	BlocoMemoria[] cache2;
		
	void setPrograma(Instrucao[] programaAux) {
		programa = programaAux;
	}
	
	BlocoMemoria[] iniciarCache(int tamanho) {
		BlocoMemoria[] umaCache = new BlocoMemoria[tamanho];
		for(int i=0; i<umaCache.length; i++) 
			umaCache[i] = new BlocoMemoria();
					
		return umaCache;
	}
	
	void iniciar(RAM ram, int tamanhoCache1, int tamanhoCache2) {
		cache1 = iniciarCache(tamanhoCache1);
		cache2 = iniciarCache(tamanhoCache2);
		opcode=0;
		PC=0;
		//aqui inicia o pipeline
		while(opcode!=-1) {
			//carregar instrucao
			Instrucao inst = programa[PC];
			
			//decodificar instrucao
			opcode = inst.opcode;
			
			if(opcode!=-1) {
				
				registrador1 = MMU.buscarNasMemorias(inst.add1, ram, cache1, cache2);    
				registrador2 = MMU.buscarNasMemorias(inst.add2, ram, cache1, cache2);
				registrador3 = MMU.buscarNasMemorias(inst.add3, ram, cache1, cache2);
				
				switch (registrador1.cacheHit){
					case 1:{
						hitC1++;
						break;
					}
					case 2:{
						missC1++;
						hitC2++;
						break;
					}
					case 3:{
						missC1++;
						missC2++;
						break;
					}
				}
			
				switch (registrador2.cacheHit){
					case 1:{
						hitC1++;
						break;
					}
					case 2:{
						missC1++;
						hitC2++;
						break;
					}
					case 3:{
						missC1++;
						missC2++;
						break;
					}
				}
				
				switch (registrador3.cacheHit){
					case 1:{
						hitC1++;
						break;
					}
					case 2:{
						missC1++;
						hitC2++;
						break;
					}
					case 3:{
						missC1++;
						missC2++;
						break;
					}
				}
				
				//executar instrucao
				//-1 -> halt
				// 0 -> soma
				// 1 -> subtrai
				switch (opcode){
					//halt
					case -1: {
						System.out.println("programa terminou!!");
						ram.imprimir();
						break;
					}
					//soma
					case 0: {
						
						registrador3.palavras[inst.add3.endPalavra] = registrador1.palavras[inst.add1.endPalavra] + registrador2.palavras[inst.add2.endPalavra];
						registrador3.atualizado = true;
						
						custo += registrador1.custo+registrador2.custo+registrador3.custo;
						
						System.out.println("Inst sum -> RAM posicao " + inst.add3.endBloco + " com conteudo na cache 1 " + registrador3.palavras[inst.add3.endPalavra]);
						System.out.println("Custo ateh o momento.... " + custo);
											
						System.out.println("Ateh o momento ... Hit C1: " + hitC1 + " Miss C1: " + missC1);
						System.out.println("Ateh o momento ... Hit C2: " + hitC2 + " Miss C2: " + missC2);
						
						break;
					}
					//subtrai
					case 1: {
						
						registrador3.palavras[inst.add3.endPalavra] = registrador1.palavras[inst.add1.endPalavra] - registrador2.palavras[inst.add2.endPalavra];
						registrador3.atualizado = true;
						
						custo += registrador1.custo+registrador2.custo+registrador3.custo;
						
						System.out.println("Inst sub -> RAM posicao " + inst.add3.endBloco + " com conteudo na cache 1 " + registrador3.palavras[inst.add3.endPalavra]);
						System.out.println("Custo ateh o momento.... " + custo);
											
						System.out.println("Ateh o momento ... Hit C1: " + hitC1 + " Miss C1: " + missC1);
						System.out.println("Ateh o momento ... Hit C2: " + hitC2 + " Miss C2: " + missC2);
						
						break;
					}			
				}			
				
				PC++;
				
			}//end if
			
			
		}//end while
	}

}
