#include <stdio.h>
#include <stdlib.h>
#include <netdb.h> // configs ip, porta, etc
#include <string.h> // tratamento de strings
#include <unistd.h>
#include <pthread.h> // criação e remoção de threads
#include <time.h> // zerar valor da função randômica

/* inicialização dos métodos do jogo */
int verificar_escolha (char* opc_par_impar);
int verificar_numero(int soma);
void dados_rodada(int numero_player, int numero_pc, char* nickname_player);
void placar_jogo(int pontos_player, int pontos_pc);
/* fim */

/* variáveis globais */
int cli_sock; // descritor do socket do cliente
char* nickname_escolhido; // armazena o nickname escolhido pelo player
/* fim das variáveis */

/* variáveis globais para a pontuação do jogo */
int pontos[2]; // vetor para armazenar a pontuação do jogo
/* fim das variáveis */

/* método auxiliar para tratar msgs de erros na execução do programa */
void erro(char* msg) {
	printf("Erro: %s\n\n", msg);
	exit(-1);
}

/* config padrão do servidor */
int config_serv() {
	int serv_sock; // cria o descritor do servidor
	int port = 4400; // declara a porta
	struct sockaddr_in serv_addr; // declara a estrutura do servidor

	if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) erro("socket"); // cria o socket

	memset(&serv_addr, 0, sizeof(serv_addr)); // zera a estrutura
	serv_addr.sin_family = AF_INET; // define a família
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // define os IPs aceitos (INADDR_ANY = todos)
	serv_addr.sin_port = htons(port); // define a porta para conexão

	if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) erro("bind"); // faz o bind
	if (listen(serv_sock, 5) < 0) erro("listen"); // fica na escuta de 5 clienteS

	int yes = 1;
	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) erro("reuso do socket"); // reutilização de socket

	return serv_sock; // retornar o descritor
} // fim do método config_serv

/* método para verificar quem escolheu par ou ímpar */
int verificar_escolha (char* opc_par_impar){
	int opc_par_impar_int = atoi(opc_par_impar); // converte a opção escrita em char* para int

	if (opc_par_impar_int > 1) { // opção 2 = ímpar
		printf("\n%s escolheu ímpar e o PC par\n", nickname_escolhido); // imprimir no console se o player tiver escolhido ímpar
		return opc_par_impar_int; // retorna a opção selecionada opc_par_impar_int = 2
	}
	else { // opção 1 = par
		printf("\n%s escolheu par e o PC ímpar\n", nickname_escolhido); // imprimir no console se o player tiver escolhido par
		return opc_par_impar_int; // retorna a opção selecionada opc_par_impar_int = 1
	}
	return 0;
} // fim do método verificar_escolha

/* método auxiliar para verificar se o resultado da jogada é par ou ímpar */
int verificar_numero(int soma) {
  int resto;
  return resto = soma % 2; // retorna o resto da divisão da soma por 2
} // fim do método verificar_numero

/* método para realizar a jogada do pc */
int jogar_pc() {
  srand(time(NULL)); // reiniciar contador randomico
  int numero_pc = rand() % 6; // gerar número de forma aleatória onde numero_pc >= 0 e numero_pc <= 5

  if (numero_pc < 0 || numero_pc > 5) { // verificar se o número randomico não foi menor que 0 ou maior que 5
    printf("\nO PC jogou um número inválido. Aguarde que ele jogará novamente.\n"); // imprimir msg se numero > 5 ou numero < 0
    sleep(2); // tempinho para um drama
    jogar_pc(); // retornar ao início do método
  }
  return numero_pc; // retorna o número jogado pelo pc
} // fim do método jogar_pc

/* método para imprimir os números escolhidos pelos jogadores (player e pc) */
void dados_rodada(int numero_player, int numero_pc, char* nickname_player) {
	printf("\nDados da jogada:\n"); // imprimir o cabeçalho
	printf("%s jogou o número %d.\n", nickname_player, numero_player); // imprimir número escolhido pelo player
	printf("PC jogou o número %d.\n\n", numero_pc); // imprimir número escolhido pelo pc
} // fim do método dados_rodada

