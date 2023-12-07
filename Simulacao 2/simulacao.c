#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include "heap.c"

/*
tempo_servico = atraso_transmissão (tamanho_pacote (188)/largura_banda (1Gbps))

Obs: tamanho_pacote (188 bytes)

intervalo_chegada: distribuição exponencial (média deve ser ajustada).
Sempre que o usuário chega, a ligação inicia imediatamente

nova_ligacao: deve durar 2 minutos e irá enviar pacotes a cada 20 milissegundos.

“Quantas pessoas eu preciso ter em média conectadas nesta rede de modo a obter cada uma das
ocupações: 60, 80, 90 e 99%?”

*/

typedef struct {
    double ocupacao;
    double tempo_simulacao;
    double qtd_pessoas_ativas;
    double intervalo_conexao;
} parametros;

typedef struct {
    double tempo_duracao;
    double tempo_prox_pacote;       // vai ser o indice da heap
} conexao;

void le_parametros(parametros * params){
    printf("Informe a ocupacao desejada (Ex. 0.6, 0.8, 0.9, 0.99 ...): ");
    scanf("%lF", &params->ocupacao);

    /* Quantas pessoas precisam estar ativas (em média) de modo a gerarmos 0.6, 0.8, 0.9 ou 0.99 Gbps? */
    params->qtd_pessoas_ativas = params->ocupacao/0.0000752;

    /* Quantas pessoas por segundo devem ser geradas para que ao final dos 2 minutos alcance a ocupacao desejada? */
    params->intervalo_conexao = params->qtd_pessoas_ativas/120.0;

    printf("Informe o tempo a ser simulado (s): ");       // 864000s = 10 dias
    scanf("%lF", &params->tempo_simulacao);

    printf("Pessoas ativas: %.10lF\n", params->qtd_pessoas_ativas);
    printf("Intervalo de conexao (qtd pessoas/s): %.10lF\n", params->intervalo_conexao);
}

double uniforme() {
	double u = rand() / ((double) RAND_MAX + 1);
	//limitando entre (0,1]
	u = 1.0 - u;

	return (u);
}

int main()
{
    int semente = time(NULL);
    srand(semente);

    parametros params;
    le_parametros(&params);

    double tempo_decorrido = 0.0;
    double tempo_chegada = (-1.0/params.intervalo_conexao) * log(uniforme()); // primeira conexao
    printf("Tempo de chegada: %.10lF\n", tempo_chegada);

    double tempo_saida = 120;
    double tempo_coleta = 10.0;
    double soma_ocupacao = 0.0;
    double nova_conexao = 0.0;

    int nos = 10;
    printf("Insrindo nos: ");
    for (int i = 0; i < nos; i++) {
        printf("%d ", i); // imprime o valor do topo
        insere(i);
    }
    printf("\n");

    for (int i = 0; i < nos; i++) {
        printf("Removendo: ");
        printf("%d\n", heap[1]); // imprime o valor do topo
        deleta(1);
    }

    while (tempo_decorrido < params.tempo_simulacao) {
        //Remover o menor evento entre todos

        if (no.evento == nova_conexao) {        // nova conexao sera um tipo de evento
            //criar conexao (alocar nova pessoa que vai alocar dados a cada 20 milissegundos)
                    // alocar
                    // gerar o tempo de duração (2 min)

            // gerar o tempo de quando começa a próxima conexão

        } else if (tempo_decorrido == chegada_pct) {
            /* usar arvore heap  com a nova_conexao e os 7978,... pessoas (o minimo está sempre na raiz) */
            tempo_atual += 0.02;
            // so adiciona na arvore se o tempo nao ultrapassar os 2 min daquela conexao
        } else if (tempo_decorrido == saida_pacote) {
            /* code */
        } else if (tempo_decorrido == tempo_coleta) {
            /* code */
        }
    }
    
    return 0;


}



