import subprocess
import re
import os
import sys
import csv
import time

# ============================================
# CONFIGURAÇÕES
# ============================================

TRATADOR = "tratador_interrupcao.txt"

# Probabilidades e tamanhos do FOR
PROBS = [60, 80, 95]   # se quiser só 50,75,90, mude para [50,75,90]
FORS = [5, 15, 30]

# Configurações das máquinas (Cache1, Cache2, Cache3, RAM)
# Baseado na tabela do PDF do TP3
CONFIGURACOES = [
    (8, 16, 32, 64),    # M1
    (32, 64, 128, 64),  # M2
    (16, 64, 256, 64),  # M3
    (8, 32, 128, 64),   # M4
    (16, 32, 64, 64),   # M5
]

# ============================================
# FUNÇÕES AUXILIARES
# ============================================

def parse_output(saida):
    """
    Extrai as estatísticas da saída do simulador.
    """
    dados = {}

    match = re.search(r"L1 - Hits:.*?Taxa: ([\d.]+)%", saida)
    if match:
        dados['taxa_l1'] = float(match.group(1))

    match = re.search(r"L2 - Hits:.*?Taxa: ([\d.]+)%", saida)
    if match:
        dados['taxa_l2'] = float(match.group(1))

    match = re.search(r"L3 - Hits:.*?Taxa: ([\d.]+)%", saida)
    if match:
        dados['taxa_l3'] = float(match.group(1))

    match = re.search(r"RAM - Hits:.*?Taxa: ([\d.]+)%", saida)
    if match:
        dados['taxa_ram'] = float(match.group(1))

    match = re.search(r"Acessos ao disco:.*?\(([\d.]+)%", saida)
    if match:
        dados['taxa_disco'] = float(match.group(1))

    match = re.search(r"Tempo total estimado: ([\d]+)", saida)
    if match:
        dados['tempo'] = int(match.group(1))

    return dados

def rodar_simulacao(l1, l2, l3, ram, programa, tratador, timeout=60):
    if os.path.exists("disco.bin"):
        try:
            os.remove("disco.bin")
        except:
            pass

    cmd = ["./tp3", str(l1), str(l2), str(l3), str(ram), programa, tratador]

    try:
        resultado = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=timeout
        )
        return resultado.stdout + resultado.stderr
    except subprocess.TimeoutExpired:
        print(f"TIMEOUT: {l1},{l2},{l3},{ram},{programa}", file=sys.stderr)
        return ""
    except Exception as e:
        print(f"ERRO: {e} - {l1},{l2},{l3},{ram},{programa}", file=sys.stderr)
        return ""

# ============================================
# PRINCIPAL
# ============================================

def main():
    if not os.path.exists("./tp3"):
        print("ERRO: executável ./tp3 não encontrado. Compile primeiro.")
        sys.exit(1)

    # Cabeçalho do CSV
    cabecalho = [
        "Maquina", "Cache1", "Cache2", "Cache3", "RAM",
        "Prob (%)", "For", "Taxa L1 (%)", "Taxa L2 (%)",
        "Taxa L3 (%)", "Taxa RAM (%)", "Taxa Disco (%)", "Tempo (ciclos)"
    ]

    total_configs = len(CONFIGURACOES) * len(PROBS) * len(FORS)
    contador = 0

    with open("resultados_45.csv", "w", newline="", encoding="utf-8") as f_csv:
        writer = csv.writer(f_csv, delimiter=";")
        writer.writerow(cabecalho)

        for idx_maq, (l1, l2, l3, ram) in enumerate(CONFIGURACOES, start=1):
            for prob in PROBS:
                for for_size in FORS:
                    contador += 1
                    nome_arquivo = f"programa_{prob}_f{for_size}.txt"
                    rotulo_rep = f"{prob}% - For {for_size}"

                    print(f"[{contador}/{total_configs}] M{idx_maq} L1={l1} L2={l2} L3={l3} RAM={ram} - {rotulo_rep}")

                    if not os.path.exists(nome_arquivo):
                        print(f"  AVISO: {nome_arquivo} não encontrado. Pulando.")
                        linha = [f"M{idx_maq}", l1, l2, l3, ram, prob, for_size,
                                 "ERRO", "ERRO", "ERRO", "ERRO", "ERRO", "ERRO"]
                        writer.writerow(linha)
                        continue

                    saida = rodar_simulacao(l1, l2, l3, ram, nome_arquivo, TRATADOR)

                    if saida:
                        dados = parse_output(saida)
                        if dados:
                            linha = [
                                f"M{idx_maq}", l1, l2, l3, ram,
                                prob, for_size,
                                f"{dados.get('taxa_l1', 0):.2f}",
                                f"{dados.get('taxa_l2', 0):.2f}",
                                f"{dados.get('taxa_l3', 0):.2f}",
                                f"{dados.get('taxa_ram', 0):.2f}",
                                f"{dados.get('taxa_disco', 0):.2f}",
                                dados.get('tempo', 0)
                            ]
                            writer.writerow(linha)
                        else:
                            linha = [f"M{idx_maq}", l1, l2, l3, ram, prob, for_size,
                                     "PARSE", "PARSE", "PARSE", "PARSE", "PARSE", "PARSE"]
                            writer.writerow(linha)
                    else:
                        linha = [f"M{idx_maq}", l1, l2, l3, ram, prob, for_size,
                                 "FALHA", "FALHA", "FALHA", "FALHA", "FALHA", "FALHA"]
                        writer.writerow(linha)

                    time.sleep(0.1)

    print("\n✅ Experimentos concluídos! Resultados salvos em 'resultados_45.csv'.")

if __name__ == "__main__":
    main()