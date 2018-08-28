/*
	Simple http server (with multiple connections)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
 
#define BUFLEN	1024	//Max length of buffer
#define PORT	8000	//The port on which to listen for incoming data


char http_ok[] = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\nServer: Test\r\n\r\n";
char http_error[] = "HTTP/1.0 400 Bad Request\r\nContent-type: text/html\r\nServer: Test\r\n\r\n";

//html page structure
char htmlTop[] = "<html>\n<head>\n<meta charset=\"UTF-8\">\n<title>\nInformações do Kernel\n</title>\n</head>\n<body>\n<p>Uptime(em segundos): ";
char htmlBottom[] = "</p>\n</body>\n</html>\n";

//html page
char page[4096];


void die(char *s)
{
	perror(s);
	exit(1);
}

void writePage(void)
{
	
	page[0] = '\0';
	strcat(page, htmlTop);
	printf("page = %s\n", page);
	
	FILE *arq = fopen("/proc/uptime","rb");
	if(arq==0) {
		printf("falha na abertura do arquivo.\n");
		exit(1);
	}
	char *str1 = 0;
	size_t size = 0;
	while(getline(&str1, &size, arq) != -1)
	{
		puts(str1);
		strcat(page, str1);
		strcat(page, "<br>");
	}
	strcat(page, "</p><p>Informações do Processador<br>-------<br>");
	free(str1);
	fclose(arq);
	
	printf("page 1:\n%s\n", page);
	
	FILE *arq2 = fopen("/proc/cpuinfo","rb");
	if(arq2 == 0) {
		printf("falha na abertura do arquivo.\n");
		exit(1);
	}
	char *str2 = 0;
	size = 0;
	while(getline(&str2, &size, arq2) != -1)
	{
		puts(str2);
		strcat(page, str2);
		strcat(page, "<br>");
	}
	free(str2);
	fclose(arq2);
	
	
	FILE *arq3 = fopen("/proc/meminfo","rb");
	if(arq3 == 0) {
		printf("falha na abertura do arquivo.\n");
		exit(1);
	}
	char *str3 = 0;
	size = 0;
	strcat(page, "</p><p>Informações de Memória<br>-------<br>");
	while(getline(&str3, &size, arq3) != -1)
	{
		puts(str3);
		strcat(page, str3);
		strcat(page, "<br>");
	}
	free(str3);
	fclose(arq3);
	
	
	
	FILE *arq4 = fopen("/proc/version","rb");
	if(arq4 == 0) {
		printf("falha na abertura do arquivo.\n");
		exit(1);
	}
	char *str4 = 0;
	size = 0;
	strcat(page, "</p><p>Versão do Linux<br>-------<br>");
	while(getline(&str4, &size, arq4) != -1)
	{
		puts(str4);
		strcat(page, str4);
		strcat(page, "<br>");
	}
	free(str4);
	fclose(arq4);
	
	FILE *arq5 = popen("date", "r");
	if(arq5 == 0) {
		printf("falha na abertura do arquivo.\n");
		exit(1);
	}
	char *str5 = 0;
	size = 0;
	strcat(page, "</p><p>Data do sistema<br>-------<br>");
	while(getline(&str5, &size, arq5) != -1)
	{
		puts(str5);
		strcat(page, str5);
		strcat(page, "<br>");
	}
	free(str5);
	fclose(arq5);
	
	FILE *arq6 = popen("ps", "r");
	if(arq6 == 0) {
		printf("falha na abertura do arquivo.\n");
		exit(1);
	}
	char *str6 = 0;
	size = 0;
	strcat(page, "</p><p>Processos<br>-------<br>");
	while(getline(&str6, &size, arq6) != -1)
	{
		puts(str6);
		strcat(page, str6);
		strcat(page, "<br>");
	}
	free(str6);
	fclose(arq6);
	
	FILE *arq7 = popen("top", "r");
	if(arq7 == 0) {
		printf("falha na abertura do arquivo.\n");
		exit(1);
	}
	char *str7 = 0;
	size = 0;
	strcat(page, "</p><p>Uso do CPU<br>-------<br>");
	getline(&str7, &size, arq7);
	//strcat(page, str7);
	getline(&str7, &size, arq7);
	puts(str7);
	strcat(page, str7);
	strcat(page, "<br>");
	free(str7);
	fclose(arq7);
	
	
	

	strcat(page, htmlBottom);
	
	printf("page final:\n%s\n", page);
}

/*
void writePage(void)
{
	FILE *arq = fopen("/proc/uptime","rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, arq) != -1)
	{
		puts(arg);
	}
	strcat(page, arg);
	strcat(page, "</p><p>");
	free(arg);
	fclose(arq);
	
	
	
	FILE *arq2;	
	if((arq2 = fopen("/proc/cpuinfo","rb"))==0) {
		printf("falha de abertura do arquivo.\n");
		exit(1);
	}
	char *arg2 = 0;
	size = 0;
	while(getdelim(&arg2, &size, 0, arq2) != -1)
	{
		puts(arg2);
	}
	puts("1");
	puts(page);
	strcat(page, arg2);
	puts("2");
	strcat(page, "</p><p>");
	puts("3");
	free(arg2);
	puts("4");
	fclose(arq2);
	puts("5");
	printf("%s", page);
	puts("6");
	

	strcat(page, "</p>\n</body>\n</html>\n");
}
*/
 
int main(void)
{
	struct sockaddr_in si_me, si_other;

	int s, i, slen = sizeof(si_other) , recv_len, conn, child = 0;
	char buf[BUFLEN];
	pid_t pid;
     
	/* create a TCP socket */
	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		die("socket");
    
	/* zero out the structure */
	memset((char *) &si_me, 0, sizeof(si_me));
     
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
	/* bind socket to port */
	if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
		die("bind");
	
	/* allow 10 requests to queue up */ 
	if (listen(s, 10) == -1)
		die("listen");
     
	/* keep listening for data */
	while (1) {
		memset(buf, 0, sizeof(buf));
		printf("Waiting a connection...");
		fflush(stdout);
		
		conn = accept(s, (struct sockaddr *) &si_other, &slen);
		if (conn < 0)
			die("accept");

		if ((pid = fork()) < 0)
			die("fork");
		else if (pid == 0) {
			close(s);

			writePage();
			
			printf("Client connected: %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

			/* try to receive some data, this is a blocking call */
			recv_len = read(conn, buf, BUFLEN);
			if (recv_len < 0)
				die("read");

			/* print details of the client/peer and the data received */
			printf("Data: %s\n" , buf);
		 
			if (strstr(buf, "GET")) {
				/* now reply the client with the same data */
				if (write(conn, http_ok, strlen(http_ok)) < 0)
					die("write");
				if (write(conn, page, strlen(page)) < 0)
					die("write");
			} else {
				if (write(conn, http_error, strlen(http_error)) < 0)
					die("write");
			}

			exit(0);
		}

		/* close the connection */
		close(conn);

		child++;
		while (child) {
			pid = waitpid((pid_t) -1, NULL, WNOHANG);
			if (pid < 0)
				die("?");
			else if (pid == 0) break;
			else child--;
		}
	}
	close(s);
	
	return 0;
}
