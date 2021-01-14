#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netdb.h>
#include<time.h>

#define BUF_SIZE 1024

char user_id[BUF_SIZE];
char cust_id[BUF_SIZE], cust_id_new[BUF_SIZE], cust[BUF_SIZE];
char amount[BUF_SIZE];
char choice[10], choice1[5];
char buf[BUF_SIZE];
char msg1[100] = "Enter Your User ID And Password";
char msg2[100] = "YES";
char msg3[100] = "NO";
char msg4[100] = "Enter Customer User ID";
char msg5[100] = "Enter Amount To Deposit";
char msg6[100] = "Enter Amount To Withdraw";
char msg7[200] = "Money Deposited Successfully In USER ID - ";
char msg8[200] = "Money Debited From USER ID - ";
char msg9[200] = "Not Enough Money In USER ID - ";
int c = 0;

FILE *fp;

void error_handling(char *message);
void customer_service();
void admin_service();
void credit_money();
void withdraw_money();
void balance_enquiry();
void transaction_history();

int serv_sock, clnt_sock;
struct sockaddr_in serv_addr, clnt_addr;
int main(int argc, char *argv[]){
	socklen_t clnt_addr_size;

	if(argc!=2){
		printf("usage: %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM,0);

	if(serv_sock == -1)
		error_handling("Socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
	if(clnt_sock == -1)
		error_handling("accept() error");
	else
		printf("CLIENT AUTHORIZED\n");

	
	read(clnt_sock, choice, sizeof(choice));
	printf("Client : %s\n", choice);

	if(!strcmp(choice, "1")){
		customer_service();
	}
	else{
		admin_service();
	}

	close(clnt_sock);	
	close(serv_sock);
	return 0;
}

void admin_service(){
	int k= 0;

	write(clnt_sock, msg1,sizeof(msg1));
	read(clnt_sock, user_id, sizeof(user_id));
	printf("Client: %s\n", user_id);
	strcat(user_id, "\n");
	fp = fopen("admin_login.txt", "r");
	if(fp == NULL)
		error_handling("can not open file");
	while(fgets(buf, BUF_SIZE, fp) != NULL){
		if(!strcmp(buf, user_id)){
			write(clnt_sock, msg2, sizeof(msg2));
			c++;
		}
	}
	fclose(fp);
	if(c == 0){
		write(clnt_sock, msg3, sizeof(msg3));
		exit(1);
	}


	read(clnt_sock, choice1, sizeof(choice1));
	printf("Client: %s\n", choice1);
	write(clnt_sock, msg4, sizeof(msg4));
	read(clnt_sock, cust_id, sizeof(cust_id));
	printf("Client: %s\n", cust_id);

	strcpy(cust_id_new, cust_id);
	strcpy(cust, cust_id);

	strcat(cust_id, "\n");
	fp = fopen("customer_id.txt", "r");
	if(fp == NULL)
		error_handling("can not open file");
	while(fgets(buf, BUF_SIZE, fp) != NULL){
		if(!strcmp(buf, cust_id)){
			write(clnt_sock, msg2, sizeof(msg2));
			k++;
		}
	}
	fclose(fp);
	if(k == 0){
		write(clnt_sock, msg3, sizeof(msg3));
		exit(1);
	}

	if(!strcmp(choice1, "1")){
		credit_money();
	}
	else{
		withdraw_money();
	}

}

void credit_money(){
	int x, y = 0, z;
	char date[500] = "";
	write(clnt_sock, msg5, sizeof(msg5));
	read(clnt_sock, amount, sizeof(amount));
	printf("Client: %s\n", amount);

	x = atoi(amount);
	
	strcat(cust_id_new, ".txt");

	fp = fopen(cust_id_new, "r+");
	if(fp == NULL)
		error_handling("can not open file");
	else{
		if(fgets(buf, BUF_SIZE, fp) != NULL){
			y = atoi(buf);
		}
	}
	fclose(fp);


	fp = fopen(cust_id_new, "w");
	if(fp == NULL)
		error_handling("can not open file");
	else{
		z = x + y;
		fprintf(fp,  "%d", z);
	}
	fclose(fp);
	
	write(clnt_sock, msg7, sizeof(msg7));

	struct tm* ptr;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	strcat(date, asctime(ptr));
	strcat(date, "DEPOSIT ");
	strcat(date, amount);
	strcat(date, "\n\n");

	strcat(cust, "_transaction.txt");
	fp = fopen(cust, "a");
	if(fp == NULL)
		error_handling("can not open file");
	fprintf(fp,  "%s", date);
	fclose(fp);
}

void withdraw_money(){
	int x, y = 0, z;
	char date[500] = "";
	write(clnt_sock, msg6, sizeof(msg6));
	read(clnt_sock, amount, sizeof(amount));
	printf("Client: %s\n", amount);
	
	x = atoi(amount);

	strcat(cust_id_new, ".txt");

	fp = fopen(cust_id_new, "r+");
	if(fp == NULL)
		error_handling("can not open file");
	else{
		if(fgets(buf, BUF_SIZE, fp) != NULL){
			y = atoi(buf);
		}
	}
	fclose(fp);

	if(y >= x){
		fp = fopen(cust_id_new, "w");
		if(fp == NULL)
			error_handling("can not open file");

		z = y - x;
		fprintf(fp,  "%d", z);
		write(clnt_sock, msg8, sizeof(msg8));
	}
	else{
		write(clnt_sock, msg9, sizeof(msg9));
		fclose(fp);
		exit(1);
	}

	fclose(fp);

	struct tm* ptr;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	strcat(date, asctime(ptr));
	strcat(date, "WITHDRAW ");
	strcat(date, amount);
	strcat(date, "\n\n");

	strcat(cust, "_transaction.txt");
	fp = fopen(cust, "a");
	if(fp == NULL)
		error_handling("can not open file");
	fprintf(fp,  "%s", date);
	fclose(fp);
}
void customer_service(){
	int j = 0;
	write(clnt_sock, msg1,sizeof(msg1));
	read(clnt_sock, user_id, sizeof(user_id));
	printf("Client: %s\n", user_id);
	strcat(user_id, "\n");
	fp = fopen("customer_login.txt", "r");
	if(fp == NULL)
		error_handling("can not open file");

	while(fgets(buf, BUF_SIZE, fp) != NULL){
		if(!strcmp(buf, user_id)){
			write(clnt_sock, msg2, sizeof(msg2));
			j++;
		}
	}
	fclose(fp);
	if(j == 0){
		write(clnt_sock, msg3, sizeof(msg3));
		exit(1);
	}

	read(clnt_sock, choice1, sizeof(choice1));
	printf("Client: %s\n", choice1);
	if(!strcmp(choice1, "1")){
		balance_enquiry();
	}
	else{
		transaction_history();
	}
}

void balance_enquiry(){
	char new_id[100];
	char* token = strtok(user_id, "_");
	strcpy(new_id, token);
	strcat(new_id, ".txt");

	fp = fopen(new_id, "r");
	if(fp == NULL)
		error_handling("can not open file");
	while(fgets(buf, BUF_SIZE, fp) != NULL){
		write(clnt_sock, buf, sizeof(buf));
	}
	fclose(fp);
}
void transaction_history(){
	char new_id[100];
	char* token = strtok(user_id, "_");
	strcpy(new_id, token);
	strcat(new_id, "_transaction.txt");

	fp = fopen(new_id, "r");
	if(fp == NULL)
		error_handling("can not open file");

	while(!feof(fp)){
		fgets(buf, BUF_SIZE, fp);
		sendto(clnt_sock, buf, strlen(buf), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
	}
	fclose(fp);
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
