#!/usr/bin/env python3
import csv
import sys

# Lê o CSV final (gerado pelo tabela_final.py)
def ler_dados(csv_file):
    dados = {}
    with open(csv_file, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            maq = row['Maquina']
            prob = int(row['N_PROB'])
            for_val = int(row['N_FOR'])
            chave = (prob, for_val)
            if maq not in dados:
                dados[maq] = {}
            dados[maq][chave] = {
                'L1': float(row['Taxa_L1%']),
                'L2': float(row['Taxa_L2%']),
                'L3': float(row['Taxa_L3%']),
                'RAM': float(row['Taxa_RAM%']),
                'Tempo': int(float(row['Tempo']))
            }
    return dados

def gerar_latex(dados, maquinas_ordem, probs, fors):
    # Cabeçalho: para cada máquina, 4 colunas (L1, L2, L3, Tempo)
    colunas_por_maq = 4  # L1%, L2%, L3%, Tempo
    num_maq = len(maquinas_ordem)
    num_colunas = 1 + num_maq * colunas_por_maq  # coluna de (Prob, FOR)

    # Início da tabela
    print("% Tabela compacta por máquina")
    print("\\begin{table}[htbp]")
    print("\\centering")
    print("\\caption{Resultados por máquina e configuração (L1\\%, L2\\%, L3\\%, Tempo)}")
    print("\\label{tab:resultados_compacto}")
    print("\\tiny")  # para caber na página
    print("\\begin{tabular}{c" + "|cccc" * num_maq + "}")
    print("\\toprule")

    # Linha do cabeçalho: Máquina e subcolunas
    cabecalho = ["Prob & FOR"]
    for maq in maquinas_ordem:
        cabecalho.append(f"\\multicolumn{{4}}{{c|}}{{{maq}}}")
    print(" & ".join(cabecalho) + " \\\\")
    # Linha com os nomes das métricas
    metricas = [" & "]
    for _ in maquinas_ordem:
        metricas.append("L1\\% & L2\\% & L3\\% & Tempo")
    print(" & ".join(metricas) + " \\\\")
    print("\\midrule")

    # Para cada combinação (prob, for)
    for prob in probs:
        for for_val in fors:
            linha = [f"{prob}\\% & {for_val}"]
            for maq in maquinas_ordem:
                chave = (prob, for_val)
                if maq in dados and chave in dados[maq]:
                    d = dados[maq][chave]
                    linha.append(f"{d['L1']:.2f} & {d['L2']:.2f} & {d['L3']:.2f} & {d['Tempo']}")
                else:
                    linha.append("-- & -- & -- & --")
            print(" & ".join(linha) + " \\\\")
            # Linha de separação entre FORs (opcional)
            # if for_val == fors[-1]:
            #     print("\\midrule")
    print("\\bottomrule")
    print("\\end{tabular}")
    print("\\end{table}")

def main():
    arquivo_csv = 'tabela_final.csv'
    if len(sys.argv) > 1:
        arquivo_csv = sys.argv[1]

    dados = ler_dados(arquivo_csv)
    maquinas_ordem = ['M1', 'M2', 'M3', 'M4', 'M5']  # ordem conforme imagem
    probs = [60, 80, 95]
    fors = [5, 15, 30]

    gerar_latex(dados, maquinas_ordem, probs, fors)

if __name__ == '__main__':
    main()