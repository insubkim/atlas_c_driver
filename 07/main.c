#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>

int
main (void)
{
   mongoc_client_t *client;
   bson_error_t error;
   bson_t *command;
   bson_t reply;
   char *str;

   mongoc_init ();

   client = mongoc_client_new (
      "mongodb+srv://admin:admin@sherpa.5ckgp.mongodb.net/?retryWrites=true&w=majority&appName=sherpa");

   command = BCON_NEW ("getReplicationInfo", BCON_INT32 (1));
   if (mongoc_client_command_simple (
          client, "admin", command, NULL, &reply, &error)) {
      str = bson_as_canonical_extended_json (&reply, NULL);
      printf ("%s\n", str);
      bson_free (str);
   } else {
      fprintf (stderr, "Failed to run command: %s\n", error.message);
   }

   bson_destroy (command);
   bson_destroy (&reply);
   mongoc_client_destroy (client);
   mongoc_cleanup ();

   return 0;
}