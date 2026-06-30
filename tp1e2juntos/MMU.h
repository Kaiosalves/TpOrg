#ifndef MMU_H
#define MMU_H

#include "RAM.h"
#include "Endereco.h"
#include "BlocoMemoria.h"


BlocoMemoria* buscarNasMemorias(Endereco end, Ram *ram, BlocoMemoria *cache1, int tamCache1, BlocoMemoria *cache2, int tamCache2);

#endif