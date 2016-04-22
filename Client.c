#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFSIZE 1024
#define S_MYFIFO "server_myfifo"
#define FIFO_PERMS (S_IRWXU | S_IWGRP| S_IWOTH)

//Yardimci Fonksiyonlarim
void Ctrlc_Sinyali(int signo); 


int main(int argc,char *argv[])
{

	int value,fdClient,fd1,fd2,fd3,strsize,count=0;
	char buffer[BUFSIZE];
	char ClientsPidBuffer[BUFSIZE];
	char path[BUFSIZE],resIntegral[BUFSIZE],logFile[BUFSIZE];
	FILE* optr;

	//Bu kod blogu ders kitabi sayfa 326-Example 8.16 dan alinmistir.
	struct sigaction act;
	act.sa_handler = Ctrlc_Sinyali;
	act.sa_flags = 0;
	if ((sigemptyset(&act.sa_mask) == -1) ||
		(sigaction(SIGINT, &act, NULL) == -1))
		perror("Failed to set SIGINT to handle Ctrl-C");



	//path isimli stringe getpid'mi yaziyorum.2.Fifoyu acarken lazim olacak.
	snprintf(path,BUFSIZE,"%d",(int)getpid());
	//5 Parametre girilmezse hata veriyorum.
	if(argc!=5){
    	fprintf(stderr,"HATA\nKullanimi:./dosya ismi -<fi> -<fj> -<time interval> -<operation>\n");
      	return 1;}

    /*----------------------------------SERVER FIFO-------------------------------------*/  	
    //Server my fifo'yu yazma modunda aciyorum.icine getpid degerimi yazicam.
    if((fdClient=open(S_MYFIFO,O_WRONLY))==-1){
		perror("AnaServer Fifo acilamadi.");
		return 1;}


	//2.fifoyu olusturyorum
	if((mkfifo(path,FIFO_PERMS)==-1) && (errno != EEXIST)){
    	perror("2.Fifo olusturulamadi.");
       	return 1;}	


    //Server my fifo icine pid'mi yaziyorum.
	snprintf(buffer,BUFSIZE,"%d",(int)getpid());
	printf("Server'a göndereceğim pid degeri %s\n",buffer);
	strcpy(ClientsPidBuffer,buffer);


	strsize =strlen(buffer) + 1;
	value =write(fdClient,buffer,strlen(buffer) + 1);
	if (value != strsize)
		fprintf(stderr, "[%ld]:failed to write to pipe: %s\n",
			(long)getpid(), strerror(errno)); 	

	

	/*----------------------------------2.FIFO-------------------------------------*/
	//buffer=getpid().Fifo adi Kendi pid'si olan fifo ya yaziyor.
	//2.FİFO icine ise parametrelerimi yazdim.
    if((fd2=open(buffer,O_WRONLY))==-1){
		perror("2.Fifo acilamadi.");
		return 1;}

    //fifonun icine Clients.c programinin parametrelerini yaziyorum
    snprintf(buffer,BUFSIZE,"%s %s %s %s",argv[1],argv[2],argv[3],argv[4]);
    printf("Server'a %s %s %s %s degerlerini gönderdim.\n",argv[1],argv[2],argv[3],argv[4]);

    strsize =strlen(buffer) + 1;
	value =write(fd2,buffer,strlen(buffer) + 1);
	if (value != strsize)
		fprintf(stderr, "[%ld]:failed to write to pipe: %s\n",
			(long)getpid(), strerror(errno)); 

	strcat(ClientsPidBuffer,"R");

	snprintf(logFile,BUFSIZE,"%d",(int)getpid());
	strcat(logFile,".log");

	while(1)
	{
		/*----------------------------------3.FIFO-------------------------------------*/
		//3.fifo  ile integralin sonucunu alicam.Bu yuzden okuma kodunda aciyorum.Bu fifonun ismi getpid+R olacak.
		optr=fopen(logFile,"a+");
    	if((fd3=open(ClientsPidBuffer,O_RDONLY))==-1){
    		printf("%s isimli fifoyu acamadim.3.fifoyum\n",ClientsPidBuffer );
			perror("3.Fifo acilamadi.");
			return 1;}

    	value=read(fd3,resIntegral,BUFSIZE);
		if(value==-1)
			fprintf(stderr,"3.Pipe'dan okuma yapilamadi.");


		//Integralin Sonucu
		printf("%s",resIntegral);
		if(count==0)
			fprintf(optr,"\nClient'in Pid Değeri==>%d\n\n",(int)getpid());
		fprintf(optr,"%s",resIntegral);
	
		count++;
		fclose(optr);
	}

}

/*Ctrl-C sinyalini burada yakalayip clientspid arayine aldigim tum clientslari kill ediyorum.*/
void Ctrlc_Sinyali(int signo) 
{
    printf("\nCTRL-C sinyali yakalandi.Client kendini kill etti.\n");
    exit(1);
}