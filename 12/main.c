#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cJSON.h"

/* 
 * rest api -> print 
 */

inline void  print_json_pretty(const char* json) {
   const cJSON *cjson = cJSON_Parse(json);
   const char  *s = cJSON_Print(cjson);

   printf("\n");   
   printf("%s\n", s);
   printf("\n");
}

/*
atlas api example 
curl --user "turmmrng:84df3787-314b-409f-a022-8ee40ee80269" --digest \
      --header "Content-Type: application/json" \
      --header "Accept: application/vnd.atlas.2023-02-01+json" \
      --include \
      --request GET "https://cloud.mongodb.com/api/atlas/v2/groups?pretty=true"
*/
void  run_command(int argc, char* argv[], char* envp[], char* buf) {
   char* execve_argv[30] = {0, };
   for (int i = 1; i < argc; i++) {
      execve_argv[i - 1] = malloc(1024);
      memcpy(execve_argv[i - 1], argv[i], strlen(argv[i]) + 1);
   }
   execve_argv[argc - 1] = 0;

   for (int i = 0; execve_argv[i]; i++) {
      printf("%s\n", execve_argv[i]);
   }
   //pipe for ipc
   int _pipe[2];
   pipe(_pipe);
   int in = dup(0);
   int out = dup(1);
   dup2(_pipe[0], 0);
   dup2(_pipe[1], 1);
   
   int pid = fork();
   if (pid == 0) {//child
      int i = execve(execve_argv[0], (char**)execve_argv, envp);
      if (i == -1) printf("execve fail!\n");
   }
   //wait child finish
   int wstatus;
   waitpid(-1, &wstatus, 0);
   read(_pipe[0], buf, 65535);
   //clean
   close(_pipe[0]);
   close(_pipe[1]);
   dup2(in, 0);
   dup2(out, 1);
   close(in);
   close(out);
   for (int i = 1; i < argc; i++)
      free(execve_argv[i - 1]);
}

int main (int argc, char* argv[], char* envp[])
{
   if (argc == 1) assert(0); 
   
   char buf[65536] = {0,};
   //run curl
   run_command(argc, argv, envp, buf);

   //find response body start
   printf("%s\n", buf);

   char* start = strstr(buf, "\n\n");
   if (start == NULL) assert(0);
   start = strstr(buf, "\n\n");
   if (start == NULL) assert(0);
   printf("%s\n", start);
   //to BSON

   //to json

   //print


   return 0;
}