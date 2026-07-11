#!/usr/bin/env python3
import csv
import sys

def main():
    arquivo_csv = 'tabela_final.csv'
    if len(sys.argv) > 1:
        arquivo_csv = sys.argv[1]

    linhas = []
    with open(arquivo_csv, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            linhas.append({
                'Maq': row['Maquina'],
                'Prob': int(row['N_PROB']),
                'FOR': int(row['N_FOR']),
                'L1': int(row['L1']),
                'L2': int(row['L2']),
                'L3': int(row['L3']),
                'Taxa_L1': float(row['Taxa_L1%']),
                'Taxa_L2': float(row['Taxa_L2%']),
                'Taxa_L3': float(row['Taxa_L3%']),
                'Taxa_RAM': float(row['Taxa_RAM%']),
                'Taxa_Disco': float(row['Taxa_Disco%']),
                'Tempo': int(float(row['Tempo'])),
            })

    # --- Gera LaTeX ---
    print("% Tabela gerada a partir do CSV final")
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