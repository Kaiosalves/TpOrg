#include "MMU.h"
#include <stdio.h>

//protoipo de funcoes de movimentacao de caches -><- Ram
BlocoMemoria* movCache2Cache1 (int posCache1, int posCache2, BlocoMemoria* cache1, BlocoMemoria* cache2, int custo);
BlocoMemoria* movRamCache2 (int posCache1, int posCache2, BlocoMemoria* cache1, BlocoMemoria* cache2,Ram *ram, Endereco e, int custo);

BlocoMemoria* buscarNasMemorias(Endereco end, Ram *ram, BlocoMemoria *cache1, int tamCache1, BlocoMemoria *cache2, int tamCache2){

    int posCache1 = end.endBloco % tamCache1;
    int posCache2 = end.endBloco % tamCache2;
    int custo = 0;

    if(cache1[posCache1].endBloco == end.endBloco){
        custo = 10;
        cache1[posCache1].custo = custo;
        cache1[posCache1].cacheHit = 1; // deu hit na cache 1;
        return &cache1[posCache1];

    }
    else{
        if(cache2[posCache2].endBloco == end.endBloco){
            custo = 110; //10 de procurar na cache 1 e 100 de proc na cache 2
            cache2[posCache2].cacheHit = 2;
            return movCache2Cache1(posCache1, posCache2, cache1, cache2, custo);

        }else{
            custo = 1110; // 10 + 100 + 1000
            return movRamCache2(posCache1, posCache2, cache1, cache2, ram, end, custo);
        }
    }
}

BlocoMemoria* movRamCache2 (int posCache1, int posCache2, BlocoMemoria* cache1, BlocoMemoria* cache2,Ram *ram, Endereco end, int custo){
    if(!cache2[posCache2]. atualizado){ // se nao tiver atualizado
        cache2[posCache2] = getBloco(ram, end.endBloco);//atualiza o bloco
        cache2[posCache2].cacheHit = 3; // veio da ram
        return movCache2Cache1(posCache1, posCache2, cache1, cache2, custo);
    }else{
        cache2[posCache2].atualizado = 0;
        cache2[posCache2].cacheHit = 0;
        cache2[posCache2].custo = 0;
        setBloco ( ram, cache2[posCache2].endBloco, cache2[posCache2]);
    
        cache2[posCache2] = getBloco(ram, end.endBloco);
        cache2[posCache2].cacheHit = 3;
        
        return movCache2Cache1(posCache1, posCache2, cache1, cache2, custo);
    
    }

}

BlocoMemoria* movCache2Cache1 (int posCache1, int posCache2, BlocoMemoria* cache1, BlocoMemoria* cache2, int custo){
    BlocoMemoria aux = cache1[posCache1];
    cache1[posCache1] = cache2[posCache2];
    cache2[posCache2] = aux;
    cache1[posCache1].custo = custo;

    return &cache1[posCache1];
}
