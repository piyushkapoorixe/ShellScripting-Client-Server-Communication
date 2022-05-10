#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <ctype.h>
#include <sys/wait.h>
int myval;
int main(int argc, char *argv[]){
myval = argc;
int sd, csd, portNumber, status;
socklen_t len;
struct sockaddr_in servAdd;//ipv4

//if(argc != 2){
//printf("Call model: %s <Port #>\n", argv[0]);
//exit(0);
//}
if((sd = socket(AF_INET, SOCK_STREAM, 0))<0){
printf("Cannot create socket\n");
exit(1);
}
//Add information to the servAdd struct variable of type sockaddr_in
servAdd.sin_family = AF_INET;
//When INADDR_ANY is specified in the bind call, the socket will be bound to all local interfaces.
//The htonl function takes a 32-bit number in host byte order and returns a 32-bit number in the network byte order used in TCP/IP networks
servAdd.sin_addr.s_addr = htonl(INADDR_ANY); //Host to network long 

sscanf(argv[1], "%d", &portNumber);
//uint16_t: optional integer type of exactly 16 bits
//htonos: Host to network short-byte order
servAdd.sin_port = htons((uint16_t)portNumber); 

//struct sockaddr is the generic structure to store socket addresses
//The procedure it to typecast the specific socket addreses of type sockaddr_in, sockaddr_in6, sockaddr_un into sockaddr

bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd));
listen(sd, 5);

while(1){
csd = accept(sd,(struct sockaddr *)NULL,NULL);
printf("Got a client\n");
if(!fork())//Child process 
handle_client(csd);
close(csd);
waitpid(0, &status, WNOHANG); // waitpid? 
}
}//End main 


void handle_client(int csd1){
char message[255];
char pattern[255];
//char pattern[255];
int n, pid, pid2, sid;

FILE *fp, *f;
char c;
int ch = 0;
int words, words2;
char buffer[255];
char buffer2[255];

write(csd1, "start chatting", 16);

/* here */
if(pid=fork()) /* reading csd messages */
{
	
	while(1)
	{
		if(n=read(csd1, pattern, 255)) {
			//fprintf(stderr,"first = %s\n", pattern);
		}
		if(n=read(csd1, message, 255))
		{
			//printf("myval = %d", myval);
			//if(myval == 3) {
				//printf("in here where argc = 3");
				
			//}
			//else {
				message[n]='\0';
				//fprintf(stderr,"second = %s\n", message);
				if(!strcasecmp(message, "Bye\n")){
					kill(pid, SIGTERM);
					exit(0);
				}
				else if (!strcasecmp(message, "File\n")) {
					
					fp = fopen("client_file_received.txt", "a");
					fp = fopen("grep_output.txt", "w");
				
					read(csd1, &words, sizeof(int));
					
					while(ch != words) {
						read(csd1, buffer, 255);
						fprintf(fp, "%s ", buffer);
						ch++;
					}
					
					rewind(fp);
					
					pid2 = fork();
					if(pid2==0) { //child
						execlp("grep","grep","-w", "--color=auto",pattern,"client_file_received.txt","grep_output.txt", NULL);
					}
					else if (pid2){ //parent
						int child_status;	

						waitpid(pid2, &child_status, 0);
						//fprintf(stderr, "inside parent");

					f = fopen("grep_output.txt", "r");
					if (f == NULL) {
						fprintf(stderr, "File 2 doesnt exist\n");
						//exit(0);
						return;
					}
					while((c = getc(f)) != EOF) {
						fscanf(f, "%s", buffer2);
						if(isspace(c) || c=='\t')
							words2++;
						
					}
					write(csd1, &words2, sizeof(int));
					rewind(f);
					
					char ch2;
					while(ch2 != EOF) {
						fscanf(f, "%[^\n]", buffer2);
						write(csd1, buffer2, 255);
						ch2 = fgetc(f);
					}
					}
					
					//execlp("ls","ls","-1", NULL);
					//execlp("cat","cat","client_file_received.txt", NULL);
					//exit(0);
				}
			//}
		}
		
		
		
	}
}
else
{
while(1)// sending messages csd
{
if(n=read(0, message, 255)){
message[n]='\0';
write(csd1, pattern, strlen(pattern)+1);
write(csd1, message, strlen(message)+1);
if(!strcasecmp(message, "Bye\n")){
kill(getppid(), SIGTERM);
exit(0);
}


else if (!strcasecmp(message, "File\n")) {
					
					fp = fopen("client_file_received.txt", "a");
					fp = fopen("grep_output.txt", "w");
				
					read(csd1, &words, sizeof(int));
					
					while(ch != words) {
						read(csd1, buffer, 255);
						fprintf(fp, "%s ", buffer);
						ch++;
					}
					//printf("client_file_received.txt has been saved successfully.");
					rewind(fp);
					
					//execlp("ls","ls","-1", NULL);
					//execlp("cat","cat","client_file_received.txt", NULL);
					//exit(0);
				}

}
}//End while 
}
}//End child

