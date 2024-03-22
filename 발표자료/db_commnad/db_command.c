#include <mongoc/mongoc.h>
#include <assert.h>

#include "cJSON.h"

void  print_json(const char* json) {
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
  const char *uri_string = "https://www.mongodb.com/community/forums/t/connect-to-specific-instance-using-mongosh/265787";
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
  assert(uri);
  
  
  //Create a new client instance
  client = mongoc_client_new_from_uri (uri);
  assert(client);

  database = mongoc_client_get_database (client, "admin");

  /* Command list
   * hello 
   * 
   *
   *
   *
   *
   */
  command = BCON_NEW ("hello", BCON_INT32 (1));
  retval = mongoc_client_command_simple (
    client, "admin", command, NULL, &reply, &error);
  assert(retval);

  str = bson_as_json (&reply, NULL);
  print_json(str);
  
  // print_bson_value_with_key(&reply, "pid");
  // reply.AT("pid.0.name")

  bson_destroy (&reply);
  bson_destroy (command);
  bson_free (str);

  mongoc_database_destroy (database);
  mongoc_uri_destroy (uri);
  mongoc_client_destroy (client);
  mongoc_cleanup ();

  return EXIT_SUCCESS;
}