/* método para imprimir o placar geral */
void placar_jogo(int pontos_player, int pontos_pc) {
	printf("Placar:\n"); // cabeçalho do placar
	/* condição para pontuação do player */
	if (pontos_player == 0 || pontos_player > 1) { // se a pontuação do player for igual a 0 ou maior que 1
		printf("%s = %d pontos\n", nickname_escolhido, pontos_player); // imprime o resultado no plural
	}
	else { // se a pontuação do player for igual a 1
		printf("%s = %d ponto\n", nickname_escolhido, pontos_player); // imprime o resultado no singular
	}
	/* fim */

	/* condição para pontuação do pc */
	if (pontos_pc == 0 || pontos_pc > 1) { // se a pontuação do pc for igual a 0 ou maior que 1
		printf("PC = %d pontos\n", pontos_pc); // imprime o resultado no plural
	}
	else { // se a pontuação do pc for igual a 1
		printf("PC = %d ponto\n", pontos_pc); // imprime o resultado no plural
	}
	/* fim */
} // fim do método placar_jogo

/* método para analisar o resultado do jogo (ganhador/perdedor)*/
void resultado_rodada(int resto, int escolha, char* nickname_escolhido) {
	if(resto > 0) { // se o resto for igual a 1
		printf("O resultado final da partida foi ÍMPAR\n"); // o resultado é ímpar

		if (escolha > 1) { // se a escolha de jogada tiver sido 2 = ímpar, então player ganha
			printf("Parabéns. %s, você venceu!\n\n", nickname_escolhido); // imprime msg de vitória
			pontos[0] += 1; // incrementa a pontuação do player
		}
		else { // senão, player perde
			printf("Sinto muito. %s, você perdeu!\n\n", nickname_escolhido); // imprime msg de derrota
			pontos[1] += 1; // incrementa a pontuação do pc
		}
	}
	else { // senão, se resto for igual a 0
		printf("O resultado final da partida foi PAR\n"); // o resultado é par

		if (escolha < 2) { // se a escolha de jogada tiver sido 1 = paar, então player ganhar
			printf("Parabéns. %s, você venceu!\n\n", nickname_escolhido); // imprime msg de vitória
			pontos[0] += 1; // incrementa a pontuação do player
		}
		else { // senão, player perde
			printf("Sinto muito. %s, você perdeu!\n\n", nickname_escolhido); // imprime msg de derrota
			pontos[1] += 1; // incrementa a pontuação do pc
		}
	}
} // fim do método resultado_rodada

