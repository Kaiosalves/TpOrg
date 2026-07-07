#!/bin/bash

# Configurações
PROBS=(60 80 95)
FORS=(5 15 30)
L1_SIZES=(8 32 16 8 16)
L2_SIZES=(16 64 64 32 32)
L3_SIZES=(32 128 256 128 64)

# Compila o gerador de instruções e o simulador
gcc -o gerador instructionsGenerator.c
gcc -o tp2 main.c CPU.c RAM.c UCM.c cache.c -I.

# Cabeçalho do CSV
echo "N_PROB,N_FOR,L1,L2,L3,L1_Hit%,L1_Miss%,L2_Hit%,L2_Miss%,L3_Hit%,L3_Miss%,RAM_Acessos,Total_Acessos" > resultados.csv

# Loop
for prob in "${PROBS[@]}"; do
  for for_val in "${FORS[@]}"; do
    # Gera instruções específicas
    ./gerador $prob $for_val
    INST_FILE="instructions_p${prob}_f${for_val}.txt"
    
    for l1 in "${L1_SIZES[@]}"; do
      for l2 in "${L2_SIZES[@]}"; do
        for l3 in "${L3_SIZES[@]}"; do
          echo "Rodando: prob=$prob for=$for_val L1=$l1 L2=$l2 L3=$l3"
          # Executa e captura a linha HIT_STATS
          OUTPUT=$(./tp2 $l1 $l2 $l3 $INST_FILE | grep "HIT_STATS:")
          if [ -n "$OUTPUT" ]; then
            # Extrai os números
            STATS=$(echo $OUTPUT | cut -d':' -f2)
            # Adiciona ao CSV
            echo "$prob,$for_val,$l1,$l2,$l3,$STATS" >> resultados.csv
          else
            echo "  ERRO: nenhum HIT_STATS encontrado"
          fi
        done
      done
    done
  done
done

echo "Pronto! Resultados em resultados.csv"