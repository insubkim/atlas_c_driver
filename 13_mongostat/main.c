#include <mongoc/mongoc.h>
#include <assert.h>

#include "cJSON.h"

/*
 * atlas mongodb 
 * 1. ¿¬°á
 * 2. run serverStatus
 * 3. bson -> json -> print
 * 4. select data 
 */


void  print_json_pretty(const char* json) {
   const cJSON *cjson = cJSON_Parse(json);
   const char  *s = cJSON_Print(cjson);

   printf("\n");   
   printf("%s\n", s);
   printf("\n");
}

void print_bson_value_with_key(bson_t* reply, const char* key) {
   bson_iter_t iter;
   bson_iter_t descendant;
   assert(bson_iter_init (&iter, reply));
   assert(bson_iter_find_descendant (&iter, key, &descendant));
   // assert(BSON_ITER_HOLDS_INT64 (&descendant));
   

   if (bson_iter_init (&iter, reply) &&
      bson_iter_find_descendant (&iter, key, &descendant) &&
      BSON_ITER_HOLDS_INT64 (&descendant)) {
      printf ("[result]\n%s = %ld\n",key, bson_iter_int64 (&descendant));
   } else {
      printf ("[result]\n%s = %x\n",key, bson_iter_type (&descendant));
      printf ("fail!\n");
   }
}

int main (void)
{
   const char *uri_string = "mongodb+srv://admin:admin@sherpa.5ckgp.mongodb.net/?retryWrites=true&w=majority&appName=sherpa";
   mongoc_uri_t *uri;
   mongoc_client_t *client;
   mongoc_database_t *database;
   bson_t *command, reply;
   bson_error_t error;
   char *str;
   bool retval;

   mongoc_init ();
   
   
   //Safely create a MongoDB URI object from the given string
   uri = mongoc_uri_new_with_error (uri_string, &error);
   if (!uri) {
      fprintf (stderr,
               "failed to parse URI: %s\n"
               "error message:       %s\n",
               uri_string,
               error.message);
      assert(0);
   }

   
   //Create a new client instance
   client = mongoc_client_new_from_uri (uri);
   if (!client) {
      assert(0);
   }

   /*
    * Register the application name so we can track it in the profile logs
    * on the server. This can also be done from the URI (see other examples).
    */
   // mongoc_client_set_appname (client, "sherpa_monitor_app");

   /*
    * Get a handle on the database "db_name" and collection "coll_name"
    */
   database = mongoc_client_get_database (client, "admin");

   /*
    * Do work. This example pings the database, prints the result as JSON and
    * performs an insert
    */
   command = BCON_NEW ("hello", BCON_INT32 (1));

   retval = mongoc_client_command_simple (
      client, "admin", command, NULL, &reply, &error);

   if (!retval) {
      fprintf (stderr, "%s\n", error.message);
      assert(0);
   }

   str = bson_as_json (&reply, NULL);
   print_json_pretty(str);
   
   // print_bson_value_with_key(&reply, "pid");
   // reply.AT("pid.0.name")

   bson_destroy (&reply);
   bson_destroy (command);
   bson_free (str);

   /*
    * Release our handles and clean up libmongoc
    */
   mongoc_database_destroy (database);
   mongoc_uri_destroy (uri);
   mongoc_client_destroy (client);
   mongoc_cleanup ();

   return EXIT_SUCCESS;
}