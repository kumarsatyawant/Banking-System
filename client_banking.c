#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netdb.h>
#define BUF_SIZE 1024

int sock;
char user_id[BUF_SIZE];
char cust_id[BUF_SIZE];
char amount[BUF_SIZE];
char buf[BUF_SIZE];
char choice[5], choice1[5];
char msg1[100], msg2[100], msg3[100], msg4[200];

void error_handling(char *message);
void customer_service();
void admin_service();
void credit_money();
void withdraw_money();
void balance_enquiry();
void transaction_history();

int main(int argc, char *argv[]){
	struct sockaddr_in serv_addr;

	if(argc != 3){
		printf("usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	else{
		printf("Welcome To Our Banking Services\n\n");
	}

	printf("1. Customer\n2. Admin\n");
	printf("Enter your choice\n");

	scanf("%s", choice);
	write(sock, choice, sizeof(choice));
	
	if(!strcmp(choice, "1")){
		customer_service();
	}
	else{
		admin_service();
	}

	close(sock);
	return 0;
}

void admin_service(){
	read(sock, msg1, sizeof(msg1));
	printf("Server : %s\n", msg1);
	printf("Admin: ");
	scanf("%s", user_id);
	write(sock, user_id, sizeof(user_id));
	read(sock, msg2, sizeof(msg2));
	
	
	if(!strcmp(msg2, "NO")){
		printf("Server: Wrong User ID And Password Combination\n");
		exit(1);
	}

	printf("1. Deposit\n2. Withdraw\n");
	printf("Enter your choice\n");
	scanf("%s", choice1);
	write(sock, choice1, sizeof(choice1));
	read(sock, msg3, sizeof(msg3));
	printf("Server: %s\n", msg3);
	printf("Customer_ID: ");
	scanf("%s", cust_id);
	write(sock, cust_id, sizeof(cust_id));

	read(sock, msg2, sizeof(msg2));
	
	
	if(!strcmp(msg2, "NO")){
		printf("Server: Wrong Customer User ID\n");
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
	read(sock, msg4, sizeof(msg4));
	printf("Server: %s\n", msg4);
	printf("Amount: ");
	scanf("%s", amount);
	write(sock, amount, sizeof(amount));
	read(sock, msg4, sizeof(msg4));
	printf("Server: %s %s\n", msg4, cust_id);
}

void withdraw_money(){
	read(sock, msg4, sizeof(msg4));
	printf("Server: %s\n", msg4);
	printf("Amount: ");
	scanf("%s", amount);
	write(sock, amount, sizeof(amount));
	read(sock, msg4, sizeof(msg4));
	printf("Server: %s %s\n", msg4, cust_id);
}

void customer_service(){
	read(sock, msg1, sizeof(msg1));
	printf("Server : %s\n", msg1);
	printf("Customer: ");
	scanf("%s", user_id);
	write(sock, user_id, sizeof(user_id));
	read(sock, msg2, sizeof(msg2));
	

	if(!strcmp(msg2, "NO")){
		printf("Server: Wrong User ID And Password Combination\n");
		exit(1);
	}
	printf("1. Balance Enquiry\n2. Transaction History\n");
	printf("Enter your choice\n");
	scanf("%s", choice1);
	write(sock, choice1, sizeof(choice1));

	if(!strcmp(choice1, "1")){
		balance_enquiry();
	}
	else{
		transaction_history();
	}
	
}

void balance_enquiry(){
	read(sock, msg2, sizeof(msg2));
	printf("Available Balance: %s\n", msg2);
}
void transaction_history(){
	int str_len;
	printf("\n");
	while(1){
		str_len = recvfrom(sock, buf, BUF_SIZE-1, 0, NULL, 0);
		if(str_len < 0){
			break;
		}
		buf[str_len] = 0;
		fputs(buf, stdout);
	}
	exit(1);
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
