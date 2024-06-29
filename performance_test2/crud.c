#include <mongoc/mongoc.h>
#include <pthread.h>
#include <stdio.h>

static void *
worker (void *data)
{
   mongoc_client_pool_t *pool = data;
   mongoc_client_t *client;
   bson_error_t error;
   bool r;
   
   int i = 0;
   for (;; i++) 
   {
      client = mongoc_client_pool_pop (pool);

      clock_t a,b;
      a = clock();

      bson_t ping = BSON_INITIALIZER;
      if (i % 10 == 0)      BSON_APPEND_INT32 (&ping, "serverStatus", 1);
      if (i % 10 == 1)      BSON_APPEND_INT32 (&ping, "top", 1);
      if (i % 10 == 2)      BSON_APPEND_INT32 (&ping, "dbStats", 1);
      if (i % 10 == 3)      BSON_APPEND_INT32 (&ping, "currentOp", 1);
      if (i % 10 == 4)      BSON_APPEND_INT32 (&ping, "listCommands", 1);
      if (i % 10 == 5)      BSON_APPEND_INT32 (&ping, "dbStats", 1);
      if (i % 10 == 6)      BSON_APPEND_INT32 (&ping, "buildInfo", 1);//
      if (i % 10 == 7)      BSON_APPEND_INT32 (&ping, "serverStatus", 1);//
      if (i % 10 == 8)      BSON_APPEND_INT32 (&ping, "dbStats", 1);//
      if (i % 10 == 9)      BSON_APPEND_INT32 (&ping, "currentOp", 1);//



      r = mongoc_client_command_simple (client, "admin", &ping, NULL, NULL, &error);

      if (!r) {
         fprintf (stderr, "%s\n", error.message);
      }
      b = clock();
      printf("%lf\n", (double)(b - a) / CLOCKS_PER_SEC);
   
      bson_destroy (&ping);

      mongoc_client_pool_push (pool, client);
   }

   return NULL;
}

int
main (int argc, char *argv[])
{
   const char *uri_string = "mongodb+srv://insub:123@cluster0.j15namd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";
   mongoc_uri_t *uri;
   bson_error_t error;
   mongoc_client_pool_t *pool;
   pthread_t threads[10];
   unsigned i;
   void *ret;

   mongoc_init ();

   if (argc > 1) {
      uri_string = argv[1];
   }

   uri = mongoc_uri_new_with_error (uri_string, &error);
   if (!uri) {
      fprintf (stderr,
               "failed to parse URI: %s\n"
               "error message:       %s\n",
               uri_string,
               error.message);
      return EXIT_FAILURE;
   }

   pool = mongoc_client_pool_new (uri);
   mongoc_client_pool_set_error_api (pool, 2);

   for (i = 0; i < 10; i++) {
      pthread_create (&threads[i], NULL, worker, pool);
   }

   for (i = 0; i < 10; i++) {
      pthread_join (threads[i], &ret);
   }

   mongoc_client_pool_destroy (pool);
   mongoc_uri_destroy (uri);

   mongoc_cleanup ();

   return EXIT_SUCCESS;
}