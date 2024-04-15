#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define PIPE '|'
#define ECOMERCIAL '&'
#define AND_LOGIC 1
#define OR_LOGIC 2
#define BACKGROUND 1

#define MAX_ARGUMENT_SIZE 50

int background_flag = 0;
int initial_condition = 0;

// Função que conta a quantidade de caracteres c em argv
int conta_char(char **argv, char c){
    int i = 0;
    int count = 0;
    while(argv[i]){
        if(argv[i][0] == c)
            count++;
        i++;
    }
    return count;
}

// Função que retorna a posição do primeiro caractere c em argv
int posicao_char(char **argv, char c, int j){
    int i = 0;
    while(argv[i]){
        if(argv[i][0] == c){
            return i;
        }
        i++;
    }
    return -1;
}

// Função que retorna a posição do primeiro caractere && ou || em argv
int posicao_cmd_cond(char **argv){
    
    int i = 0;
    while(argv[i]){
        if((argv[i][0] == ECOMERCIAL) && (argv[i][1] == ECOMERCIAL) || (argv[i][0] == PIPE && argv[i][1] == PIPE))
            return i;
        i++;
    }
    return -1;
}

// Função que executa um comando
int executar_cmd(char **argv){
    pid_t pid;
    pid = fork();

    if(pid == 0){
        execvp(argv[0], argv);
        return 0;
    } else if (pid > 0){
        int status;
        if (background_flag != BACKGROUND){
            waitpid(pid, &status, 0);
        }
        return WEXITSTATUS(status);
    } else {
        perror("fork");
        return -1;
    }
}

// Função que executa um comando condicional
int executar_cmd_cond(char **argv, int position){
    char **condicao_comando;
    if(position != -1){
        condicao_comando = &argv[position + 1];
        initial_condition = (argv[position][0] == ECOMERCIAL) ? AND_LOGIC : OR_LOGIC;
        argv[position] = NULL;    
    } else {
        initial_condition = 0;
    }

    int status = executar_cmd(argv);

    if((initial_condition == AND_LOGIC && status == 0) || (initial_condition == OR_LOGIC && status != 0)){
        int posicao = posicao_cmd_cond(condicao_comando);
        return executar_cmd_cond(condicao_comando, posicao);
    }

    return status;
}

// Função que executa um comando com pipe
int executar_cmd_pipe(char **argv, int i){
    int fd[i][2];
    int j = 0, aux_in = STDIN_FILENO, aux_out = STDOUT_FILENO, n;
    n = posicao_char(argv, PIPE, j);
    char **comando = &argv[j];

    if(n != -1){
        comando[n - j] = NULL;
    }

    for(j = 0; j < i; j++){
        if(j != i - 1){
            if(pipe(fd[j]) < 0){
                perror("pipe");
                exit(-1);
            }
        }

        if(fork() == 0){
            if(j != 0){
                dup2(fd[j - 1][0], aux_in);
                close(fd[j - 1][0]);
                close(fd[j - 1][1]);
            }
            if(j != i - 1){
                dup2(fd[j][1], aux_out);
                close(fd[j][0]);
                close(fd[j][1]);
            }
            if(i < j){
                execvp(comando[0], comando);
                return 0;
            }
        } else if (fork() > 0){
            close(fd[i - 1][0]);
            close(fd[i - 1][1]);
            waitpid(-1, NULL, 0);
        } else {
            perror("fork");
            exit(-1);
        }
    }
    return 0;
}

int main(int argc, char **argv){

    while(1)
    {
        char comando[1024];
        printf("$ ");
        fgets(comando, sizeof(comando), stdin);

        comando[strcspn(comando, "\n")] = '\0';

        char *token = strtok(comando, " ");
        char *args[64];
        int i = 0;

        while(token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if(i == 0) {
            printf("Não foram inseridos nenhum comando\n");
            continue;
        }

        int position = posicao_cmd_cond(args);

        if(position > 0){
            executar_cmd_cond(args, position);
        } else {
            int pipes = conta_char(args, PIPE);
            if(pipes > 0){
                executar_cmd_pipe(args, pipes);
            } else {
                int n = conta_char(args, ECOMERCIAL);
                if(n > 0){
                    background_flag = BACKGROUND;
                    position = posicao_char(args, ECOMERCIAL, 0);
                    args[position] = NULL;
                }
                executar_cmd(args);
            }
        }
    }

    return 0;
}
