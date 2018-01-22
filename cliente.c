#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/* inicialização dos métodos */
char* menu_principal(void);
char jogar_player();
/* fim */

/* método para tratamento de erros */
void erro(char* msg) {
	printf("Erro: %s\n\n", msg);
	exit(-1);
}

/* config padrão do cliente */
int config_cli() {
	int port = 4400; // declarar a porta de conexão
	char msg_aut[50] = "Conexão detectada."; // declarar msg de conexão autorizada
	int cli_sock;
	struct sockaddr_in cli_addr; // estrutura do cliente
	socklen_t cli_addlen = sizeof(cli_addr);

	memset(&cli_addr, 0, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET; // define a família da estrutura do cliente
	cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // define o IP do cliente
	cli_addr.sin_port = htons(port); // define a porta para a estrutura do cliente

	if ((cli_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) erro("socket"); // cria o socket

	if (connect(cli_sock, (struct sockaddr*) &cli_addr, sizeof(cli_addr)) < 0) { // verificar se conexão foi estabelecida
		erro("connect"); // se der erro, imprimir isso
	}
	else { // senão...
		write(cli_sock, msg_aut, sizeof(msg_aut)); // escrever a msg de autorização enviada pelo servidor
	}
	return cli_sock; // retornar o descritor
}

/* método de execução do cliente */
void cliente(int cli_sock) {
	char msg_servidor[255]; // para msgs que serão enviadas ao servidor
	char msg_cliente[255]; // para msgs que serão recebidas no cliente

	int jogar_novamente_int = 0; // variável de controle


	printf("Digite seu nickname: "); // entrada do nickname do jogador
	scanf("%s", msg_servidor); // ler o nickname digitado
	fflush(stdin); // limpa o buffer do teclado
	sleep(1); // load para verificar o nickname digitado

	printf("Nickname válido.\n"); // se o nickname estiver ok, então imprime isso
	write(cli_sock, msg_servidor, sizeof(msg_servidor)); // escreve para o servidor
	sleep(2);

	int opc_jogar_nov; // variável de controle
	int numero_rodada = 0; // número da rodada (1, 2, 3, etc)

	do {
		numero_rodada++; // incrementa numero_rodada
		printf("\n### RODADA %d ####\n", numero_rodada); // imprime na tela em qual rodada a partida está
		sleep(2); // load para iniciar o menu principal

		char* escolha_string = malloc(1); // aloca memória para a variável escolha_string
		escolha_string = menu_principal(); // escolha_string armazena retorno do método menu_principal
		sleep(1); // load para imprimir escolha do player no menu principal

		int escolha_int = atoi(escolha_string);  // converte a escolha_string em int
		if (escolha_int < 2) { // compara o resultado
			printf("Você escolheu par.\n"); // se escolha = 1, imprime isso no console
		}
		else {
			printf("Você escolheu ímpar.\n"); // senão, se escolha = 2, imprime isso no console
		}

		write(cli_sock, escolha_string, sizeof(escolha_string)); // escreve para o servidor a escolha do player

		char numero_jogado_player = jogar_player(); // armazena retorno do método jogar_player
		char* numero_jogado = &numero_jogado_player; // armazena um char num char*
		write(cli_sock, numero_jogado, sizeof(numero_jogado)); // escreve número jogado em char* para o servidor
		sleep(10); // load para esperar ser carregado todo conteúdo do servidor

		printf("\nDeseja jogar novamente?\n1- Sim\n2- Não\n\n");
		printf("Digite uma opção: ");
		scanf("%d", &opc_jogar_nov);

		if (opc_jogar_nov == 2) { // se opção selecionada for igual a 2, sai do jogo
			printf("Você escolheu não jogar novamente.\n\n"); // sinaliza que a opção 2 foi selecionada
			sleep(2); // load

			printf("Aguarde enquanto o jogo está sendo encerrado...\n"); // sinaliza que o jogo está sendo encerrado
			sleep(2); // load

			printf("Jogo encerrado com sucesso.\n\n"); // sinaliza que o jogo foi finalizado com sucesso
			sleep(2); // load

			char* msg_sair = "sair"; // armazena a string "sair"
			write(cli_sock, msg_sair, sizeof(msg_sair)); // escreve para o servidor "sair" e assim sinalizar que o player saiu do jogo

			close(cli_sock); // fecha o socket do cliente
			exit(-1); // encerra o programa
		}

		system("clear"); // limpa o terminal se uma nova rodada for iniciada
	} while(opc_jogar_nov == 1); // manter o loop enquanto uma nova rodada for iniciada
} // fim do método cliente

/* método para iniciar o menu principal */
char* menu_principal(void) {
  int opc; // armazena opção escolhida pelo player
	char* msg_escolha = malloc(1); // aloca memória para armanezar texto em msg_escolha

	/* imprime no console a sequência de textos */
	printf("\nEscolha uma das opções abaixo:\n"); // orienta o player
	printf("1 - Par\n"); // 1 para par
	printf("2 - Ímpar\n\n"); // 2 para ímpar

	printf("Digite uma opção: "); // imprime orientação do que o player deve fazer
	scanf("%d", &opc); // ler a opção escolhida

	switch (opc) {
		case 1: // caso opc seja 1
			strcpy(msg_escolha, "1"); // copia "1" para a variável msg_escolha
			return msg_escolha; // retorna "1"
			break;
		case 2: // caso opc seja 2
			strcpy(msg_escolha, "2"); // copia "2" para a variável msg_escolha
			return msg_escolha; // retorna "2"
			break;
		case 0: // caso opc seja 0
			strcpy(msg_escolha, "Você escolheu sair do jogo. Aguarde"); // copia o texto para a variável msg_escolha
			return msg_escolha; // retona a msg copiada
			sleep(2); // load para encerrar o programa

			exit(-1); // encerra o programa
		default:
			break;
	}
} // fim do método menu_principal

/* método para iniciar a jogada do player */
char jogar_player() {
	int numero_player; // armazena o número jogado pelo player em int
	char numero_player_char; // armazena o número jogado pelo player em char

	printf("\nDigite um número de 0 a 5: "); // imprime no console para sinalizar que o player deve escolher um número de 0 a 5
  scanf("%d", &numero_player); // ler o número escolhido pelo player

	sprintf(&numero_player_char, "%d", numero_player); // converte o valor em int para char

  if(numero_player > 5 || numero_player < 0) { // verifica se o número escolhido pelo player não foi menor que 0 ou maior que 5
    printf("Número inválido. Aguarde para tentar novamente.\n"); // imprime no console para sinalizar que o número digitado pelo player é inválido
    sleep(2); // load para que o player jogue novamente

    jogar_player(); // retorna ao início do método
  }

  return numero_player_char; // retorna o número escolhido pelo player desde que seja >= 0 ou <= 5
} /* fim do método jogar_player */

/* método main */
int main() {
	system("clear"); // limpar o console ao iniciar o cliente

	/* variáveis locais */
	struct sockaddr_in cli_addr; // cria a estrutura do cliente
	socklen_t cli_addrlen = sizeof(cli_addr); // define o tamanho da estrutura
	int cli_sock; // descritor do socket do cliente
	/* fim */

	cli_sock = config_cli(); // retorna o socket do cliente configurado

	/* loading principal do jogo */
  printf("Aguarde enquanto o jogo está sendo carregado...\n"); // imprime para sinalizar que o jogos está sendo carregado no cliente
  sleep(2); // load antes de imprimir as boas vindas ao player

	printf("\n********* Bem vindo ao Jogo do Par ou Ímpar *********\n\n"); // imprime as boas vindas
  sleep(2); // load antes da configuração do socket do cliente
  /* fim */

	cliente(cli_sock); // chama método cliente passando socket configurado como parâmetro

	/* ao finalizar o programa */
	close(cli_sock); // encerra o socket
	exit(-1); // encerra a execução do programa
} // fim do método main
