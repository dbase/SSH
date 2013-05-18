// Compile as: gcc server.c -lcrypt -w
// Assuming that we know the path where the ssh is hosted
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <crypt.h>	// to encrypt a file
#define max 429496
//char ch[100]={"/home/vikas/Desktop/b/s/"};
//char ch=getcwd(0, 0);
int main()
{	printf("Worked");	
	char ch=getcwd(0, 0);
	int sd,b,cd,bytes_r,connected,sin_size,f_open,m,flag=1,flag2=1,pid,i,k,st;
	char fname[50], op[max],data1[max],user_name[1024],pass_name[1024],*op1,*op2,path[1024],ch2[100],ch3[100];
	struct sockaddr_in caddr,saddr;
	FILE *fp;
	socklen_t clen=sizeof(caddr);
	strcpy(data1,"#connected \0");
	
	sd=socket(AF_INET,SOCK_STREAM,0);
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // TCP connection
		perror("Socket");
		exit(1);
	}
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(10165);
	saddr.sin_addr.s_addr=INADDR_ANY;
	bzero(&(saddr.sin_zero),8); 
	b=bind(sd,(struct sockaddr*)&saddr,sizeof(saddr));

	if(b>=0)
		printf("binded successfully \n");
	else
	{	perror("Unable to bind");
        	exit(1);
	}
	listen(sd, 5);
	printf("\nSSH Server Waiting for client on port 10k*\n"); 

	sin_size = sizeof(struct sockaddr_in);
	connected = accept(sd, (struct sockaddr *)&caddr,&sin_size);
	printf("\n I got a connection from (%s , %d)\n",inet_ntoa(caddr.sin_addr),ntohs(caddr.sin_port));
	send(connected, data1,strlen(data1), 0);        // 1. sending connected
	while(flag)
        {	flag2=1;
		while(flag2)
		{	bytes_r = recv(connected,user_name,1024,0);//2. recvng username
			user_name[bytes_r] = '\0';
			printf("User-->>%s\n",user_name);
			bytes_r = recv(connected,pass_name,1024,0);//3. recvng password
			pass_name[bytes_r] = '\0';
			printf("pass-->>%s\n",pass_name);
		
			char chk[100],fi[100],fpass[100];
			//strcpy(chk,user_name);
			//op1=crypt(pass_name,user_name);
			//strcat(chk,op1);
			
			//Checking for password in the passwd file
			
			int y=0;
			fp=fopen("pass.txt","r");
			while(!feof(fp))
			{	fscanf(fp,"%s",fname);
				fscanf(fp,"%s",fpass);
				//strcpy(fi,fname);
				strcpy(fi,fpass);
				if((strcmp(fi,pass_name)==0)&&(strcmp(fname,user_name)==0))
				{	y++;
				}
			}
			fclose(fp);
			if(y>0)				//password matches
			{	strcpy(data1,"Match \n");
				strcat(data1,"Welcome ");
				strcat(data1,user_name);
				strcat(data1,"\n");				
				send(connected, data1,strlen(data1), 0); //4. sending match
				if(strcmp(user_name,"admin")==0)
				{	printf("Welcome admin!\n");	
					while(1)
					{	bytes_r = recv(connected,data1,1024,0);//recvng cmd from user
						data1[bytes_r] = '\0';
						printf("Admin Command recieved-->%s\n",data1);	
						if(strcmp(data1,"quit")==0)
							break;
						else if(strcmp(data1,"useradd")==0)
						{//	printf("hahaha\n");
							bytes_r = recv(connected,data1,1024,0);//recvng new user from user
							data1[bytes_r]='\0';
							
							k=0;		
							fp=fopen("pass.txt","r");
							while(!feof(fp))
							{	fscanf(fp,"%s",fname);
								fscanf(fp,"%s",fpass);
								//strcpy(fi,fname);
								//strcat(fi,fpass);
								if(strcmp(fname,data1)==0)
								{	k=1;
								}
							}
							fclose(fp);		
							if(k==1)
							{	printf("User already exists!\n");
								strcpy(data1,"User already exists!\n");				
								send(connected, data1,strlen(data1), 0);				
							}
							else
							{	send(connected, data1,strlen(data1), 0);			
								// got the username; making directory for him
								chdir("/home/vikas/Desktop/b/s/");
								strcpy(user_name,data1);
								mkdir(data1,0);
								strcpy(data1,"chmod 770 ");
								strcat(data1,user_name);
								system(data1);
								//got the password
								bytes_r = recv(connected,data1,1024,0);//recvng cmd from user
								data1[bytes_r]='\0';
								strcat(user_name,"\t");
								strcat(user_name,data1);
								strcat(user_name,"\n");
								//storing the password in the file		
								fp=fopen("pass.txt","a");	
								fprintf(fp,"%s",user_name);
								fclose(fp);
							}
						}
						else if(strcmp(data1,"put")==0)
						{	//memset(data1,'\0',strlen(data1));
							bytes_r=recv(connected,fname,sizeof(fname),0);
							//data1[bytes_r]='\0';							
							printf("File-->>%s\n",fname);									
							if ((fp=fopen(fname,"w"))==NULL)
							{	printf("error in opening in a file");
								exit(0);
							}
							else 
								printf("file opened successfully \n");
							bytes_r=recv(connected,data1,sizeof(data1),0);
							//data1[bytes_r]='\0';	
							fwrite(data1,strlen(data1),1,fp);
							fclose(fp);
							printf("The file has been transferred;\n");
						}
						else if(strcmp(data1,"get")==0)
						{	recv(connected,fname,sizeof(fname),0);
							fp=fopen(fname,"rb");
							fseek(fp, 0L, SEEK_END);
							m = ftell(fp);
							op1=malloc(m*(sizeof(char)));
							//memset(op2,'\0',strlen(op2));
							fseek(fp, 0, SEEK_SET);
							fread(op1,m,1,fp);
							send(connected,op1,m,0);
							fclose(fp);
							printf("The file has been transferred!\n");
						}
						else if(strcmp(data1,"mkdir")==0)
						{	recv(connected,fname,sizeof(fname),0);
							mkdir(fname,0);
							strcpy(data1,"chmod 777 ");
							strcat(data1,fname);
							system(data1);				
							printf("Directory '%s' created \n",fname);
							
						}
						else if(strcmp(data1,"chdir")==0)
						{	recv(connected,fname,sizeof(fname),0);
					
							if(fname[0]!='/')
							{//	printf("heeasd\n");			
					
								chdir(fname);
								printf("New directory: ");			
								system("pwd");
							}
							else if(fname[0]=='/')
							{	chdir("/");
								chdir(fname);
								printf("New directory: ");			
								system("pwd");
							}
							
						}
						else if(strcmp(data1,"remove")==0) 
						{	recv(connected,fname,sizeof(fname),0);
							int x=remove(fname);
							memset(data1,'\0',strlen(data1));									
							if(x==0)
							{	printf("The directory has been removed \n");
								strcpy(data1,"The directory has been removed \n");												
								
							}	
							else
							{	printf("The directory can't be removed \n");
								strcpy(data1,"The directory can't be removed \n");												
								
							}
							send(connected, data1,strlen(data1), 0);							
						}
						else if(strcmp(data1,"ls")==0)
						{	pid = fork();
							if (pid < 0) 
							{	fprintf(stderr, "Fork Failed");
								exit(-1);
							}
							else if (pid == 0) 
							{ 	
								FILE *fp;
								//int fp1;
								close(1);	
								fp = fopen("list.txt", "w");
								dup(fp);
								execlp("/bin/ls","ls", NULL);
								close(fp);
							}
							else
							{	wait(NULL);
								FILE *fp;
								i=0;
								fp = fopen("list.txt", "r");
								while((op[i]=fgetc(fp))!=EOF)
								{
									i++;
								}
								op[i]='\0';
								fclose(fp);
								remove("list.txt");
								//printf("%s\n",send_data);
								send(connected,op, strlen(op), 0);
								memset(op,'\0',strlen(op));
							}			
						}
						else
						{	printf("Invalid Command!\n");
						}				
					}				
				}
				else		//non-ADMIN
				{	printf("Welcome %s\n",user_name);
					chdir(user_name);
					system("pwd");
					while(1)
					{	bytes_r = recv(connected,data1,1024,0);//recvng cmd from user
						data1[bytes_r] = '\0';
						printf("Command recieved-->%s\n",data1);
						if(strcmp(data1,"quit")==0)
							break;
						else if(strcmp(data1,"put")==0)
						{	//memset(data1,'\0',strlen(data1));
							bytes_r=recv(connected,fname,sizeof(fname),0);
							//data1[bytes_r]='\0';							
							printf("File-->>%s\n",fname);									
							if ((fp=fopen(fname,"w"))==NULL)
							{	printf("error in opening in a file");
								exit(0);
							}
							else 
								printf("file opened successfully \n");
							bytes_r=recv(connected,data1,sizeof(data1),0);
							//data1[bytes_r]='\0';	
							fwrite(data1,strlen(data1),1,fp);
							fclose(fp);
							printf("The file has been transferred;\n");
						}
						else if(strcmp(data1,"get")==0)
						{	recv(connected,fname,sizeof(fname),0);
							fp=fopen(fname,"rb");
							fseek(fp, 0L, SEEK_END);
							m = ftell(fp);
							op1=malloc(m*(sizeof(char)));
							//memset(op2,'\0',strlen(op2));
							fseek(fp, 0, SEEK_SET);
							fread(op1,m,1,fp);
							send(connected,op1,m,0);
							fclose(fp);
							printf("The file has been transferred!\n");
						}
						else if(strcmp(data1,"mkdir")==0)
						{	recv(connected,fname,sizeof(fname),0);
							if(fname[0]=='/')
							{	strcpy(ch3,ch);
								strcat(ch3,user_name);
								st=strlen(ch3);
								if((strncmp(ch3,fname,st))!=0)
								{	printf("Can't create directory over there!\n");
									memset(data1,'\0',strlen(data1));
									strcpy(data1,"Can't create directory over there!\n");												
									send(connected, data1,strlen(data1), 0);											
								}
								else
								{	mkdir(fname,0);
									strcpy(data1,"chmod 777 ");
									strcat(data1,fname);
									system(data1);				
									printf("Directory '%s' created \n",fname);
									
									memset(data1,'\0',strlen(data1));
									strcpy(data1,"Directory created\n");												
									send(connected, data1,strlen(data1), 0);		
								}
							}
							else 	
							{	mkdir(fname,0);
								strcpy(data1,"chmod 777 ");
								strcat(data1,fname);
								system(data1);				
								printf("Directory '%s' created \n",fname);
								memset(data1,'\0',strlen(data1));
								strcpy(data1,"Directory created\n");												
								send(connected, data1,strlen(data1), 0);							
							}
							
						}
						else if(strcmp(data1,"chdir")==0)
						{	recv(connected,fname,sizeof(fname),0);
					
							if(fname[0]=='/')
							{	strcpy(ch3,ch);
								strcat(ch3,user_name);
								st=strlen(ch3);
								if((strncmp(ch3,fname,st))!=0)
								{	printf("Can't change directory!\n");
									memset(data1,'\0',strlen(data1));
									strcpy(data1,"Can't change directory!\n");												
									send(connected, data1,strlen(data1), 0);
								}
								else
								{	chdir("/");
									chdir(fname);
									printf("New directory: ");			
									system("pwd");
									
									memset(data1,'\0',strlen(data1));
									strcpy(data1,"Directory changed!\n");												
									send(connected, data1,strlen(data1), 0);	
								}
							}									
							else
							{	chdir(fname);
								printf("New directory: ");			
								system("pwd");
								
								memset(data1,'\0',strlen(data1));
								strcpy(data1,"Directory changed!\n");
								send(connected, data1,strlen(data1), 0);
							}
							
							
						}

						else if(strcmp(data1,"remove")==0) 
						{	recv(connected,fname,sizeof(fname),0);
							memset(data1,'\0',strlen(data1));
							if(fname[0]=='/')
							{	strcpy(ch3,ch);
								strcat(ch3,user_name);
								st=strlen(ch3);
								if((strncmp(ch3,fname,st))!=0)
								{	printf("Wrong path!\n");
									strcpy(data1,"Wrong path!\n");
									send(connected, data1,strlen(data1), 0);
								}
								else
								{	int x=remove(fname);
									if(x==0)
									{	printf("The directory has been removed \n");
										strcpy(data1,"The directory has been removed \n");												
										send(connected, data1,strlen(data1), 0);
									}	
									else
									{	printf("The directory can't be removed \n");
										strcpy(data1,"The directory can't be removed \n");												
										send(connected, data1,strlen(data1), 0);
									}
								}
							}
							else
							{	int x=remove(fname);
								if(x==0)
								{	printf("The directory has been removed \n");
									strcpy(data1,"The directory has been removed \n");												
									send(connected, data1,strlen(data1), 0);
								}	
								else
								{	printf("The directory can't be removed \n");
									strcpy(data1,"The directory can't be removed \n");												
									send(connected, data1,strlen(data1), 0);
								}
							}											
						}
						else if(strcmp(data1,"ls")==0)
						{	pid = fork();
							if (pid < 0) 
							{	fprintf(stderr, "Fork Failed");
								exit(-1);
							}
							else if (pid == 0) 
							{ 	
								FILE *fp;
								//int fp1;
								close(1);	
								fp = fopen("list.txt", "w");
								dup(fp);
								execlp("/bin/ls","ls", NULL);
								close(fp);
							}
							else
							{	wait(NULL);
								FILE *fp;
								i=0;
								fp = fopen("list.txt", "r");
								while((op[i]=fgetc(fp))!=EOF)
								{
									i++;
								}
								op[i]='\0';
								fclose(fp);
								remove("list.txt");
								//printf("%s\n",send_data);
								send(connected,op, strlen(op), 0);
								memset(op,'\0',strlen(op));
							}			
						}							
						else
							printf("Invalid command!");
					}				
				}
				
			}
			else			// wrong password
			{	strcpy(data1,"Failed to connect!\n");
				send(connected, data1,strlen(data1), 0); //4. sending fail
			}
			flag2=y=0;	
			
		}
		printf("Do you want to close server: Yes==0, No==1: ");
		scanf("%d",&flag);		
	}
	return 0;
}
