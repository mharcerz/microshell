///Michal Holubowicz gr.12

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <ncurses.h>
#include <readline/readline.h> /// aby skompilować program należy dodać
#include <readline/history.h> /// opcje -lreadline aby owe dwie biblioteki można było używać

char path_buffer[4095], array[5000], pwd[4095], buf_old[4095];
char *buffer, *token, *user;
int pid, arg_counter;
char *arg[100], *ar[205];

void command_separation();

void help();

void cal();

void cut();

void cd();

void concat(char *t[5]);

void sig(int syg);

int main() {
    user = getlogin();

    if (user == NULL) {
        printf("\033[0;32m Cannot get username, you will be: \033[0;93m John_Doe\n");
        user = "John_Doe";
    }

    getcwd(pwd, 200);

    while (1) {
    /* ********** capturing signals ********** */
        signal(SIGINT, sig);
        signal(SIGQUIT, sig);
        signal(SIGHUP, sig);
        signal(SIGTSTP, sig);
        signal(SIGCONT, sig);
        signal(SIGSTOP, sig);
        signal(SIGBUS, sig);

        pid = 0;

        if (strcmp(path_buffer, buf_old) != 0)
            strcpy(buf_old, path_buffer);

        getcwd(path_buffer, 200); // path


        char *a[5] = {"\033[0;94m\0", // creating an array for username and current path
                      user,
                      ":\033[0;32m[\033[0;36m\0",
                      path_buffer,
                      "\033[0;32m]\033[0;93m$\033[0;0m  \0"};

        concat(a); // concat an array a
        buffer = readline(array); // print tab and read all line (without \n)

        if (buffer[0] != 0) // if there was any character on input
            add_history(buffer); // add lines to history
        else
            continue; // if there was nothing on the input, continue

        command_separation(); // seperate command to n-strings

        if (strcmp(arg[0], "exit") == 0) // if the command exit, then return
            return 0;

        else if (strcmp(arg[0], "cd") == 0) // if the command cd, then cd()
            cd();
        else if (strcmp(arg[0], "help") == 0) // if the command help, then help()
            help();
        else if (strcmp(arg[0], "pwd") == 0) // if the command pwd, then print path
            printf("%s\n", path_buffer);
        else if (strcmp(arg[0], "cut") == 0) // if the command cut, then cut()
            cut();
        else if (strcmp(arg[0], "cal") == 0) // if the command cal, then cal()
            cal();
        else { // if other command, then fork() and exec*

            pid = fork(); // create a child process
            if (pid == -1) { // if error, then print error

                perror("fork");
                printf("Error number: %d\n", errno);

            } else if (pid == 0) { // if a child process, then execute

                execvp(arg[0], arg); // call a function
                perror("execvp"); // if it failed, write an error
                if (errno != 0)
                    printf("Error number: %d\n", errno); // number of error
                exit(0); // terminate child functions
            } else
                wait(NULL); // if parent process, then wait
        }
        if (pid == 0)
            errno = 0;
    }
    return 0;
}

void help() { // writing information about the project

    printf("\n");
    printf("\033[0;91m	***\033[0;93mMICROSHELL\033[0;91m***\n");
    printf("\033[0;94m	Author: Michal Holubowicz\n");
    printf("	University: Uniwersytet im. Adama Mickiewicza w Poznaniu\n");
    printf("	My shell Microshell offers:\n");
    printf("\033[0;35m	1) Command support:\n");
    printf("\033[0;93m		 cd\n");
    printf("		 exit\n");
    printf("		 help\n");
    printf("		 pwd\n");
    printf("		 cut (-c,-f)\n");
    printf("		 cal\n");
    printf("\033[0;35m	2) Forwarding unsupported commands to other programs for execute\n");
    printf("	3) Color support\n");
    printf("	4) Displaying the login of the currently logged in user\n");
    printf("	5) Command history handler\n");
    printf("	6) Signal handling\n");
    printf("	7) File and directory syntax completion\n");
    printf("	8) Support for arguments in quotation marks, separated by spaces, tabs\n");
    printf("\033[0;0m\n");
}

