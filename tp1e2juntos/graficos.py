#!/usr/bin/env python3
import csv
import matplotlib.pyplot as plt
import numpy as np
import os

# Configurações visuais
plt.style.use('seaborn-v0_8-whitegrid')
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 12

def ler_dados(csv_file):
    """Lê o arquivo CSV e retorna uma lista de dicionários."""
    dados = []
    with open(csv_file, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            dados.append({
                'maq': row['Maquina'],
                'prob': int(row['N_PROB']),
                'for_val': int(row['N_FOR']),
                'l1': float(row['Taxa_L1%']),
                'l2': float(row['Taxa_L2%']),
                'l3': float(row['Taxa_L3%']),
                'ram': float(row['Taxa_RAM%']),
                'tempo': int(float(row['Tempo']))
            })
    return dados

def gerar_graficos(dados, pasta_saida='graficos'):
    """Gera gráficos de barras para cada combinação de Prob e FOR."""
    if not os.path.exists(pasta_saida):
        os.makedirs(pasta_saida)

    # Máquinas na ordem desejada
    maquinas = ['M1', 'M2', 'M3', 'M4', 'M5']
    probs = sorted(set(d['prob'] for d in dados))
    for_vals = sorted(set(d['for_val'] for d in dados))

    # 1. Gráfico de barras agrupadas (L1, L2, L3) para cada (Prob, FOR)
    for prob in probs:
        for for_val in for_vals:
            # Filtra dados para esta combinação
            sub = [d for d in dados if d['prob'] == prob and d['for_val'] == for_val]
            # Ordena por máquina
            sub.sort(key=lambda x: maquinas.index(x['maq']))

            if not sub:
                continue

            # Prepara dados
            x = np.arange(len(maquinas))  # posições das máquinas
            width = 0.25  # largura das barras

            l1_vals = [d['l1'] for d in sub]
            l2_vals = [d['l2'] for d in sub]
            l3_vals = [d['l3'] for d in sub]

            fig, ax = plt.subplots(figsize=(10, 6))
            bars1 = ax.bar(x - width, l1_vals, width, label='L1%', color='#1f77b4')
            bars2 = ax.bar(x, l2_vals, width, label='L2%', color='#ff7f0e')
            bars3 = ax.bar(x + width, l3_vals, width, label='L3%', color='#2ca02c')

            ax.set_xlabel('Máquina')
            ax.set_ylabel('Hit Rate (%)')
            ax.set_title(f'Probabilidade = {prob}%, Tamanho FOR = {for_val}')
            ax.set_xticks(x)
            ax.set_xticklabels(maquinas)
            ax.legend()
            ax.grid(axis='y', linestyle='--', alpha=0.7)

            # Adiciona valores acima das barras (opcional)
            for bars in [bars1, bars2, bars3]:
                for bar in bars:
                    height = bar.get_height()
                    ax.annotate(f'{height:.1f}%',
                                xy=(bar.get_x() + bar.get_width()/2, height),
                                xytext=(0, 3),
                                textcoords="offset points",
                                ha='center', va='bottom', fontsize=8)

            plt.tight_layout()
            nome_arquivo = f'hitrates_prob{prob}_for{for_val}.png'
            plt.savefig(os.path.join(pasta_saida, nome_arquivo), dpi=150)
            plt.close()
            print(f'Gerado: {nome_arquivo}')

    # 2. Gráfico de tempo de execução (linhas) para cada máquina
    fig, ax = plt.subplots(figsize=(12, 6))
    for maq in maquinas:
        # Filtra dados da máquina
        sub = [d for d in dados if d['maq'] == maq]
        # Ordena por Prob e FOR
        sub.sort(key=lambda x: (x['prob'], x['for_val']))
        # Cria rótulos para o eixo X
        labels = [f"{d['prob']}/{d['for_val']}" for d in sub]
        tempos = [d['tempo'] for d in sub]
        ax.plot(labels, tempos, marker='o', label=maq)

    ax.set_xlabel('Probabilidade / Tamanho FOR')
    ax.set_ylabel('Tempo (ciclos)')
    ax.set_title('Tempo de execução por máquina e configuração')
    ax.legend()
    ax.grid(True, linestyle='--', alpha=0.7)
    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.savefig(os.path.join(pasta_saida, 'tempo_execucao.png'), dpi=150)
    plt.close()
    print('Gerado: tempo_execucao.png')

    print(f"\nTodos os gráficos salvos na pasta '{pasta_saida}'.")

if __name__ == "__main__":
    # Altere para 'tabela_final.csv' se preferir
    arquivo = 'tabela_final.csv'
    if not os.path.exists(arquivo):
        print(f"Erro: arquivo {arquivo} não encontrado.")
        exit(1)
    dados = ler_dados(arquivo)
    gerar_graficos(dados)