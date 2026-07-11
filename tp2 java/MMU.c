#include "MMU.h"
#include <stdio.h>

//protoipo de funcoes de movimentacao de caches -><- Ram
BlocoMemoria* movCache2Cache1 (int posCache1, int posCache2, BlocoMemoria* cache1, BlocoMemoria* cache2, int custo);
BlocoMemoria* movRamCache2 (int posCache1, int posCache2, BlocoMemoria* cache1, BlocoMemoria* cache2,Ram *ram, Endereco e, int custo);
void expulsaEmCascata(BlocoMemoria blocoExpulso, Ram *R, BlocoMemoria* cache1, BlocoMemoria* cache2, BlocoMemoria* cache3, int tamC1, int tamC2, int tamC3, int tempoGlobal);

void expulsaEmCascata(BlocoMemoria blocoExpulso, Ram *R, BlocoMemoria* cache1, BlocoMemoria* cache2, BlocoMemoria* cache3, int tamC1, int tamC2, int tamC3, int tempoGlobal){
    tempoGlobal++;

    int posNaC2 = encontrarPosicao(tamC2, cache2);//posição do bloco na cache2
    
    BlocoMemoria blocoExpC2 = cache2[posNaC2];//backup do dado da cahe2
    cache2[posNaC2] = blocoExpulso;//bloco que saiu da cache1 entrando na cache2
    cache2[posNaC2].contador = tempoGlobal;

    //se o bloco que saiu da cache2 não estava vazio
    if(blocoExpC2.endBloco != -1){
        int posC3 = encontrarPosicao(tamC3, cache3);
        BlocoMemoria blocoExpC3 = cache3[posC3];
        cache3[posC3] = blocoExpC2;
        cache3[posC3].contador = tempoGlobal;
        //se o bloco que saiu da 3 n estava vazio e esta atualizado
        if(blocoExpC3.endBloco !=-1 && blocoExpC3.atualizado)
            setBloco(R,blocoExpC3.endBloco, blocoExpC3);
    }
}
void swap(BlocoMemoria* cacheSup, BlocoMemoria* cacheInf, int posCSup, int posCInf){
    BlocoMemoria blocoAlvo = cacheInf[posCInf];
    BlocoMemoria blocoRemovido = cacheSup[posCSup];

    cacheSup[posCSup] = blocoAlvo;
    cacheInf[posCInf] = blocoRemovido;
}

