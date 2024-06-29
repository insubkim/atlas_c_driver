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
   
   while (1)
   {
      client = mongoc_client_pool_pop (pool);

      clock_t a,b;
      a = clock();

      bson_t ping = BSON_INITIALIZER;
      BSON_APPEND_INT32 (&ping, "serverStatus", 1);

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
   const char *uri_string[6] = {
      "mongodb+srv://insub:123@ac-wqfyxog-shard-00-00.j15namd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-wqfyxog-shard-00-01.j15namd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-wqfyxog-shard-00-02.j15namd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-t1n90yr-shard-00-00.ai90jjt.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-t1n90yr-shard-00-01.ai90jjt.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-t1n90yr-shard-00-02.ai90jjt.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"
      };

   mongoc_uri_t *uri[6];
   bson_error_t error;
   mongoc_client_pool_t *pool[6];
   pthread_t threads[100];
   unsigned i;
   void *ret;

   mongoc_init ();

   for (int i = 0; i < 6; i++) {
      uri[i] = mongoc_uri_new_with_error (uri_string[i], &error);
      if (!uri[i]) {
         fprintf (stderr,
                  "failed to parse URI: %s\n"
                  "error message:       %s\n",
                  uri_string[i],
                  error.message);
         return EXIT_FAILURE;
      }

      pool[i] = mongoc_client_pool_new (uri[i]);
      mongoc_client_pool_set_error_api (pool[i], 2);
   }

   for (i = 0; i < 100; i++) {
      pthread_create (&threads[i], NULL, worker, pool[i % 6]);
   }

   for (i = 0; i < 100; i++) {
      pthread_join (threads[i], &ret);
   }

   for (int i = 0; i < 6; i++){
      mongoc_client_pool_destroy (pool[i]);
      mongoc_uri_destroy (uri[i]);
   }

   mongoc_cleanup ();

   return EXIT_SUCCESS;
}