#!/usr/bin/env python3
import csv
import sys

# Definição das máquinas (conforme imagem)
MAQUINAS = {
    'M1': (8, 16, 32),
    'M2': (32, 64, 128),
    'M3': (16, 64, 256),
    'M4': (8, 32, 128),
    'M5': (16, 32, 64),
}

PROBS = [60, 80, 95]
FORS = [5, 15, 30]

def calcular_taxa(hits, total):
    return (hits / total * 100) if total > 0 else 0.0

def calcular_tempo(l1_hits, l1_miss, l2_hits, l2_miss, l3_hits, l3_miss, ram_acessos):
    # Latências: L1=1, L2=10, L3=50, RAM=200
    # Cada acesso que chega a um nível contribui com a latência daquele nível
    # Tempo = L1_hits*1 + L1_miss*1 (todos os acessos a L1) + L2_hits*10 + L2_miss*10 + L3_hits*50 + L3_miss*50 + ram_acessos*200
    # Mas L2_hits+L2_miss = L1_miss, L3_hits+L3_miss = L2_miss, ram_acessos = L3_miss
    tempo = (l1_hits + l1_miss) * 1      # acessos a L1
    tempo += (l2_hits + l2_miss) * 10    # acessos a L2
    tempo += (l3_hits + l3_miss) * 50    # acessos a L3
    tempo += l3_miss * 200               # acessos a RAM (quando miss em L3)
    return tempo

def main():
    arquivo_csv = 'resultados.csv'
    if len(sys.argv) > 1:
        arquivo_csv = sys.argv[1]

    # Lê o CSV
    dados = {}
    with open(arquivo_csv, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            prob = int(row['N_PROB'])
            for_val = int(row['N_FOR'])
            l1 = int(row['L1'])
            l2 = int(row['L2'])
            l3 = int(row['L3'])
            chave = (prob, for_val, l1, l2, l3)
            dados[chave] = {
                'l1_hits': int(row['L1_Hit%']),
                'l1_miss': int(row['L1_Miss%']),
                'l2_hits': int(row['L2_Hit%']),
                'l2_miss': int(row['L2_Miss%']),
                'l3_hits': int(row['L3_Hit%']),
                'l3_miss': int(row['L3_Miss%']),
                'ram_acessos': int(row['RAM_Acessos']),
                'total': int(row['Total_Acessos']),
            }

    # Prepara os resultados
    resultados = []
    for nome_maq, (l1, l2, l3) in MAQUINAS.items():
        for prob in PROBS:
            for for_val in FORS:
                chave = (prob, for_val, l1, l2, l3)
                if chave not in dados:
                    print(f"Aviso: combinação não encontrada: {nome_maq}, N_PROB={prob}, N_FOR={for_val}", file=sys.stderr)
                    continue
                d = dados[chave]
                total = d['total']
                taxa_l1 = calcular_taxa(d['l1_hits'], total)
                taxa_l2 = calcular_taxa(d['l2_hits'], d['l1_miss']) if d['l1_miss'] > 0 else 0.0
                taxa_l3 = calcular_taxa(d['l3_hits'], d['l2_miss']) if d['l2_miss'] > 0 else 0.0
                taxa_ram = calcular_taxa(d['ram_acessos'], total)
                tempo = calcular_tempo(d['l1_hits'], d['l1_miss'], d['l2_hits'], d['l2_miss'],
                                       d['l3_hits'], d['l3_miss'], d['ram_acessos'])
                resultados.append({
                    'Maquina': nome_maq,
                    'N_PROB': prob,
                    'N_FOR': for_val,
                    'L1': l1,
                    'L2': l2,
                    'L3': l3,
                    'Taxa_L1%': round(taxa_l1, 2),
                    'Taxa_L2%': round(taxa_l2, 2),
                    'Taxa_L3%': round(taxa_l3, 2),
                    'Taxa_RAM%': round(taxa_ram, 2),
                    'Taxa_Disco%': 0.0,
                    'Tempo': round(tempo, 0),
                })

    # Salva CSV
    with open('tabela_final.csv', 'w', newline='') as f:
        fieldnames = ['Maquina', 'N_PROB', 'N_FOR', 'L1', 'L2', 'L3',
                      'Taxa_L1%', 'Taxa_L2%', 'Taxa_L3%', 'Taxa_RAM%', 'Taxa_Disco%', 'Tempo']
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(resultados)

    # Exibe uma tabela compacta no terminal
    print("\n=== TABELA DE RESULTADOS (Máquinas x Probabilidades x N_FOR) ===\n")
    print("Máq | Prob | FOR | L1 | L2 | L3  | L1%  | L2%  | L3%  | RAM% | Disco% | Tempo")
    print("----|------|-----|----|----|-----|------|------|------|------|--------|-------")
    for r in resultados:
        print(f"{r['Maquina']:3s} | {r['N_PROB']:4d} | {r['N_FOR']:3d} | {r['L1']:2d} | {r['L2']:2d} | {r['L3']:3d} | {r['Taxa_L1%']:5.2f} | {r['Taxa_L2%']:5.2f} | {r['Taxa_L3%']:5.2f} | {r['Taxa_RAM%']:5.2f} | {r['Taxa_Disco%']:6.1f} | {r['Tempo']:6.0f}")

    print(f"\nArquivo 'tabela_final.csv' gerado com {len(resultados)} linhas.")

if __name__ == '__main__':
    main()