#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <ctime>
#include <queue>

using namespace std;

typedef struct Pacote
{
    double tempo;
    double duracao;
    struct Node *next;
} Pacote;

typedef struct
{
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
} little;

void inicia_little(little *l)
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

double min(double d1, double d2)
{
    return d1 < d2 ? d1 : d2;
}

double uniforme()
{
    double u = rand() / ((double)RAND_MAX + 1);
    u = 1.0 - u;

    return (u);
}

void le_parametros(Parametros *params)
{
    cout << "Informe a ocupação: ";
    cin >> params->ocupacao;
    cout << endl;

    /* Quantas pessoas precisam estar ativas (em média) de modo a gerar
    mos 0.6, 0.8, 0.9 ou 0.99 Gbps? */
    params->qtd_pessoas_ativas = params->ocupacao / 0.0000752;

    /* Quantas pessoas por segundo devem ser geradas para que ao final dos 2 minutos alcance a ocupacao desejada? */
    params->media_chegada = params->qtd_pessoas_ativas / 120.0;

    cout << "Informe o tempo de simulacao: ";
    cin >> params->tempo_simulacao;
    cout << endl;

    cout << "Pessoas ativas: " << fixed << params->qtd_pessoas_ativas << endl;
    cout << "Media chegada (qtd pessoas/s): " << fixed << params->media_chegada << endl;
}

int main()
{
    int semente = time(NULL);
    srand(semente);

    Parametros params;
    le_parametros(&params);

    double DBL_MAX = numeric_limits<double>::max();
    
    double tempo_servico = (188.0 * 8.0) / 1.0e9; // atraso de transmissao

    double tempo_saida = 120.0;
    double tempo_decorrido = 0.0;
    
    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    little e_n;
    little e_w_chegada;
    little e_w_saida;

    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);

    double soma_ocupacao = 0.0;
    double e_n_calculo = 0.0;
    double e_w_calculo = 0.0;

    ofstream fptr("result_99.csv");
    if (!fptr.is_open())
    {
        cout << "File not found!" << endl;
        return 1;
    }
    fptr << "segundo,erro_little\n";

    queue<Pacote> q;
    Pacote pacote;

    double nova_conexao = (-1.0 / params.media_chegada) * log(uniforme());
    double tempo_coleta = 10.0;
    double chegada_pacote = DBL_MAX;
    double saida_pacote = DBL_MAX;

    while (tempo_decorrido < params.tempo_simulacao)
    {

        if (q.empty())
        {
            chegada_pacote = DBL_MAX;
        }
        else
        {
            pacote = q.front();
            chegada_pacote = pacote.tempo;
        }
        tempo_decorrido = min(min(min(nova_conexao, chegada_pacote), saida_pacote), tempo_coleta);

        if (tempo_decorrido == nova_conexao)
        {
            
            // Adiciona o tempo do pacote na fila
            q.push({tempo_decorrido + 0.02, tempo_decorrido + 0.02 + (-1.0 * tempo_saida) * log(uniforme()), nullptr});

            // Cria a próxima conexão
            nova_conexao = tempo_decorrido + (-1.0 / params.media_chegada) * log(uniforme());
        }
        else if (tempo_decorrido == chegada_pacote)
        {
            q.pop();
            pacote.tempo += 0.02;

            if (pacote.tempo < pacote.duracao)
            {
                q.push({pacote.tempo, pacote.duracao, nullptr});
            }

            if (!fila)
            {
                saida_pacote = tempo_decorrido + tempo_servico;

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
        else if (tempo_decorrido == saida_pacote)
        {

            fila--;
            if (fila)
            {
                saida_pacote = tempo_decorrido + tempo_servico;

                soma_ocupacao += tempo_servico;
            }
            else
            {
                saida_pacote = DBL_MAX;
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
        else if (tempo_decorrido == tempo_coleta)
        {
            tempo_coleta += 10.0;

            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

            e_n.tempo_anterior = tempo_decorrido;
            e_w_chegada.tempo_anterior = tempo_decorrido;
            e_w_saida.tempo_anterior = tempo_decorrido;

            e_n_calculo = e_n.soma_areas / tempo_decorrido;

            e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            double lambda = e_w_chegada.no_eventos / tempo_decorrido;

            fptr << (int)tempo_decorrido << "," << fixed << setprecision(20) << fabs(e_n_calculo - lambda * e_w_calculo) << endl;
        }
        else
        {
            cout << "Evento invalido!" << endl;
            return 1;
        }
    }

    fptr.close();

    cout << "Ocupacao Calculada: " << fixed << params.ocupacao * 100.0 << endl;
    cout << "Ocupacao Simulada: " << fixed << (soma_ocupacao / params.tempo_simulacao) * 100.0 << endl;
    cout << "E[N]: " << fixed << e_n_calculo << endl;
    cout << "E[W]: " << fixed << e_w_calculo << endl;
    cout << "Max fila: " << max_fila << endl;

    return 0;
}
