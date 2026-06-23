package bcc266TP2.toy;

//MMU -> memory management unit
//UGM -> unidade de gerenciamento de memoria
public class MMU {
	
	
	static BlocoMemoria buscarNasMemorias(Endereco e, RAM ram, BlocoMemoria[] cache1, BlocoMemoria[] cache2){
		
		//mapeamento direto a memoria
		int posicaoCache1 = e.endBloco%cache1.length;
		int posicaoCache2 = e.endBloco%cache2.length;
		int custo =0;
		
		if(cache1[posicaoCache1].endBloco==e.endBloco){
			custo =10;
			cache1[posicaoCache1].custo = custo;
			//indica onde foi achado 
			// 1 significa cache 1; 2 significa cache 2; e 3 significa RAM
			cache1[posicaoCache1].cacheHit = 1;
			return cache1[posicaoCache1];
		}else{
			
			if(cache2[posicaoCache2].endBloco==e.endBloco){	
				custo =110; // inclui custo da cache 1
				cache2[posicaoCache2].cacheHit = 2;
				return movCache2Cache1(posicaoCache1, posicaoCache2, cache1, cache2, custo);
			}else{
				//buscar na ram e levar para cache2 e depois levar para cache1
				custo =1110;				
				return movRamCache2(posicaoCache1, posicaoCache2, cache1, cache2, ram, e, custo);
				
			}			
		}	
		
	}
	
	static BlocoMemoria movCache2Cache1(int posicaoCache1, int posicaoCache2, BlocoMemoria[] cache1, BlocoMemoria[] cache2, int custo){
		BlocoMemoria aux = cache1[posicaoCache1];	
		cache1[posicaoCache1] = cache2[posicaoCache2];
		cache2[posicaoCache2] = aux;
		
		cache1[posicaoCache1].custo = custo;
		
		return cache1[posicaoCache1]; 
	}	
	
	static BlocoMemoria movRamCache2(int posicaoCache1, int posicaoCache2, BlocoMemoria[] cache1, BlocoMemoria[] cache2, RAM ram, Endereco e, int custo){
		if(!cache2[posicaoCache2].atualizado){
			cache2[posicaoCache2] = ram.getDado(e.endBloco);
			cache2[posicaoCache2].cacheHit = 3;
			return movCache2Cache1(posicaoCache1, posicaoCache2, cache1, cache2, custo);
			
		}else{
			cache2[posicaoCache2].atualizado = false;
			cache2[posicaoCache2].cacheHit = 0;
			cache2[posicaoCache2].custo = 0;
			ram.setDado(cache2[posicaoCache2].endBloco, cache2[posicaoCache2]);
					
			cache2[posicaoCache2] = ram.getDado(e.endBloco);
			cache2[posicaoCache2].cacheHit = 3;
			
			return movCache2Cache1(posicaoCache1, posicaoCache2, cache1, cache2, custo);
		}		 
	}

}
