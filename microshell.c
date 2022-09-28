///Michal Holubowicz gr.12
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <readline/readline.h> ///aby skompilowac program nalezy dodac
#include <readline/history.h> ///opcje -lreadline aby owe dwie biblioteki mozna bylo uzywac
char buf[4095],tab[5000],bf[4000],ps[4095],bufold[4095];
char *czytaj,*buffer,*token,*user;
int size,u,pid,glob;
char* arg[100],*ar[205];
int separacja_polecen();
void help();
void cal();
int cut();
void contat(char* t[5]);
void sig(int syg) //funkcja wykonywana po przechwyceniu sygnalu
{
	printf("\033[0;94m\n		Wykryto sygnal o numerze \033[0;0m %d,\033[0;94m jesli chcesz go wykonac,\033[0;93m NACISNIJ GO PONOWNIE\033[0;94m, jesli nie, to \033[0;93mKLIKNIJ ENTER\n\033[0;0m",syg);
	printf("%s",tab);
	signal(syg,SIG_DFL); //ponowne przechwycenie i wykonanie
}
int main()
{
	user=getlogin();
	getcwd(ps,200);
	while(1)
	{
		signal(SIGINT,sig); //przechwytywanie sygnalow
		signal(SIGQUIT,sig);
		signal(SIGHUP,sig);
        signal(SIGTSTP,sig);
        signal(SIGCONT,sig);
        signal(SIGSTOP,sig);
        signal(SIGBUS,sig);
		pid=0;
		if(strcmp(buf,bufold)!=0) strcpy(bufold,buf);
		getcwd(buf,200); // sciezka
		char* t[5]={"\033[0;94m",user,":\033[0;32m[\033[0;36m",buf,"\033[0;32m]\033[0;93m$\033[0;0m  "}; //tworzenie tablicy dla nazwy uzytkownika i aktualnej sciezki
		contat(t); //laczenie tablicy t
		buffer=readline(tab); //wypisz tab i wczytaj cala linie (bez wczytywania \n)
		if(buffer[0]!=0) add_history(buffer); //jesli byl na wejsciu jakis znak to dodaj linie do historii
		else continue; //jesli nic nie bylo na wejsciu continue
		glob=separacja_polecen(); //podziel linie na n stringow
		if(strcmp(arg[0],"exit")==0) return 0; //jesli polecenie exit to wyjdz z petli (skoncz program)
		else if(strcmp(arg[0],"cd")==0) //jesli polecenie cd to
		{
			if(arg[1]==0) arg[1]=ps; //zeby cd dzialalo tak samo jak cd ~
		 	else if(strcmp(arg[1],"~")==0) arg[1]=ps; //jak do domowego to ustaw domowy
		 	else if(strcmp(arg[1],"-")==0) arg[1]=bufold; //jak do ostatniej lokalizacji to ustaw ostatnia lokalizacje
 			int y=chdir(arg[1]); //wejdz do katalogu
			if(y==-1) perror("cd"); //jak sie nie udalo wypisz blad
			if(errno!=0) printf("Error number: %d\n",errno); //numer bledu jesli byl
		}
		else if(strcmp(arg[0],"help")==0) help(); //jak polecenie help to odpal funkcje i wypisz informacje
		else if(strcmp(arg[0],"pwd")==0) printf("%s\n",buf); //jak pwd to wypisz sciezke
		else if(strcmp(arg[0],"cut")==0) cut(); //jak cut to odpal funkcje
		else if(strcmp(arg[0],"cal")==0) cal(); //jak cal to odpal funkcje
		else //w innym wypadku fork() i exec*()
		{
			pid=fork(); //utworz proces potomny
			if(pid==-1) //jak blad to wypisz blad
			{
				perror("fork");
				printf("Error number: %d\n",errno);
			}
			else if(pid==0) //jak proces potomny to wykonaj
			{
				execvp(arg[0],arg); //odpal funkcje
				perror("execvp"); //jak sie nie udalo to wypisz blad
            	if(errno!=0) printf("Error number: %d\n",errno); //i numer bledu
				exit(0); //zakoncz funkcje potomna
			}
			else wait(NULL); //jak proces macierzysty czekaj
		}
		if(pid==0) errno=0; //wyzeruj zmienna do bledow
	}
	return 0;
}
void help() //wypisywanie informacji o projekcie
{
	printf("\n");
	printf("\033[0;91m	***\033[0;93mMICROSHELL\033[0;91m***\n");
	printf("\033[0;94m	Autor: Michal Holubowicz\n");
	printf("	Uczelnia: Uniwersytet im. Adama Mickiewicza w Poznaniu\n");
	printf("	Moja powloka Microshell wlasnej implementacji oferuje:\n");
	printf("\033[0;35m	1) Obsluge polecen:\n");
	printf("\033[0;93m		 cd\n");
	printf("		 exit\n");
	printf("		 help\n");
	printf("		 pwd\n");
	printf("		 cut (-c,-f)\n");
	printf("		 cal\n");
	printf("\033[0;35m	2) Przekazywanie nieobslugiwanych polecen do innych programow w celu ich wykonania\n");
	printf("	3) Obsluge kolorow\n");
	printf("	4) Wyswietlanie loginu aktualnie zalogowanego uzytkownika\n");
	printf("	5) Obsluge histori polecen\n");
	printf("	6) Obsluge sygnalow\n");
	printf("	7) Uzupelnianie skladni dla plikow i katalogow\n");
	printf("	8) Obsluga argumentow w cudzyslowach, oddzielonych spacja i tabulacja\n");
	printf("\033[0;0m\n");
}
void cal() //funkcja wyswietlajaca kalendarz aktualnego miesiaca z podswietlonym dzisiejszym dniem
{
    char* m1[15]={" ","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char* m2[15]={" ","Styczen","Luty","Marzec","Kwiecien","Maj","Czerwiec","Lipiec","Sierpien","Wrzesien","Pazdziernik","Listopad","Grudzien"};
    char* t1[10]={" ","Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    char *data,*t;
    char* a[10];
    int m3[15]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    int tabl[50];
    int tyg,mies,z,l,n,number,e,v=1,c=1;
    time_t czas;
    czas=time(NULL); //czas w sekundasz od 1 stycznia 1970
    data=ctime(&czas); //data dzisiejszego dnia
    t=strtok(data,"  "); //separacja daty
	a[0]=t;
	while(t!=NULL) //separacja
	{
		t=strtok(NULL," ");
		a[v]=t;
		v++;
	}
	l=atoi(a[4]); //konwersja string na int -rok
	printf("\n");
    if(l%4==0 &&(l%100!=0 || l%400==0)) m3[2]++; //czy przestepny rok
  	printf("%c[%dm",0x1B,91); //kolor
    for(int i=1; i<=12; i++) //wypisuje miesiac
    {
        if(strcmp(m1[i],a[1])==0)
        {
            mies=i;
            printf("    %s ",m2[i]);
            break;
        }
    }
    printf("%s",a[4]); ///wypisuje rok
    printf("\033[0;94mPn Wt Sr Cz Pt Sb Nd\033[0;0m\n"); ///wypisuje dni tygodnia
    for(int i=1; i<=7; i++) //okresla numer tygodnia
    {
        if(strcmp(t1[i],a[0])==0)
        {
            tyg=i+6;
            if(tyg>7) tyg%=7;
            break;
        }
    }
    n=atoi(a[2]); //dzien
    number=n%7;
    e=number;
    for(int i=tyg+7; i>=0; i--) //wpisywanie do tablicy kalendarza na podstawie dnia
    {
        tabl[i]=e;
        e--;
    }
    e=number+1;
    for(int i=tyg+1+7; c<=m3[mies]; i++)
    {
        tabl[i]=e;
        e++;
        c++;
    }
    for(int i=1;; i++) //wypisywanie i personalizacja
    {
        if(tabl[i-1]==m3[mies]) break;
        if(tabl[i]<=0 && tabl[i+6]<=0) i+=7;
        if(tabl[i]<10) printf(" ");
        if(tabl[i]>0 && tabl[i]!=n) printf("%d ",tabl[i]);
        else if(tabl[i]==n)
        {
            printf("\033[0;95m%d\033[0;0m ",tabl[i]);
        }
        else printf("  ");
        if(i%7==0) printf("\n");
    }
	printf("\n");
}
int separacja_polecen()
{
	if(strlen(buffer)==0) return 0; //jesli bufor jest pusty to zwroc 0 argumentow
	token=strtok(buffer," \t\""); //separacja pierwszej komendy
	arg[0]=token; //bedziemy przypisywac komendy do tablicy arg[]
	int i=1;
	while(token!=NULL) //program obsluguje argumenty w cudzyslowach, oddzielone tabulacja i spacja
	{
		token=strtok(NULL," \t\"");
		arg[i]=token;
		i++;
	}
	glob=i-1;
	return glob; //zwracanie ilosci argumentow
}
int cut() //program wyswietla podana kolumne znakow lub stringow
{
	if(glob!=4) //jesli zla ilosc argumentow
	{
		printf("Error: Wrong arguments\n");
		return 0;
	}
	FILE *fd_in; //bedzie plik
	int c=atoi(arg[2])-1; //przekonwertuj na liczbe
	fd_in=fopen(arg[3],"r"); //otworz dany plik do odczytu
	if(fd_in==NULL)	printf("Error: No such file\n"); //jesli plik nie istnieje
	else
	{
		if(strcmp(arg[1],"-c")==0) //bedziemy wypisywac znak
		{
			while (fscanf(fd_in,"%[^\n]\n",bf)!=EOF) //wczytuj do bf wiersz po wierszu
			{
        		if(bf[c]!=0) printf("%c\n",bf[c]); //wypisz znak jesli istnieje
				else printf(" \n");
				for(int i=0; i<=c+1; i++)
					bf[c]=' '; //czysc buffer
    		}
    	}
		else if(strcmp(arg[1],"-f")==0) //bedziemy wypisywac kolumne
    	{
			char* tok;
			while(fscanf(fd_in,"%[^\n]\n",bf)!=EOF) //wczytuj do bf wiersz po wierszu
			{
				tok=strtok(bf,"\t "); //separacja pierwszego stringa
				ar[0]=tok; //bedziemy przypisywac stringi do tablicy a[]
				int z=1;
				while(tok!=NULL && z<=c) //dopoki jest cos w tablicy i nie mamy jeszcze potrzebnej kolumny
				{
					tok=strtok(NULL,"\t "); //separuj kolejne stringi
					ar[z]=tok; //zapisuj do tablicy
					z++;
     			}
				if(ar[c]!=0) printf("%s\n",ar[c]); //wypisz porzadany element danej kolumny jesli istnieje
				else printf("\n"); //jak nie istnieje znak nowej linii
			}
		}
		else //obsluga bledow
		{
			errno=5;
			perror("cut");
			printf("Error number: %d\n",errno);
		}
	}
}
void contat(char* t[5]) //scalanie tablicy t
{
	int m=0;
	for(int i=0; i<=4; i++)
	{
		for(int j=0; j<strlen(t[i]); j++)
		{
			tab[m]=t[i][j];
			m++;
		}
	}
	tab[m]=0;
}
