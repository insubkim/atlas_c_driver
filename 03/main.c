#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>

/* 
 * mongoc -> find 
 */

int
main (void)
{
   mongoc_client_t *client;
   mongoc_collection_t *collection;
   mongoc_cursor_t *cursor;
   const bson_t *doc;
   bson_t *query;
   char *str;

   mongoc_init ();

   client =
      mongoc_client_new ("mongodb+srv://admin:admin@sherpa.5ckgp.mongodb.net/?retryWrites=true&w=majority&appName=sherpa");
   collection = mongoc_client_get_collection (client, "test", "data");
   query = bson_new ();
   cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);

   while (mongoc_cursor_next (cursor, &doc)) {
      str = bson_as_canonical_extended_json (doc, NULL);
      printf ("%s\n", str);
      bson_free (str);
   }

   bson_destroy (query);
   mongoc_cursor_destroy (cursor);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
   mongoc_cleanup ();

   return 0;
}