import serial
import csv

# ================= CONFIGURAÇÕES =================
PORTA_SERIAL = 'COM8'
BAUD_RATE = 115200
ARQUIVO_CSV = 'dados_cinematicos.csv'
# =================================================

def iniciar_leitura():
    try:
        conexao = serial.Serial(PORTA_SERIAL, BAUD_RATE, timeout=1)
        print(f"Conectado na porta {PORTA_SERIAL}.")
        print("Pressione Ctrl+C para parar a gravacao.")

        with open(ARQUIVO_CSV, mode='w', newline='') as arquivo:
            escritor_csv = csv.writer(arquivo)
            
            # Força a gravação do cabeçalho necessário para gerar o gráfico depois
            escritor_csv.writerow(['ax','ay','az','vx','vy','vz','px','py','pz'])

            while True:
                if conexao.in_waiting > 0:
                    linha = conexao.readline().decode('utf-8', errors='ignore').strip()
                    
                    if not linha:
                        continue
                    
                    # Separa os valores pela vírgula
                    dados = linha.split(',')
                    
                    # Salva no arquivo APENAS se a linha contiver exatamente 9 números
                    if len(dados) == 9:
                        escritor_csv.writerow(dados)
                        print(f"Dado salvo: {dados}")

    except serial.SerialException as e:
        print(f"Erro ao abrir a porta serial: {e}")
    except KeyboardInterrupt:
        print("\nLeitura encerrada pelo usuario.")
    finally:
        if 'conexao' in locals() and conexao.is_open:
            conexao.close()
            print("Porta serial fechada.")

if __name__ == "__main__":
    iniciar_leitura()