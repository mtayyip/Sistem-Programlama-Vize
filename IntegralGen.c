#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFSIZE 1024
#define S_MYFIFO "server_myfifo"//Ana Fifo
#define FIFO_PERMS (S_IRWXU | S_IWGRP| S_IWOTH)


//Global Degiskenler
int*clientsPid;//client pid degerlerini tutuacagim arrayim.
int MAXCLIENT;//max.client sayisi.argv[2] ile alicam

//Yardimci Fonksiyonlarim
void Ctrlc_Sinyali(int signo); 
double integralHesaplama(double(*f)(double x), double a, double b, int n);
double f(double x);


int main (int argc,char *argv[]) 
{
	
	int i=0,fdServer,fd1,fd2,fd3,val,j=0,strsize,deger,count=0;
	double t0,resolution,timeInterval,integralSonucu;
	time_t basla,bit;
	pid_t childpid;

	FILE* iptr1=fopen("f1.txt","r");FILE* iptr2=fopen("f2.txt","r");
	FILE* iptr3=fopen("f3.txt","r");FILE* iptr4=fopen("f4.txt","r");
	FILE* iptr5=fopen("f5.txt","r");FILE* iptr6=fopen("f6.txt","r");


	char f1[BUFSIZE],f2[BUFSIZE],f3[BUFSIZE],f4[BUFSIZE],f5[BUFSIZE],f6[BUFSIZE];
	char buffer[BUFSIZE],ClientsPidBuffer[BUFSIZE],copy_buffer[BUFSIZE],Fonksiyon[BUFSIZE];
	char*par1,*par2,*par3,*par4,*pch;


	resolution=atoi(argv[1]);//resolution degerini 1.parametre ile aliyorum.
	MAXCLIENT=atoi(argv[2]);//max clients degerini 2.parametre ile aliyorum.

	clientsPid=(int*)malloc(sizeof(int)*MAXCLIENT);//global olan degerime maxclient kadar yer aciyorum.


	//Bu kod blogu ders kitabi sayfa 326-Example 8.16 dan alinmistir.
	struct sigaction act;
	act.sa_handler = Ctrlc_Sinyali;
	act.sa_flags = 0;
	if ((sigemptyset(&act.sa_mask) == -1) ||
		(sigaction(SIGINT, &act, NULL) == -1))
		perror("Failed to set SIGINT to handle Ctrl-C");



	//Dosyalardan Verileri Okuma
	/*-------------------------------------f1.txt yi okuyorum.-----------------------------------------*/
	if (iptr1==NULL){
        printf("f1.txt Dosyasi acilamadi\n");
        return 0;}
    else{   
		while(!feof(iptr1)){
			fscanf(iptr1,"%c",&f1[i]);i++; } f1[i-2]='\0';i=0;}
	/*-------------------------------------f2.txt yi okuyorum.-----------------------------------------*/
	if (iptr2==NULL){
        printf("f2.txt Dosyasi acilamadi\n");
        return 0;}
	else{
		while(!feof(iptr2)){
			fscanf(iptr2,"%c",&f2[i]); i++;} f2[i-2]='\0';i=0;}
	/*-------------------------------------f3.txt yi okuyorum.-----------------------------------------*/
	if (iptr3==NULL){
        printf("f3.txt Dosyasi acilamadi\n");
        return 0;}
    else{    
		while(!feof(iptr3)){
			fscanf(iptr3,"%c",&f3[i]); i++;} f3[i-2]='\0';i=0;}
	/*-------------------------------------f4.txt yi okuyorum.-----------------------------------------*/
	if (iptr4==NULL){
        printf("f4.txt Dosyasi acilamadi\n");
        return 0;}
	else{
		while(!feof(iptr4)){
			fscanf(iptr4,"%c",&f4[i]); i++;} f4[i-2]='\0';i=0;}
	/*-------------------------------------f5.txt yi okuyorum.-----------------------------------------*/		
	if (iptr5==NULL){
        printf("f5.txt Dosyasi acilamadi\n");
        return 0;}
	else{
		while(!feof(iptr5)){
			fscanf(iptr5,"%c",&f5[i]); i++;} f5[i-2]='\0';i=0;}
	/*-------------------------------------f6.txt yi okuyorum.-----------------------------------------*/
	if (iptr6==NULL){
        printf("f6.txt Dosyasi acilamadi\n");
        return 0;}
	else{
		while(!feof(iptr6)){
		fscanf(iptr6,"%c",&f6[i]); i++;} f6[i-2]='\0';i=0;}


	printf("DOSYADAN OKUNAN DEGERLER:\n");	
	printf("f1.txt %s\n",f1);
	printf("f2.txt %s\n",f2);
	printf("f3.txt %s\n",f3);
	printf("f4.txt %s\n",f4);
	printf("f5.txt %s\n",f5);
	printf("f6.txt %s\n\n",f6);

	
	//3 Parametre girilmezse hata veriyorum.
	if(argc!=3){
    	fprintf(stderr,"HATA\nKullanimi:./dosya ismi -<resolution> -<max # of clients>\n");
      	return 1;}
    
    time (&basla);
    
    //Serverin ana fifosu nu olusturuyorum.
    if((mkfifo(S_MYFIFO,FIFO_PERMS)==-1) && (errno != EEXIST)){
    	perror("AnaServer Fifo olusturulamadi.");
       	return 1;}

    //Okuma ve Yazma modunda aciyorum.
    if((fdServer=open(S_MYFIFO,O_RDWR))==-1){
		perror("AnaServer Fifo acilamadi.");
		return 1;}



	while(1)
	{	
		
		/*----------------------------------SERVER FIFO-------------------------------------*/	
		//SERVERFİFO--->clients'den gelen pid'yi buffer a okuyor.
		val=read(fdServer,buffer,BUFSIZE);
		if(val==-1)
			fprintf(stderr,"failed to read from pipe.");

		time (&bit);
  		t0 = difftime(bit, basla);
		printf("Client'dan --->%s degerini aldim.Client'in baglanma zamani %.3f saniye.\n",buffer,t0);
		

		strcpy(ClientsPidBuffer,buffer);//ClientsPidBuffer icine getpid degerini aliyorum.3.Fifoyu olusturmada lazim olacak.
		deger=atoi(buffer);//gelen client pid degerini clientsPid arayime aliyorum.Daha sonra kullanicam.
		clientsPid[j]=deger;
		j++;


		//Burada 3.Fifo yu da olusturuyorum.3.Fifonun adi getpid+R olacak.
		strcat(ClientsPidBuffer,"R");
		if((mkfifo(ClientsPidBuffer,FIFO_PERMS)==-1) && (errno != EEXIST)){
			printf("%s isimli fifoyu acamadim.3.fifoyum\n",ClientsPidBuffer );
    		perror("3.Fifo olusturulamadi.");
       		return 1;}





		/*----------------------------------2.FIFO-------------------------------------*/
		//buffer isimli fifo yu okuma modunda aciyorum.(buffer=clients pid).
       	//2.FİFO Clients'in Parametrelerini aliyor.
    	if((fd2=open(buffer,O_RDONLY))==-1){
			perror("1.Fifo acilamadi.");
			return 1;}

		//2.fifodan gelen clients'in parametrelerini buffera yaziyorum.
    	val=read(fd2,buffer,BUFSIZE);
		if(val==-1)
			fprintf(stderr,"2.Pipe'dan okuma yapilamadi.");


		/*----------------------------------3.FIFO-------------------------------------*/
		//3.fifo getpid+R isimli mkfifo yazma modunda aciyorum.
    	if((fd3=open(ClientsPidBuffer,O_WRONLY))==-1){
			perror("3.Fifo acilamadi.");
			return 1;}





		/*2.Fifo ile gelen degerleri baska bir stringe kopyaladim cunku degeri degisecek.
		  Daha sonra Clients'den 4 parametre geldigi kesin oldugu icin 4 e bolup ayrı ayrı stringlere
		  atiyorum.*/	
		strcpy(copy_buffer,buffer);
		par1=strtok(copy_buffer," ");
		printf("1.PARAMETRE %s---",par1);
		par2=strtok(NULL," ");
		printf("2.PARAMETRE %s---",par2);
		par3=strtok(NULL," ");
		printf("3.PARAMETRE %s---",par3);
		par4=strtok(NULL," ");
		printf("4.PARAMETRE %s\n",par4);


		//Fonksiyon stringimi bosaltiyorum.
		strcpy(Fonksiyon,"");


		/*Daha sonra parametre1,parametre2 ve parametre4'e bakarak hangi dosyanin geldigini anliyorum.
		  Ve Fonksiyon stringime yaziyorum*/	 
		if(strcmp(par1,"f1")==0)
			strcat(Fonksiyon,f1);
		else if(strcmp(par1,"f2")==0)
			strcat(Fonksiyon,f2);
		else if(strcmp(par1,"f3")==0)
			strcat(Fonksiyon,f3);
		else if(strcmp(par1,"f4")==0)
			strcat(Fonksiyon,f4);
		else if(strcmp(par1,"f5")==0)
			strcat(Fonksiyon,f5);
		else if(strcmp(par1,"f6")==0)
			strcat(Fonksiyon,f6);
		else
			printf("Girilen isimde dosya yok.\n");							
		/*-----------------------------------------------------------------------*/
		if(strcmp(par4,"+")==0)
			strcat(Fonksiyon,par4);
		else if(strcmp(par4,"-")==0)
			strcat(Fonksiyon,par4);
		else if(strcmp(par4,"/")==0)
			strcat(Fonksiyon,par4);
		else if(strcmp(par4,"*")==0)
			strcat(Fonksiyon,par4);
		else
			printf("Girilen operator bulunamadi.\n");
		/*-----------------------------------------------------------------------*/
		if(strcmp(par2,"f1")==0)
			strcat(Fonksiyon,f1);
		else if(strcmp(par2,"f2")==0)
			strcat(Fonksiyon,f2);
		else if(strcmp(par2,"f3")==0)
			strcat(Fonksiyon,f3);
		else if(strcmp(par2,"f4")==0)
			strcat(Fonksiyon,f4);
		else if(strcmp(par2,"f5")==0)
			strcat(Fonksiyon,f5);
		else if(strcmp(par2,"f6")==0)
			strcat(Fonksiyon,f6);
		else
			printf("Girilen isimde dosya yok.\n");
		/*-----------------------------------------------------------------------*/
		//time interval degeri pararmetre3 ile alindi.
		sscanf(par3,"%lf",&timeInterval);
		/*-----------------------------------------------------------------------*/
		printf("Client'in Baglanma Zamani==>%.3lf---Resolution Degeri==>%.3lf---Time Interval degeri==>%.3lf\n\n\n",t0,resolution,timeInterval);


		/*---------------------------------FORK----------------------------------*/
		childpid=fork();

		if(childpid==-1){
			printf("HATA!Fork olusturulamadi.\n");
			exit(1);}
		else if(childpid==0)
		{

			for( ; ; ){									
				integralSonucu=integralHesaplama(f,t0,t0+timeInterval,timeInterval/resolution);
				t0=t0+timeInterval;	

				//Bu kısımda hesaplanan integral sonucunu ClientsPidBuffer a yaziliyor.	
				snprintf(ClientsPidBuffer,BUFSIZE,"Fonksiyon==>%s---Client Bağlanma zamanı==>%.3lf---İntegralin sonucu==>%lf\n",Fonksiyon,(t0-timeInterval),integralSonucu);
    			strsize =strlen(ClientsPidBuffer) + 1;
				val =write(fd3,ClientsPidBuffer,strlen(ClientsPidBuffer) + 1);

				if (val != strsize)
				fprintf(stderr, "[%ld]:failed to write to pipe: %s\n",
					(long)getpid(), strerror(errno)); 

				sleep(timeInterval);}				
		}
		else;
							
	
    	}//while(1) sonu	

    	
	return 0;

}



/*Ctrl-C sinyalini burada yakalayip clientspid arayine aldigim tum clientslari kill ediyorum.*/
void Ctrlc_Sinyali(int signo) 
{
	for(int i=0;i<MAXCLIENT;i++)
		kill(clientsPid[i],9);	
}


double f(double x)
{		
	return x;
}


/*Bu Fonksiyon asagidaki siteden kaynak olarak alindi.
https://gist.github.com/calebreister/634fe925c84fadd01c6f	
*/	
double integralHesaplama(double(*f)(double x), double a, double b, int n) 
{
	double step = (b - a) / n;  // width of each small rectangle
	double area = 0.0;  // signed area
	for (int i = 0; i < n; i ++) 
    	area += f(a + (i + 0.5) * step) * step; // sum up each small rectangle
    
    return area;
}