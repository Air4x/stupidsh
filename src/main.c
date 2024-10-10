#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE 1000
#define MAX_ARGC 100

struct entry {
     char* program_name;
     size_t name_len;
     char* args[MAX_ARGC];
     size_t argsc;
};


void print_prompt();
struct entry* create_command(char*, int);
void free_entry(struct entry*);


int main(int argc, char *argv[])
{
  printf("Benvenuto in stupidsh\n");
  char* line = malloc(MAX_SIZE * sizeof(char));
  size_t len = 0;
  ssize_t nread = 0;
  struct entry* e;
  pid_t pid;
  int status = 0;
  while(1){
    print_prompt();
    nread = getline(&line, &len, stdin);
    if(nread == -1) continue;
    e = create_command(line, len);
    printf("%s\n", line);
    pid = fork();
    if(pid == -1) perror("[ERROR] fork():");
    if(pid == 0) execvp(e->program_name, e->args);
    else {
      if(wait(&status) == -1){
	perror("[ERROR] wait():");
      } else {
	printf("status code: %d\n", status);   
      }
    }
  }
  free_entry(e);
  return 0;
}

void print_prompt(){
     char* buf = getcwd(NULL, 0);
     printf("%s λ: ", buf);
     free(buf);
};

// dati:
//    - il buffer che contiene la linea letta da stdin
//    - la lunghezza della linea
// ritorna:
//    -  una entry allocata con malloc
struct entry* create_command(char* buf, int len) {
     size_t i = 0;
     char* token;
     struct entry* command = malloc(1*sizeof(struct entry));
     command->program_name = strtok(buf, " ");
     command->name_len = strlen(command->program_name);
     while((token = strtok(NULL, " ")) != NULL) {
	  command->args[i] = malloc(strlen(token)*sizeof(char));
	  strcpy(token, command->args[i]);
	  i++;
     }
     command->argsc = i;
     return command;
};

void free_entry(struct entry* e){
     free(e->program_name);
     for(int i = 0; i < e->argsc; i++){
	  free(e->args[i]);
     }
     free(e);
};
