Construção do Acelerometro
=====================================

Este código é uma demonstração do funcionamento do método acelerômetro MMA8451Q, tal qual envia dados da aceleração (x, y e z) a partir 
do movimento da placa micrcontroladora Freedom FRDM KL25Z pela porta serial para o terminal do computador. Há dois arquivos python que 
extraem a leitura destes valores e convertem na forma de um gráfico, sendo assim, fácil e intutivo de visualizar o movimento em tempo 
real da aceleração estimada e a partir disso, a velocidade e posição estimada.

A arquitetura e estrutura do código é dado da seguinte forma:
O código main.c contém toda a estrutura da leitura dos dados, calculo da velocidade e posição e também da saída de dados, sendo divida
entre 3 threads:
    1. Aquisição do Sinal do acelerômetro (extrai a aceleração ocasionada pela placa)
    2. Cálculo da velocidade e posição, utilizando a integração por Trapézios 
    3. Envio dos três dados pela serial

Lógica e Arquitetura adotada: Para a comunicação entre as threads, foi utilizado variáveis globais para compartilhar os dados entre as 
tarefas. O escalonador do Zephyr gerencia as threads por prioridade, sendo da ordem mais importante para a menos: "leitura do sensor" 
(garante que não se perca amostras) >> "cálculo" (processa os dados de leitura) >> "envio serial" (apenas imprime o que já foi calculado)

```shell
Observações: 
             1° e 2° Commit contém apenas a estrutura padrão do acelerômetro, sobretudo, na parte de extração do sinal.
             3° Commit apresenta o código funcional e pronto do acelerômetro, mas faltando detalhes e retoques para adptação aos
arquivos python.
             4° Commit apresenta o código adptado e funcional para os arquivos python, agora é possível extrair os dados e visualizar
por meio de gráfico.
             5° Commit é a versão final do código, nele, foi corrigido uma interpretação de que a placa está sempre em queda livre.
Agora a placa apresenta um comportamento semelhante com a realidade

Adpatações feitas:
             O arquivo leitor_serial.py foi alterado sob sugestões de IA generativa para que pudesse permitir registrar os valores
seriais no arquivo .csv, tal arquivo armaneza a saída serial para ser lida pela geradora de gráfico, a sua versão anterior não permitia
que lesse as impressões no serial, a alteração feita fez revogar isso e permitir qualquer tipo de leitura.
             Para o código final, as adaptações feitas para suavizar e apaziguar o efeito "queda livre" foram feitas de maneira opcional,
afim de complementar a tarefa encarregada.

Como executar:
             Como uma placa freedom em mãos, basta realizar build e compilar para a placa - verifique de ter limpado seu framework com
"clear". Uma vez realizado isso, insira no terminal o seguinte comando:             
              
             "python leitor_serial.py"

             Em seguida, realize as medidas conforme vai movimentando a placa e encerre a leitura apertando CRTL + C. Uma vez isso, os
dados imprimidos no terminal pararão em um novo arquivo criado chamado de "dados_cinematicos.csv". É neste arquivo que o segundo arquivo
python entrará em ação.

             Tendo concluido isto, basta novamente colocar no terminal:
             
             "python gerar_graficos.py"

             E assim criará um novo arquivo de um gráfico contendo os valores registrados no movimento da placa.
     