void cal() { // function displaying the calendar of the current month with today's day highlighted

    char *months[15] = {" ", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char *days[10] = {" ", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char *date, *temp;
    char *sep[10];
    int days_month[15] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int output_cal[50];
    int week, month, year, day_of_month, day_of_week, j = 1, c;
    time_t czas;
    czas = time(NULL); // time in seconds since January 1, 1970
    date = ctime(&czas); // date of today
    temp = strtok(date, "  "); // date separation
    sep[0] = temp;
    while (temp != NULL) {// separation

        temp = strtok(NULL, " ");
        sep[j] = temp;
        j++;
    }
    year = atoi(sep[4]); // cast string to int
    printf("\n");
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) days_month[2]++; // for leap year
    printf("%c[%dm", 0x1B, 91); // color
    for (int i = 1; i <= 12; i++) { // print month
        if (strcmp(months[i], sep[1]) == 0) {
            month = i;
            printf("    %s ", months[i]);
            break;
        }
    }
    printf("%s", sep[4]); // print year
    printf("\033[0;94mMo Tu We Th Fr Sa Su\033[0;0m\n"); // print days of week
    for (int i = 1; i <= 7; i++) { // specifies the week number

        if (strcmp(days[i], sep[0]) == 0) {
            week = i + 6;
            if (week > 7) week %= 7;
            break;
        }
    }
    day_of_month = atoi(sep[2]); // day
    day_of_week = day_of_month % 7;
    j = day_of_week;
    for (int i = week + 7; i >= 0; i--) { // writing to the calendar array by day

        output_cal[i] = j;
        j--;
    }
    j = day_of_week + 1;

    c   = week + 1 + 7;
    for (int i = 1; i <= days_month[month]; i++) {
        output_cal[c] = j;
        j++;
        c++;
    }
    for (int i = 1;; i++) { // printing and personalization

        if (output_cal[i - 1] == days_month[month]) break;
        if (output_cal[i] <= 0 && output_cal[i + 6] <= 0) i += 7;
        if (output_cal[i] < 10) printf(" ");
        if (output_cal[i] > 0 && output_cal[i] != day_of_month) printf("%d ", output_cal[i]);
        else if (output_cal[i] == day_of_month) {
            printf("\033[0;95m%d\033[0;0m ", output_cal[i]);
        } else printf("  ");
        if (i % 7 == 0) printf("\n");
    }
    printf("\n");
}

void command_separation() {
    if (strlen(buffer) != 0) {

        token = strtok(buffer, " \t\""); // separation of the first command
        arg[0] = token; // we will assign commands to the arg table
        int i = 1;
        while (token != NULL) { // the program supports arguments in quotation marks, separated by tab and space

            token = strtok(NULL, " \t\"");
            arg[i] = token;
            i++;
        }
        arg_counter = i - 1;
    } else
        arg_counter = 0;
}

void cut() { // the program displays the given column of characters or strings

    char buffer[4000];

    if (arg_counter == 4) { // if good number of arguments

        FILE *fd_in;
        int c = atoi(arg[2]) - 1; // cast to int
        fd_in = fopen(arg[3], "r"); // open the file for reading

        if (fd_in == NULL)
            printf("Error: No such file\n"); // if the file does not exist
        else {
            if (strcmp(arg[1], "-c") == 0) { // we will write the sign

                while (fscanf(fd_in, "%[^\n]\n", buffer) != EOF) { // load into buffer line by line

                    if (buffer[c] != 0)
                        printf("%c\n", buffer[c]); // print the sign, if exist
                    else
                        printf(" \n");

                    for (int i = 0; i <= c + 1; i++)
                        buffer[c] = ' '; // clear buffer
                }
            } else if (strcmp(arg[1], "-f") == 0) { // we will write the column

                char *tok;
                while (fscanf(fd_in, "%[^\n]\n", buffer) != EOF) { // load into buffer line by line

                    tok = strtok(buffer, "\t "); // separation of first string
                    ar[0] = tok; // we will assign strings to the ar array
                    int z = 1;
                    while (tok != NULL && z <= c) { // as long as there is something in the array and we don't have the column we need yet

                        tok = strtok(NULL, "\t "); // separate subsequent thongs
                        ar[z] = tok; // write to array
                        z++;
                    }
                    if (ar[c] != 0) printf("%s\n", ar[c]); // wypisz żądany element danej kolumny, jeśli istnieje
                    else printf("\n"); // as there is no, then newline
                }
            } else { // Error handling

                errno = 5;
                perror("cut");
                printf("Error number: %d\n", errno);
            }
        }
    } else
        printf("Error: Wrong arguments\n");

}

void concat(char *t[5]) { // concat array t

    int _i = 0;
    for (int i = 0; i <= 4; i++) {
        for (int j = 0; j < strlen(t[i]); j++) {
            array[_i] = t[i][j];
            _i++;
        }
    }
    array[_i] = 0;
}

void sig(int syg) { // function performed after signal capture

    printf("\033[0;94m"
           "\n		Signal detected with number "
           "\033[0;0m %d,"
           "\033[0;94m if you want to do it,"
           "\033[0;93m PRESS IT AGAIN"
           "\033[0;94m, if not, then "
           "\033[0;93mPRESS ENTER\n"
           "\033[0;0m",
           syg);
    printf("%s", array);
    signal(syg, SIG_DFL); // re-interception and execute
}

void cd() {

    if (arg[1] == 0)
        arg[1] = pwd; // to make cd work the same as cd ~
    else if (strcmp(arg[1], "~") == 0)
        arg[1] = pwd; // as for home, set home path
    else if (strcmp(arg[1], "-") == 0)
        arg[1] = buf_old; // if to the last location, set the last location

    int y = chdir(arg[1]); // enter the directory

    if (y == -1)
        perror("cd"); // if it failed, print an error
    if (errno != 0)
        printf("Error number: %d\n", errno); // error number, if there was one
}