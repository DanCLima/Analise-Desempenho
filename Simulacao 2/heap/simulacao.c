#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "heap.h"

#define NOVA_CONEXAO 0
#define CHEGADA_PACOTE 1
#define SAIDA_PACOTE 2
#define TEMPO_COLETA 3

typedef struct
{
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
} little;

void inicia_litte(little *l)
{
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

typedef struct
{
    double ocupacao;
    double tempo_simulacao;
    double qtd_pessoas_ativas;
    double media_chegada;
} Parametros;

double uniforme()
{
    double u = rand() / ((double)RAND_MAX + 1);
    // limitando entre (0,1]
    u = 1.0 - u;

    return (u);
}

void le_parametros(Parametros *params)
{
    printf("Informe a ocupacao: ");
    scanf("%lF", &params->ocupacao);

    printf("Informe o tempo a ser simulado: ");
    scanf("%lF", &params->tempo_simulacao);

    /* Quantas pessoas precisam estar ativas (em média) de modo a gerarmos 0.6, 0.8, 0.9 ou 0.99 Gbps? */
    params->qtd_pessoas_ativas = params->ocupacao / 0.0000752;

    /* Quantas pessoas por segundo devem ser geradas para que ao final dos 2 minutos alcance a ocupacao desejada? */
    params->media_chegada = params->qtd_pessoas_ativas / 120.0;

    printf("Pessoas ativas: %.10lF\n", params->qtd_pessoas_ativas);
    printf("Media chegada (qtd pessoas/s): %.10lF\n", params->media_chegada);
}

int main()
{
    int semente = time(NULL);
    srand(semente);

    Evento evento;
    Parametros params;
    le_parametros(&params);

    double intervalo_chegada = 1.0 / params.media_chegada;
    double tempo_servico = (188.0 * 8.0) / 1.0e9;
    double tempo_saida = 120.0;
    double tempo_coleta = 10.0;
    double tempo_decorrido = 0.0;
    double nova_conexao = (-1.0 / (1.0 / intervalo_chegada)) * log(uniforme()); // primeira conexao

    double soma_ocupacao = 0.0;
    double e_n_calculo = 0.0;
    double e_w_calculo = 0.0;
    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    little e_n;
    little e_w_chegada;
    little e_w_saida;

    inicia_litte(&e_n);
    inicia_litte(&e_w_chegada);
    inicia_litte(&e_w_saida);

    FILE *fptr;
    fptr = fopen("result.csv", "w");
    if (fptr == NULL)
    {
        puts("File not found!");
        exit(1);
    }
    fputs("segundo,erro_little\n", fptr);

    /* Gerando o primeiro evento, que será uma nova conexão e adicionando na heap */
    evento.tipo_evento = NOVA_CONEXAO;
    evento.tempo_evento = nova_conexao;
    evento.tempo_duracao = 0;
    insere(evento);

    evento.tipo_evento = TEMPO_COLETA;
    evento.tempo_evento = tempo_coleta;
    evento.tempo_duracao = 0;
    insere(evento);

    while (tempo_decorrido < params.tempo_simulacao)
    {
        // Remover o menor evento entre todos
        evento = heap[1];
        tempo_decorrido = evento.tempo_evento;
        deleta(1);

        if (evento.tipo_evento == NOVA_CONEXAO)
        {

            // A nova conexão foi adicionada na heap e tornou-se chegada de pacotes
            evento.tipo_evento = CHEGADA_PACOTE;
            evento.tempo_evento = tempo_decorrido;
            evento.tempo_duracao = tempo_decorrido + (-1.0 / (1.0 / tempo_saida)) * log(uniforme());
            insere(evento);

            // Criação da próxima conexão
            evento.tipo_evento = NOVA_CONEXAO;
            evento.tempo_evento = tempo_decorrido + (-1.0 / (1.0 / intervalo_chegada)) * log(uniforme());
            evento.tempo_duracao = 0;
            insere(evento);
        }
        else if (evento.tipo_evento == CHEGADA_PACOTE)
        {

            evento.tempo_evento += 0.02;

            if (evento.tempo_evento < evento.tempo_duracao)
            {
                insere(evento);
            }

            if (!fila)
            {
                evento.tipo_evento = SAIDA_PACOTE;
                evento.tempo_evento = tempo_decorrido + tempo_servico;
                evento.tempo_duracao = 0;
                insere(evento);

                soma_ocupacao += tempo_servico;
            }
            fila++;

            max_fila = fila > max_fila ? fila : max_fila;

            // E[N]
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos++;
            e_n.tempo_anterior = tempo_decorrido;

            // E[W]
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_chegada.no_eventos++;
            e_w_chegada.tempo_anterior = tempo_decorrido;
        }
        else if (evento.tipo_evento == SAIDA_PACOTE)
        {

            fila--;
            if (fila)
            {
                evento.tipo_evento = SAIDA_PACOTE;
                evento.tempo_evento = tempo_decorrido + tempo_servico;
                evento.tempo_duracao = 0;
                insere(evento);

                soma_ocupacao += tempo_servico;
            }

            // E[N]
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos--;
            e_n.tempo_anterior = tempo_decorrido;

            // E[W]
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.no_eventos++;
            e_w_saida.tempo_anterior = tempo_decorrido;
        }
        else if (evento.tipo_evento == TEMPO_COLETA)
        {
            evento.tipo_evento = TEMPO_COLETA;
            evento.tempo_evento = tempo_decorrido + tempo_coleta;
            evento.tempo_duracao = 0;
            insere(evento);

            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

            e_n.tempo_anterior = tempo_decorrido;
            e_w_chegada.tempo_anterior = tempo_decorrido;
            e_w_saida.tempo_anterior = tempo_decorrido;

            e_n_calculo = e_n.soma_areas / tempo_decorrido;

            e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            double lambda = e_w_chegada.no_eventos / tempo_decorrido;

            fprintf(fptr, "%d,%.20lF\n", (int)tempo_decorrido, fabs(e_n_calculo - lambda * e_w_calculo));
        }
        else
        {
            printf("Evento invalido!\n");
            return 1;
        }
    }

    printf("Ocupacao Calculada: %lF\n", params.ocupacao * 100);
    printf("Ocupacao Simulada: %lF\n", (soma_ocupacao / params.tempo_simulacao) * 100);
    printf("E[N]: %lF\n", e_n_calculo);
    printf("E[W]: %lF\n", e_w_calculo);
    printf("Max fila: %ld\n", max_fila);
    return 0;
}