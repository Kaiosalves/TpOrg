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
    tempo = (l1_hits + l1_miss) * 1
    tempo += (l2_hits + l2_miss) * 10
    tempo += (l3_hits + l3_miss) * 50
    tempo += l3_miss * 200
    return tempo

def main():
    arquivo_csv = 'tabela_final.csv'
    if len(sys.argv) > 1:
        arquivo_csv = sys.argv[1]

    # Lê o CSV e mostra o cabeçalho para debug
    with open(arquivo_csv, 'r') as f:
        reader = csv.DictReader(f)
        fieldnames = reader.fieldnames
        print("Colunas disponíveis no CSV:", fieldnames, file=sys.stderr)

    dados = {}
    with open(arquivo_csv, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            # Usa os nomes exatos do cabeçalho
            prob = int(row['N_PROB'])
            for_val = int(row['N_FOR'])
            l1 = int(row['L1'])
            l2 = int(row['L2'])
            l3 = int(row['L3'])
            chave = (prob, for_val, l1, l2, l3)
            # Os nomes das colunas com % podem ser 'L1_Hit%' ou 'L1_Hit%' (igual)
            # Vamos tentar com os nomes que você tem
            try:
                l1_hits = int(row['L1_Hit%'])
            except KeyError:
                # Tenta sem o % (caso o arquivo tenha sido salvo de forma diferente)
                l1_hits = int(row['L1_Hit'])
            try:
                l1_miss = int(row['L1_Miss%'])
            except KeyError:
                l1_miss = int(row['L1_Miss'])
            try:
                l2_hits = int(row['L2_Hit%'])
            except KeyError:
                l2_hits = int(row['L2_Hit'])
            try:
                l2_miss = int(row['L2_Miss%'])
            except KeyError:
                l2_miss = int(row['L2_Miss'])
            try:
                l3_hits = int(row['L3_Hit%'])
            except KeyError:
                l3_hits = int(row['L3_Hit'])
            try:
                l3_miss = int(row['L3_Miss%'])
            except KeyError:
                l3_miss = int(row['L3_Miss'])
            ram_acessos = int(row['RAM_Acessos'])
            total = int(row['Total_Acessos'])

            dados[chave] = {
                'l1_hits': l1_hits,
                'l1_miss': l1_miss,
                'l2_hits': l2_hits,
                'l2_miss': l2_miss,
                'l3_hits': l3_hits,
                'l3_miss': l3_miss,
                'ram_acessos': ram_acessos,
                'total': total,
            }

    # Prepara os dados
    linhas = []
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
                linhas.append({
                    'Maq': nome_maq,
                    'Prob': prob,
                    'FOR': for_val,
                    'L1': l1,
                    'L2': l2,
                    'L3': l3,
                    'Taxa_L1': round(taxa_l1, 2),
                    'Taxa_L2': round(taxa_l2, 2),
                    'Taxa_L3': round(taxa_l3, 2),
                    'Taxa_RAM': round(taxa_ram, 2),
                    'Taxa_Disco': 0.0,
                    'Tempo': round(tempo, 0),
                })

    # --- Gerar código LaTeX ---
    print("% Tabela gerada automaticamente")
    print("\\begin{table}[htbp]")
    print("\\centering")
    print("\\caption{Resultados das simulações (taxas de hit e tempo de execução)}")
    print("\\label{tab:resultados}")
    print("\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|c|}")
    print("\\hline")
    print("\\textbf{Máquina} & \\textbf{Probabilidade} & \\textbf{Tamanho FOR} & \\textbf{L1} & \\textbf{L2} & \\textbf{L3} & \\textbf{L1\\%} & \\textbf{L2\\%} & \\textbf{L3\\%} & \\textbf{RAM\\%} & \\textbf{Tempo} \\\\")
    print("\\hline")
    for r in linhas:
        print(f"{r['Maq']} & {r['Prob']}\\% & {r['FOR']} & {r['L1']} & {r['L2']} & {r['L3']} & {r['Taxa_L1']:.2f}\\% & {r['Taxa_L2']:.2f}\\% & {r['Taxa_L3']:.2f}\\% & {r['Taxa_RAM']:.2f}\\% & {r['Tempo']:.0f} \\\\")
    print("\\hline")
    print("\\end{tabular}")
    print("\\end{table}")

if __name__ == '__main__':
    main()