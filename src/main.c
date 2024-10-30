#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE 1000
#define MAX_ARGC 100

struct entry {
	char *program_name;
	size_t name_len;
	char *args[MAX_ARGC];
	size_t argsc;
};

enum builtin {
	CD = 1,
	NO_BUILTIN = -1
};


void print_prompt();
struct entry *create_command(char *buf, int len);
void free_entry(struct entry *e);
enum builtin which_builtin(struct entry *e);
bool is_builtin(struct entry *e);
void run_builtin(struct entry *e);
int execute_entry(struct entry* e);


int main(int argc, char *argv[])
{
	printf("Benvenuto in stupidsh\n");
	char *line = malloc(MAX_SIZE * sizeof(char));
	size_t len = 0;
	ssize_t nread = 0;
	struct entry *e;
	pid_t pid;
	int status = 0;
	int err = 0;
	while (1) {
		print_prompt();
		nread = getline(&line, &len, stdin);
		if (nread == -1)
			continue;
		e = create_command(line, len);
		// if e is builtin then
		//      run_buitin
		// else
		//      fork
		//      exec
		//      wait
		if(is_builtin(e)) run_builtin(e);
		else{
			pid = fork();
			if(pid == -1) perror("[ERR] fork");
			if(pid == 0) err = execute_entry(e);
			else {
				if(err == -1) perror("[ERR] exec");
				else wait(&status);
			}
		}
	}
	free_entry(e);
	return 0;
}

void print_prompt()
{
	char *buf = getcwd(NULL, 0);
	printf("%s λ: ", buf);
	free(buf);
};

// dati:
//    - il buffer che contiene la linea letta da stdin
//    - la lunghezza della linea
// ritorna:
//    -  una entry allocata con malloc
struct entry *create_command(char *buf, int len)
{
	size_t i = 0;
	char *token;
	struct entry *command = malloc(1 * sizeof(struct entry));
	command->program_name = strtok(buf, " ");
	command->name_len = strlen(command->program_name);
	while ((token = strtok(NULL, " ")) != NULL) {
		command->args[i] = malloc(strlen(token) * sizeof(char));
		strcpy(token, command->args[i]);
		i++;
	}
	command->argsc = i;
	return command;
};

void free_entry(struct entry *e)
{
	free(e->program_name);
	for (int i = 0; i < e->argsc; i++) {
		free(e->args[i]);
	}
	free(e);
};


// getline
// make-entry
// check if e is builtin then:
//       run_builtin
// else:
//       fork()
//       execute()
//       wait()

int execute_entry(struct entry* e)
{
	int err;
	if (e->argsc == 0) {
		err = execlp(e->program_name, (char *) NULL);
		return err;
	} else if (e->argsc > 0) {
		err = execvp(e->program_name, e->args);
		return err;
	}
};


enum builtin which_builtin(struct entry *e)
{
	if(strcmp(e->program_name, "cd") == 0){
		return CD;
	}
	return NO_BUILTIN;
};

bool is_builtin(struct entry *e)
{
	if(strcmp(e->program_name, "cd") == 0){
		return true;
	}
	return false;
};

void run_builtin(struct entry *e)
{
	enum builtin b = which_builtin(e);
	switch(b){
	case CD:
		chdir(e->args[0]);
		break;
	default:
		break;
	}
};
