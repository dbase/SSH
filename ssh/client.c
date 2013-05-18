//Compile as: gcc <filename> -lcrypt -w
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <crypt.h>
#define max 4294967

int main()
{
	int sd,c,s,connected,bytes_r,m;
	int count=0,flag=1;
	char fname[50],sip[25],recv_data[1024],user_data[1024],pass_data[1024],op[max],send_data[1024],*op1;
	struct sockaddr_in caddr;
	struct hostent *he;
	FILE *fp;
	printf("enter the server ip address \n");
	scanf("%s",sip);
	he=gethostbyname(sip);
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1)  // TCP connection
	{	perror("Socket");
		exit(1);
        }
	caddr.sin_family = AF_INET;     
        caddr.sin_port = htons(10165);    // Port to connect to server
        caddr.sin_addr = *((struct in_addr *)he->h_addr);
        bzero(&(caddr.sin_zero),8); 

	connected=connect(sd,(struct sockaddr *)&caddr,sizeof(struct sockaddr));
	
	if(connected>=0)
		printf("connected to server \n");
	else
	{	printf("connection failed \n");
		exit(1);
	}
	
	
	bytes_r=recv(sd,recv_data,1024,0);	//1. receivng connected
        recv_data[bytes_r] = '\0';
	printf("#ssh %s",recv_data);//printing intial message that connection is established
	while(flag)
	{	printf("\n#ssh: username: ");
		scanf("%s",user_data);
		send(sd,user_data,strlen(user_data), 0);//2. sending username
		printf("\n#ssh: passwd: ");
		scanf("%s",pass_data);
		
		op1=crypt(pass_data,user_data);
		
		strcpy(pass_data,op1);
		send(sd,pass_data,strlen(pass_data), 0);//3. sending "encrypted" password
	
		bytes_r=recv(sd,recv_data,1024,0);	//4. receivng match / fail
		recv_data[bytes_r] = '\0';
		if(recv_data[0]=='M')			//password match
		{	printf("%s",recv_data);	
			while(1)	
			{	fflush(stdout);
				printf("\n#ssh@%s: ",user_data);	//waiting for the command from the user
				if(strcmp(user_data,"admin")==0)
				{	scanf("%s",send_data);
					send(sd,send_data,strlen(send_data), 0);//sending cmd
					if(strcmp(send_data,"quit")==0)
						break;
					else if(strcmp(send_data,"useradd")==0)
					{	printf("\nEnter the new user_name:\n#ssh@%s: ",user_data);	
						scanf("%s",send_data);
						send(sd,send_data,strlen(send_data), 0);//sending username
					
						bytes_r=recv(sd,recv_data,1024,0);	
						recv_data[bytes_r] = '\0';
					
						if(strcmp(recv_data,"User already exists!\n")!=0)
						{	printf("\nEnter the new passwd:\n#ssh@%s: ",user_data);	
							scanf("%s",pass_data);
							op1=crypt(pass_data,send_data);
							strcpy(send_data,op1);						
							send(sd,send_data,strlen(send_data), 0);//sending password
						}
						else
						{	printf("%s\n",recv_data);
						}					
					}
					else if(strcmp(send_data,"put")==0)
					{	printf("\nEnter the new file:\n#ssh@%s: ",user_data);
						//memset(send_data,'\0',strlen(send_data));
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
					
						fp=fopen(fname,"rb");
						fseek(fp,0L,SEEK_END);
						m=ftell(fp);
						op1=malloc(m*sizeof(char));
						fseek(fp,0,SEEK_SET);
						fread(op1,m,1,fp);
						send(sd,op1,m,0);
						fclose(fp);
							
					}
					else if(strcmp(send_data,"get")==0)
					{	printf("\nEnter the new file:\n#ssh@%s: ",user_data);
						//memset(send_data,'\0',strlen(send_data));
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
						//recv(connected,fname,sizeof(fname),0);
						if ((fp=fopen(fname,"w"))==NULL)
						{	printf("error in opening in a file");
							exit(0);
						}
						else 
							printf("file opened successfully \n");
						memset(op,'\0',strlen(op));
						
						recv(sd,op,sizeof(op),0);
						fwrite(op,strlen(op),1,fp);
						fclose(fp);
						printf("The file has been transferred!\n");
					}
					else if(strcmp(send_data,"mkdir")==0)
					{	printf("Enter the name of the directory:\nssh@%s: ",user_data);
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
						printf("Directory has been created!\n");
					}
					else if(strcmp(send_data,"chdir")==0)
					{	printf("Enter the name of the directory:\nssh@%s: ",user_data);
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
						
					}
					else if(strcmp(send_data,"remove")==0 )
					{	printf("Enter the name of the directory:\nssh@%s: ",user_data);
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
						
						memset(recv_data,'\0',strlen(recv_data));
						bytes_r=recv(sd,recv_data,1024,0);	
						recv_data[bytes_r] = '\0';				
											
						printf("%s",recv_data);
					}
					else if(strcmp(send_data,"ls")==0)
					{	printf("The directory contains: \n");
						recv(sd,op,sizeof(op),0);
						printf("%s",op);
						memset(op,'\0',strlen(op));
						fflush(stdout);		
					}	
					else
						printf("Invalid Command\n");			

				}
				else		//non-admin
				{	scanf("%s",send_data);
					send(sd,send_data,strlen(send_data), 0);//sending cmd
					if(strcmp(send_data,"quit")==0)
						break;
					else if(strcmp(send_data,"put")==0)
					{	printf("\nEnter the new file:\n#ssh@%s: ",user_data);
						//memset(send_data,'\0',strlen(send_data));
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
					
						fp=fopen(fname,"rb");
						fseek(fp,0L,SEEK_END);
						m=ftell(fp);
						op1=malloc(m*sizeof(char));
						fseek(fp,0,SEEK_SET);
						fread(op1,m,1,fp);
						send(sd,op1,m,0);
						fclose(fp);
							
					}
					else if(strcmp(send_data,"get")==0)
					{	printf("\nEnter the new file:\n#ssh@%s: ",user_data);
						//memset(send_data,'\0',strlen(send_data));
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
						//recv(connected,fname,sizeof(fname),0);
						if ((fp=fopen(fname,"w"))==NULL)
						{	printf("error in opening in a file");
							exit(0);
						}
						else 
							printf("file opened successfully \n");
						memset(op,'\0',strlen(op));
						
						recv(sd,op,sizeof(op),0);
						fwrite(op,strlen(op),1,fp);
						fclose(fp);
						printf("The file has been transferred!\n");
					}
					else if(strcmp(send_data,"mkdir")==0)
					{	printf("Enter the name of the directory:\nssh@%s: ",user_data);
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
						
						memset(recv_data,'\0',strlen(recv_data));
						bytes_r=recv(sd,recv_data,1024,0);	
						recv_data[bytes_r] = '\0';				
											
						printf("%s",recv_data);
					}
					else if(strcmp(send_data,"chdir")==0)
					{	printf("Enter the name of the directory:\nssh@%s: ",user_data);
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
						
						memset(recv_data,'\0',strlen(recv_data));
						bytes_r=recv(sd,recv_data,1024,0);	
						recv_data[bytes_r] = '\0';				
											
						printf("%s",recv_data);
						
					}
					else if(strcmp(send_data,"remove")==0 )
					{	printf("Enter the name of the directory:\nssh@%s: ",user_data);
						scanf("%s",fname);
						send(sd,fname,sizeof(fname),0);
						
						memset(recv_data,'\0',strlen(recv_data));
						bytes_r=recv(sd,recv_data,1024,0);	
						recv_data[bytes_r] = '\0';				
											
						printf("%s",recv_data);
					}
					else if(strcmp(send_data,"ls")==0)
					{	printf("The directory contains: \n");
						recv(sd,op,sizeof(op),0);
						printf("%s",op);
						memset(op,'\0',strlen(op));
						fflush(stdout);		
					}					
					else
						printf("Invalid Command\n");			

				}
							
			}	
		}
		else					// password Failed to connect
		{	printf("%s",recv_data);
		}
		printf("Do you want to close the terminal. Yes==0 No==1: ");
		scanf("%d",&flag);
	}	
	return 0;
}

		
