import subprocess
import re
import os
import sys
import csv
import time

# ============================================
# CONFIGURAÇÕES
# ============================================

# Arquivo do tratador (fixo)
TRATADOR = "tratador_interrupcao.txt"

# Programas para cada nível de repetição
PROBS = [50, 75, 90]
FORS = [5, 15, 30]

PROGRAMAS = {}
for p in PROBS:
    for f in FORS:
        nome_arquivo = f"programa_{p}_f{f}.txt"
        rotulo = f"{p}% - For {f}"
        PROGRAMAS[rotulo] = nome_arquivo

# Configurações das máquinas (Cache1, Cache2, Cache3, RAM)
# Baseado na tabela do PDF do TP3
CONFIGURACOES = [
    (8, 16, 32, 64),    # M181632
    (32, 64, 128, 64),   # M23264128
    (16, 64, 256, 64),   # M31664256
    (8, 32, 128, 64),    # M4832128
    (16, 32, 64, 64),    # M5163264
]

# Se quiser testar todas as combinações, descomente o bloco abaixo:
# L1_SIZES = [4, 8, 16, 32]
# L2_SIZES = [8, 16, 32, 64]
# L3_SIZES = [16, 32, 64, 128]
# RAM_SIZES = [16, 32, 64, 128]
# CONFIGURACOES = [(l1,l2,l3,ram) for l1 in L1_SIZES for l2 in # # # L2_SIZES for l3 in L3_SIZES for ram in RAM_SIZES]

# ============================================
# FUNÇÕES AUXILIARES
# ============================================

def parse_output(saida):
    """
    Extrai as estatísticas da saída do simulador usando expressões regulares.
    Retorna um dicionário com as métricas.
    """
    dados = {}

    # Taxa L1
    match = re.search(r"L1 - Hits:.*?Taxa: ([\d.]+)%", saida)
    if match:
        dados['taxa_l1'] = float(match.group(1))

    # Taxa L2
    match = re.search(r"L2 - Hits:.*?Taxa: ([\d.]+)%", saida)
    if match:
        dados['taxa_l2'] = float(match.group(1))

    # Taxa L3
    match = re.search(r"L3 - Hits:.*?Taxa: ([\d.]+)%", saida)
    if match:
        dados['taxa_l3'] = float(match.group(1))

    # Taxa RAM
    match = re.search(r"RAM - Hits:.*?Taxa: ([\d.]+)%", saida)
    if match:
        dados['taxa_ram'] = float(match.group(1))

    # Taxa Disco (dentro de parênteses)
    match = re.search(r"Acessos ao disco:.*?\(([\d.]+)%", saida)
    if match:
        dados['taxa_disco'] = float(match.group(1))

    # Tempo total (ciclos)
    match = re.search(r"Tempo total estimado: ([\d]+)", saida)
    if match:
        dados['tempo'] = int(match.group(1))

    return dados


def rodar_simulacao(l1, l2, l3, ram, programa, tratador, timeout=60):
    """
    Executa o simulador com os parâmetros fornecidos.
    Retorna a saída padrão (stdout + stderr) como string.
    """
    # Remove o disco antigo para garantir um estado limpo (opcional)
    if os.path.exists("disco.bin"):
        try:
            os.remove("disco.bin")
        except PermissionError:
            pass  # se não conseguir remover, ignora

    cmd = ["./tp3", str(l1), str(l2), str(l3), str(ram), programa, tratador]

    try:
        resultado = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=timeout
        )
        # Junta stdout e stderr (o simulador imprime tudo no stdout)
        return resultado.stdout + resultado.stderr
    except subprocess.TimeoutExpired:
        print(f"TIMEOUT: {l1},{l2},{l3},{ram},{programa}", file=sys.stderr)
        return ""
    except Exception as e:
        print(f"ERRO: {e} - {l1},{l2},{l3},{ram},{programa}", file=sys.stderr)
        return ""


def main():
    # Verifica se o executável existe
    if not os.path.exists("./tp3"):
        print("ERRO: executável ./tp3 não encontrado. Compile primeiro.")
        sys.exit(1)

    # Verifica se os arquivos de programa existem
    for rep, arquivo in PROGRAMAS.items():
        if not os.path.exists(arquivo):
            print(f"AVISO: {arquivo} não encontrado. Pule essa repetição.", file=sys.stderr)

    # Cabeçalho do CSV
    cabecalho = [
        "Cache1",
        "Cache2",
        "Cache3",
        "RAM",
        "Repetição",
        "Taxa L1 (%)",
        "Taxa L2 (%)",
        "Taxa L3 (%)",
        "Taxa RAM (%)",
        "Taxa Disco (%)",
        "Tempo (ciclos)"
    ]

    # Abre o arquivo de saída CSV
    with open("resultados.csv", "w", newline="", encoding="utf-8") as f_csv:
        writer = csv.writer(f_csv, delimiter=";")
        writer.writerow(cabecalho)

        # Loop sobre as configurações
        total_configs = len(CONFIGURACOES) * len(PROGRAMAS)
        contador = 0

        for l1, l2, l3, ram in CONFIGURACOES:
            for rep, prog in PROGRAMAS.items():
                contador += 1
                print(f"[{contador}/{total_configs}] Executando L1={l1} L2={l2} L3={l3} RAM={ram} - {rep}")

                # Se o arquivo de programa não existir, pula com erro
                if not os.path.exists(prog):
                    linha = [l1, l2, l3, ram, rep, "ERRO", "ERRO", "ERRO", "ERRO", "ERRO", "ERRO"]
                    writer.writerow(linha)
                    continue

                # Executa a simulação
                saida = rodar_simulacao(l1, l2, l3, ram, prog, TRATADOR)

                if saida:
                    dados = parse_output(saida)
                    if dados:
                        linha = [
                            l1,
                            l2,
                            l3,
                            ram,
                            rep,
                            f"{dados.get('taxa_l1', 0):.2f}",
                            f"{dados.get('taxa_l2', 0):.2f}",
                            f"{dados.get('taxa_l3', 0):.2f}",
                            f"{dados.get('taxa_ram', 0):.2f}",
                            f"{dados.get('taxa_disco', 0):.2f}",
                            dados.get('tempo', 0)
                        ]
                        writer.writerow(linha)
                    else:
                        # Não conseguiu parsear
                        linha = [l1, l2, l3, ram, rep, "PARSE", "PARSE", "PARSE", "PARSE", "PARSE", "PARSE"]
                        writer.writerow(linha)
                else:
                    # Saída vazia (timeout ou erro)
                    linha = [l1, l2, l3, ram, rep, "FALHA", "FALHA", "FALHA", "FALHA", "FALHA", "FALHA"]
                    writer.writerow(linha)

                # Pequeno delay para evitar sobrecarga (opcional)
                time.sleep(0.1)

    print("\n✅ Experimentos concluídos! Resultados salvos em 'resultados.csv'.")
    print("Você pode abrir este arquivo no Excel ou Google Sheets.")


if __name__ == "__main__":
    main()