/* método principal para capturar e enviar msgs ao cliente */
void* servidor(void* arg) {
	char msg_cliente[255];  // armazena msgs que serão enviadas pelo cliente
	int sock_entrada = *(int*) arg;

	nickname_escolhido = malloc(1); // aloca memória para utilização da variável global nickname_escolhido

	/* variáveis locais de controle */
	int flag_nick = 0; // nickname
	int flag_par_impar = 0; // opção par ou ímpar
	int flag_conexao = 0; // conexão estabelecida
	/* fim das variáveis */

	int escolha_int; // armazena escolha de par ou ímpar do player (1 ou 2, respectivamente)

	for(;;) { // for para manter o servidor sempre ativo

		read(sock_entrada, msg_cliente, sizeof (msg_cliente)); // ler tudo que vier do cliente

		if(strcmp(msg_cliente, "sair") != 0) { // se msg_cliente for diferente de "sair", então:

			if(flag_conexao == 0) { // se a flag_conexao for igual a 0, então:
				printf("%s\n", msg_cliente); // imprime no console que uma nova conexão foi detectada
				flag_conexao++; // incrementa flag_conexao
			} // na segunda volta do for, o programa não deve mais entrar nesse if

			else if (flag_conexao == 1 && flag_nick == 0) { // passando pelo if anterior, o programa deve comparar as condições desse if
				char* msg_nick = " entrou no jogo."; // string complementar

				strcpy(nickname_escolhido, msg_cliente); // copia o nickname escolhido pelo player para a variável nickname_escolhido
				strcat(msg_cliente, msg_nick); // concatena o nickname recebido em msg_cliente + a string complementar

				printf("\n%s\n", msg_cliente); // imprime no console (<nickname> entrou no jogo)
				flag_nick++; // incrementa flag_nick
			} // na terceira volta do for, o programa não deve mais entrar nesse if

			else { // passando pelas duas condições anteriores, o programa deve comparar as condições abaixo

				if (flag_conexao == 1 && flag_nick == 1 && flag_par_impar == 0) { // se essa condição for verdadeira, então:
	 				escolha_int = verificar_escolha(msg_cliente); // armazena retorno do método verificar_escolha (1 - par e 2- ímpar)
					/* imprime a verificação feita no método verificar_escolha */

	 				flag_par_impar++; // incrementa flag_par_impar
				} // na quarta volta do for,o programa não deve mais entrar nesse if

	 			else if (flag_nick == 1 && flag_par_impar == 1) { // passando pelas condições anteriores, o programa deve comparar essa condição
	 				char* numero_player_string = msg_cliente; // armazena o número jogado pelo player
	 				int numero_player_int = atoi(numero_player_string); // converte o valor em char* para int

	 				printf("\n%s jogou!\n", nickname_escolhido); // imprime no console que o player jogou
	 				sleep(2); // load para iniciar a jogada do pc (2 segundos)

	 				int numero_pc_int = jogar_pc(); // armazena o número jogado pelo pc (0 a 5)
	 				printf("PC jogou!\n"); // imprime no console que o pc jogou
	 				sleep(2); // load para iniciar o cálculo da jogada (2 segundos)

	 				printf("\nCalculando o resultado da jogada...\n"); // imprime no console para sinalizar que o cálculo da jogada foi iniciado
	 				int soma_numeros_jogados = numero_player_int + numero_pc_int; // armazena a soma do número jogado pelo player e pelo pc
	 				int resultado_divisao_soma = verificar_numero(soma_numeros_jogados);  // armazena o retorno do método verificar_numero, passando a soma dos números jogados por parâmetro
	 				sleep(2); // load para finalizar o cálculo (2 segundos)

	 				printf("Resultado calculado com sucesso.\n"); // imprime no console para sinalizar que o resultado foi calculado com sucesso
	 				sleep(2); // load para chamar o método dados_rodada

					dados_rodada(numero_player_int, numero_pc_int, nickname_escolhido); // o método dados_rodada é chamado com o número jogado pelo player, o número jogado pelo pc e o nickname do player sendo passados como parâmetros
	 				sleep(2); // load para chamar o método resultado_rodada

					resultado_rodada(resultado_divisao_soma, escolha_int, nickname_escolhido); // o método resultado_rodada é chamado com o resto da divisão da soma dos números jogados pelo player e pelo pc,
					//a escolha feita pelo player (1- par ou 2- ímpar) e o nickname do player sendo passados como parâmetros
					sleep(2); // load para chamar o método placar_jogo

					placar_jogo(pontos[0], pontos[1]); // o método placar_jogo é chamado com a pontuação do player e a pontuação do pc sendo passadas como parâmetros
					flag_par_impar--; // decrementa flag_par_impar
					/* decrementar a variável flag_par_impar possibilita a existência de novas rodadas */
				}
			}
		} // fim do if para caso msg_cliente for diferente de "sair"

		else { // se msg_cliente for igual a "sair", então:
			/* a pontuação do jogo é reiniciada */
			pontos[0] = 0; // zera pontuação do player
			pontos[1] = 0; // zera pontuação do pc

			printf("\n%s saiu do jogo.\n", nickname_escolhido); // imprime no console para sinalizar que o player saiu (<nickname> saiu do jogo)

			close(sock_entrada); // encerra o sock de entrada do cliente
			pthread_exit((void*) 0); // encerra a thread
		}
	} // fim do else para no caso da msg_cliente for igual a "sair"
} // fim do método servidor

/* main */
int main() {
	system("clear"); // limpar o console ao iniciar o servidor

	/* variáveis locais */
	struct sockaddr_in serv_addr; // cria a estrutura do servidor
	int serv_sock; // declarar o socket do servidor
	/* fim das variáveis locais*/

	serv_sock = config_serv(); // armazena o retorno do socket do servidor criado e configurado

	printf("O jogo está sendo carregado...\n"); // imprime no console para sinalizar que o jogo está sendo carregado no servidor
	sleep(2); // tempo para concluir o carregamento do jogo

	printf("Carregamento concluído.\n\n"); // imprime no console para sinalizar que o jogo foi carregado com sucesso
	sleep(2); // tempo para iniciar a verificação por novas conexões

	printf("Esperando conexão...\n"); // imprime no console para sinalizar que o servidor está aguardando uma conexão do cliente

	while(1) { // manter conexão com loop infinito
		/* variáveis do cliente */
		struct sockaddr_in cli_addr; // cria a estrutura do cliente no servidor
		unsigned int cli_len = sizeof(cli_addr); // define o tamanho da estrutura do cliente
		/* fim das variáveis do cliente */

		pthread_t thread; // declara a thread

		if ((cli_sock = accept(serv_sock, (struct sockaddr *) &cli_addr, &cli_len)) < 0) erro("socket cli");
		if (pthread_create(&thread, NULL, servidor, &cli_sock) != 0) erro("thread creation");
		pthread_detach(thread);
	}
	exit(0);
} // fim do método main
