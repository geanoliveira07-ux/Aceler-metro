#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdlib.h>

// Obter referência do dispositivo usando o nodelabel
static const struct device *const accel = DEVICE_DT_GET(DT_NODELABEL(mma8451q));

//==================================================================================================================
// 1. VARIÁVEIS GLOBAIS PARA COMPARTILHAR OS DADOS ENTRE AS THREADS
	double ax = 0, ay = 0, az = 0;
	double vx = 0, vy = 0, vz = 0;
	double px = 0, py = 0, pz = 0;
	uint32_t tempo_ms = 0;  // Contador de tempo em milissegundos

//==================================================================================================================
// 2. THREADS DE OPERAÇÃO
void thread_aquisicao(void)
{
    struct sensor_value accel_x, accel_y, accel_z;
    int ret;

    // Verificar se o dispositivo está pronto
    if (!device_is_ready(accel)) {
        printk("ERRO: Acelerometro nao esta pronto!\n");
        return;
    }

    // Pequeno delay antes de começar a enviar dados
    k_sleep(K_MSEC(1000));

    while (1) {
        // Solicitar leitura do sensor
        ret = sensor_sample_fetch(accel);
        if (ret) {
            printk("Erro ao ler sensor: %d\n", ret);
            k_sleep(K_MSEC(500));
            tempo_ms += 500;
            continue;
        }

        // Obter valores dos eixos X, Y e Z
        sensor_channel_get(accel, SENSOR_CHAN_ACCEL_X, &accel_x);
        sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Y, &accel_y);
        sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Z, &accel_z);

		ax = sensor_value_to_double(&accel_x);
		ay = sensor_value_to_double(&accel_y);
		az = sensor_value_to_double(&accel_z);

        // Aguardar 500ms antes da próxima leitura
        k_sleep(K_MSEC(500));
        tempo_ms += 500;
	}
}

void thread_calculo(void)
{
	// O tempo entre as leituras é de 500ms, ou seja, 0.5 segundos (dt = 0.5)
    double dt = 0.5;

    // Variáveis para guardar a aceleração e velocidade da leitura "anterior"
	double ax_ant = 0, ay_ant = 0, az_ant = 0;
	double vx_ant = 0, vy_ant = 0, vz_ant = 0;

    k_sleep(K_MSEC(1100));		// Espera a Thread 1 fazer a primeira leitura antes de começar

    while (1) {
        // Cálculo da Velocidade: v = v_anterior + (área do trapézio da aceleração)
		vx = vx_ant + (dt / 2,0) * (ax + ax_ant);
		vy = vy_ant + (dt / 2,0) * (ay + ay_ant);
		vz = vz_ant + (dt / 2,0) * (az + az_ant);

        // Cálculo da Posição: p = p_anterior + (área do trapézio da velocidade)
		px = px + (dt / 2,0) * (vx + vx_ant);
		py = py + (dt / 2,0) * (vy + vy_ant);
		pz = pz + (dt / 2,0) * (vz + vz_ant);

        // Guarda os valores atuais para usar na próxima conta como "anteriores"
		ax_ant = ax, ay_ant = ay, az_ant = az;
		vx_ant = vx, vy_ant = vy, vz_ant = vz;

        // Roda a cada 500ms, acompanhando a leitura
        k_sleep(K_MSEC(500));
	}
}

void thread_envio_serial(void)
{
	k_sleep(K_MSEC(1200));
    printk("========================================\n");
    printk("  FRDM-KL25Z - Acelerometro MMA8451Q\n");
	printk("     Iniciando envio de dados...\n");
    printk("========================================\n");

	while (1) {
		// Imprime os valores com algumas casas decimais
		printk("T: %u ms | Acel(x,y,z): %.2f, %.2f, %.2f 
						 | Vel(x,y,z): %.2f, %.2f, %.2f 
						 | Pos(x,y,z): %.2f, %.2f, %.2f\r\n", 
				tempo_ms, ax, ay, az, 
						  vx, vy, vz, 
						  px, py, pz);
	
		k_sleep(K_MSEC(500));
	}
}

void main(void) {}

	// Tamanho da pilha para cada thread
	#define STACK_SIZE 1024

	// Prioridades 
	#define PRIORIDADE_AQUISICAO	5
	#define PRIORIDADE_CALCULO		6
	#define PRIORIDADE_ENVIO		7

	K_THREAD_DEFINE(aquisicao_id, STACK_SIZE, thread_aquisicao, NULL, NULL, NULL, PRIORIDADE_AQUISICAO, 0, 0);
	K_THREAD_DEFINE(calculo_id, STACK_SIZE, thread_calculo, NULL, NULL, NULL, PRIORIDADE_CALCULO, 0, 0);
	K_THREAD_DEFINE(envio_serial_id, STACK_SIZE, thread_envio_serial, NULL, NULL, NULL, PRIORIDADE_ENVIO, 0, 0);