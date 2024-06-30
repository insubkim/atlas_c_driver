#include <mongoc/mongoc.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#define PARSE_BSON_INT32(reply, target, result) \
do { \ 
   bson_iter_t iter; \     
   bson_iter_t child; \     
   if (bson_iter_init (&iter, reply) && \
      bson_iter_find_descendant (&iter, #target, &child) && \ 
      BSON_ITER_HOLDS_INT32 (&child)) { \
      result.target = bson_iter_int32 (&child); \
   }\
   else assert(0); \
} while(0);

#define PARSE_BSON_INT64(reply, target, result) \
do { \ 
   bson_iter_t iter; \     
   bson_iter_t child; \     
   if (bson_iter_init (&iter, reply) && \
      bson_iter_find_descendant (&iter, #target, &child) && \ 
      BSON_ITER_HOLDS_INT64 (&child)) { \
      result.target = bson_iter_int64 (&child); \
   }\
   else assert(0); \
} while(0);

#define PARSE_BSON_UTF(reply, target, result) \
do { \ 
   bson_iter_t iter; \     
   bson_iter_t child; \     
   if (bson_iter_init (&iter, reply) && \
      bson_iter_find_descendant (&iter, #target, &child) && \ 
      BSON_ITER_HOLDS_UTF8 (&child)) { \
      result.target = bson_iter_utf8 (&child, NULL); \
   }\
   else assert(0); \
} while(0);

static void *
worker (void *data)
{
   mongoc_client_pool_t *pool = data;
   mongoc_client_t *client;
   bson_error_t error;
   bool r;
   
   while (1)
   {
      struct {
         // char  host[128];
         // char  version[128];
         char  *host;
         char  *version;
         long  pid;
         int   uptime;
         long  uptimeMillis;
         long  uptimeEstimate;
         struct {
            int   regular;
            int   warning;
            int   msg;
            int   user;
            int   rollovers;
         } asserts;
         struct {
            long  bytesIn;
            long  bytesOut;
            long  numRequests;
         } network;
         struct {
            long  insert;
            long  query;
            long  update;
            long  delete;
            long  getmore;
            long  command;
            struct {
               long  query;
               long  getmore;
            }  deprecated;
         } opcounters;
         struct {
            struct {
               // char  workloadType[128];
               // char  provider[128];
               // char  nodeType[128];
               // char  diskState[128];
               // char  availabilityZone[128];
               // char  region[128];
               char  *workloadType;
               char  *provider;
               char  *nodeType;
               char  *diskState;
               char  *availabilityZone;
               char  *region;
            } tags;
         } repl;
      } serverStatus; 
      
      
      
      clock_t a,b;
      a = clock();

      client = mongoc_client_pool_pop (pool);

      bson_t ping = BSON_INITIALIZER;
      BSON_APPEND_INT32 (&ping, "serverStatus", 1);

      bson_t reply;
      r = mongoc_client_command_simple (client, "admin", &ping, NULL, &reply, &error);
      if (!r) {
         fprintf (stderr, "ERROR!!! %s\n", error.message);
      } else {
         PARSE_BSON_UTF(&reply, host, serverStatus);
         PARSE_BSON_UTF(&reply, version, serverStatus);
         PARSE_BSON_INT64(&reply, pid, serverStatus);
         // PARSE_BSON_INT64(&reply, uptime, serverStatus);
         PARSE_BSON_INT64(&reply, uptimeMillis, serverStatus);
         PARSE_BSON_INT64(&reply, uptimeEstimate, serverStatus);

         PARSE_BSON_INT32(&reply, asserts.regular, serverStatus);
         PARSE_BSON_INT32(&reply, asserts.warning, serverStatus);
         PARSE_BSON_INT32(&reply, asserts.msg, serverStatus);
         PARSE_BSON_INT32(&reply, asserts.user, serverStatus);
         PARSE_BSON_INT32(&reply, asserts.rollovers, serverStatus);

         PARSE_BSON_INT64(&reply, network.bytesIn, serverStatus);
         PARSE_BSON_INT64(&reply, network.bytesOut, serverStatus);
         PARSE_BSON_INT64(&reply, network.numRequests, serverStatus);

         PARSE_BSON_INT64(&reply, opcounters.insert, serverStatus);
         PARSE_BSON_INT64(&reply, opcounters.query, serverStatus);
         PARSE_BSON_INT64(&reply, opcounters.update, serverStatus);
         PARSE_BSON_INT64(&reply, opcounters.delete, serverStatus);
         PARSE_BSON_INT64(&reply, opcounters.getmore, serverStatus);
         PARSE_BSON_INT64(&reply, opcounters.command, serverStatus);
         
         // PARSE_BSON_INT64(&reply, opcounters.deprecated.query, serverStatus);
         // PARSE_BSON_INT64(&reply, opcounters.deprecated.getmore, serverStatus);

         // PARSE_BSON_UTF(&reply, repl.tags.workloadType, serverStatus);
         // PARSE_BSON_UTF(&reply, repl.tags.provider, serverStatus);
         // PARSE_BSON_UTF(&reply, repl.tags.nodeType, serverStatus);
         // PARSE_BSON_UTF(&reply, repl.tags.diskState, serverStatus);
         // PARSE_BSON_UTF(&reply, repl.tags.availabilityZone, serverStatus);
         // PARSE_BSON_UTF(&reply, repl.tags.region, serverStatus);
      }
      
      bson_destroy (&ping);
      bson_destroy (&reply);
      
      // dbstats
      bson_t ping2 = BSON_INITIALIZER;
      BSON_APPEND_INT32 (&ping2, "dbStats", 1);
      r = mongoc_client_command_simple (client, "admin", &ping2, NULL, &reply, &error);
      if (!r) {
         fprintf (stderr, "ERROR!!! %s\n", error.message);
      }
      bson_destroy (&ping2);
      bson_destroy (&reply);
      
      // top
      bson_t ping3 = BSON_INITIALIZER;
      BSON_APPEND_INT32 (&ping3, "top", 1);
      r = mongoc_client_command_simple (client, "admin", &ping3, NULL, &reply, &error);
      if (!r) {
         fprintf (stderr, "ERROR!!! %s\n", error.message);
      }
      bson_destroy (&ping3);
      bson_destroy (&reply);
      

      mongoc_client_pool_push (pool, client);
      b = clock();
      printf("%lf\n", (double)(b - a) / CLOCKS_PER_SEC);
      // sleep(3);
   }

   return NULL;
}


int
main (int argc, char *argv[])
{
   const char *uri_string[15] = {
      "mongodb+srv://insub:123@ac-wqfyxog-shard-00-00.j15namd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-wqfyxog-shard-00-01.j15namd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-wqfyxog-shard-00-02.j15namd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-t1n90yr-shard-00-00.ai90jjt.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-t1n90yr-shard-00-01.ai90jjt.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb+srv://insub:123@ac-t1n90yr-shard-00-02.ai90jjt.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0",
      "mongodb://127.0.0.1:27020",
      "mongodb://127.0.0.1:27021",
      "mongodb://127.0.0.1:27022",
      "mongodb://127.0.0.1:27030",
      "mongodb://127.0.0.1:27031",
      "mongodb://127.0.0.1:27032",
      "mongodb://127.0.0.1:27040",
      "mongodb://127.0.0.1:27041",
      "mongodb://127.0.0.1:27042",
      };

   mongoc_uri_t *uri[15];
   bson_error_t error;
   mongoc_client_pool_t *pool[15];
   pthread_t threads[150 * 15];
   unsigned i;
   void *ret;

   mongoc_init ();

   for (int i = 0; i < 15; i++) {
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
      mongoc_client_pool_max_size (pool[i], 150);
   }

   for (i = 0; i < 150 * 15; i++) {
      pthread_create (&threads[i], NULL, worker, pool[i % 15]);
   }

   for (i = 0; i < 150 * 15; i++) {
      pthread_join (threads[i], &ret);
   }

   for (int i = 0; i < 15; i++){
      mongoc_client_pool_destroy (pool[i]);
      mongoc_uri_destroy (uri[i]);
   }

   mongoc_cleanup ();

   return EXIT_SUCCESS;
}