int encontrarPosicao(int tamMem, BlocoMemoria* cache){
    
    for(int i = 0; i  < tamMem; i++){
        if(cache[i].endBloco == -1)
            return i;
    }
    int menor = 0;
    for(int i = 0; i < tamMem; i++){
        if(cache[i].contador < cache[menor].contador)
            menor = i;
    }
    return menor;
}
int buscaPorEndereco(int tamMem, BlocoMemoria* cache, int endBloco){
    for(int i = 0; i < tamMem; i++){
        if(cache[i].endBloco == endBloco)
            return i;
    }
    return -1;
}
BlocoMemoria* buscarNasMemoriasLRU(Endereco end, Ram *ram, BlocoMemoria *cache1, int tamCache1, BlocoMemoria *cache2, int tamCache2, BlocoMemoria* cache3, int tamCache3, int relogioGlobal){
    int posC1, posC2, posC3;

    relogioGlobal++;
    //busca o endereco na cache1
    int idxCache1 = buscaPorEndereco(tamCache1, cache1, end.endBloco);
    if(idxCache1 != -1){ // hit na cache 1
        cache1[idxCache1].cacheHit = 1;
        cache1[idxCache1].custo = 10;
        cache1[idxCache1].contador = relogioGlobal; 
        return &cache1[idxCache1];
    }
    //busca o endereco na cache2
    int idxCache2 = buscaPorEndereco(tamCache2, cache2, end.endBloco);
    if(idxCache2 != -1){
        BlocoMemoria blocoAlvo = cache2[idxCache2];
        //encontra um posicao livre na c1 ou com LRU e sobe o bloco da c2
        posC1 = encontrarPosicao(tamCache1, cache1);
        BlocoMemoria expulsoC1 = cache1[posC1];

        cache1[posC1] = blocoAlvo;
        cache1[posC1].custo = 110;
        cache1[posC1].cacheHit = 2;
        cache1[posC1].contador = relogioGlobal;
    
        if(expulsoC1.endBloco != -1)
            expulsaEmCascata(expulsoC1, ram, cache1, cache2, cache3, tamCache1, tamCache2, tamCache3, relogioGlobal);
        
        return &cache1[posC1];
    }
    //busca o endereco na cache3
    int idxCache3 = buscaPorEndereco(tamCache3, cache3, end.endBloco);
    if(idxCache3 != -3){
        BlocoMemoria blocoAlvo = cache3[idxCache3];
        cache3[idxCache3].endBloco = -1;// libera o bloco
        // sobe o bloco encontrado na c3 para c2
        posC2 = encontrarPosicao(tamCache2, cache2);
        BlocoMemoria removidoC2 = cache2[posC2];
        cache2[posC2] = blocoAlvo;
        //sobe o bloco alvo da c2 para c1 com LRU
        int posC1 = encontrarPosicao(tamCache1, cache1);
        BlocoMemoria removidoC1 = cache1[posC1];

        cache1[posC1] = cache2[posC2];
        cache2[posC2].endBloco = -1;//libera o bloco
        
        cache1[posC1].cacheHit = 3;
        cache1[posC1].contador = relogioGlobal;
        cache1[posC1].custo = 1110;
    
        if(removidoC1.endBloco != -1)
            expulsaEmCascata(removidoC1, ram, cache1, cache2, cache3, tamCache1, tamCache2, tamCache3, relogioGlobal);
        if(removidoC2.endBloco != -1)
            expulsaEmCascata(removidoC2, ram, cache1, cache2, cache3, tamCache1, tamCache2, tamCache3, relogioGlobal);
    
        return &cache1[posC1];
    }

    // nao achou em nenhuma das caches, vamos procurar na ram
    int custo = 11110;
    BlocoMemoria dadoNovo = getBloco(ram, end.endBloco);
    dadoNovo.custo = custo;
    dadoNovo.cacheHit = 4;

    // Passo A: Entra na Cache 3
    posC3 = encontrarPosicaoLRU(cache3, tamCache3);
    BlocoMemoria expulsoC3 = cache3[posC3];
    cache3[posC3] = dadoNovo;
    cache3[posC3].contador = relogioGlobal;

    if (expulsoC3.endBloco != -1 && expulsoC3.atualizado == 1) {
        setBloco(ram, expulsoC3.endBloco, expulsoC3);
    }

    // Passo B: Sobe para a Cache 2
    posC2 = encontrarPosicaoLRU(cache2, tamCache2);
    BlocoMemoria expulsoC2 = cache2[posC2];
    cache2[posC2] = cache3[posC3];
    cache3[posC3].endBloco = -1; // tirou da C3

    if (expulsoC2.endBloco != -1) {
        descidaCascataLRU(expulsoC2, ram, cache2, tamCache2, cache3, tamCache3);
    }

    // Passo C: Finalmente sobe para a Cache 1
    posC1 = encontrarPosicaoLRU(cache1, tamCache1);
    BlocoMemoria expulsoC1 = cache1[posC1];
    cache1[posC1] = cache2[posC2];
    cache2[posC2].endBloco = -1; // tirou da C2

    if (expulsoC1.endBloco != -1) {
        descidaCascataLRU(expulsoC1, ram, cache2, tamCache2, cache3, tamCache3);
    }

    return &cache1[posC1];


}
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

BlocoMemoria* movRamCache3 (int posCache1, int posCache2, int posCache3, BlocoMemoria* cache1, BlocoMemoria* cache2, BlocoMemoria* cache3, Ram *ram, Endereco end, int custo){
    if(!cache3[posCache3]. atualizado){ // se nao tiver atualizado
        cache3[posCache3] = getBloco(ram, end.endBloco);//atualiza o bloco
        cache3[posCache3].cacheHit = 4; // veio da ram
        return movCache2Cache1(posCache2, posCache3, cache2, cache3, custo);
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
