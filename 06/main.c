
#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>

int
main (void)
{
   mongoc_client_t *client;
   mongoc_collection_t *collection;
   bson_error_t error;
   bson_t *doc;
   int64_t count;

   mongoc_init ();

   client =
      mongoc_client_new ("mongodb+srv://admin:admin@sherpa.5ckgp.mongodb.net/?retryWrites=true&w=majority&appName=sherpa");
   collection = mongoc_client_get_collection (client, "test", "data");
   doc = bson_new_from_json (
      (const uint8_t *) "{\"hello\" : \"world\"}", -1, &error);

   count = mongoc_collection_count_documents (
      collection, doc, 0, 0, NULL, &error);

   if (count < 0) {
      fprintf (stderr, "%s\n", error.message);
   } else {
      printf ("%" PRId64 "\n", count);
   }

   bson_destroy (doc);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
   mongoc_cleanup ();

   return 0;
}