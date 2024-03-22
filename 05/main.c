#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>

/*
 * delete
 */

int
main (void)
{
   mongoc_client_t *client;
   mongoc_collection_t *collection;
   bson_error_t error;
   bson_oid_t oid;
   bson_t *doc;

   mongoc_init ();

   client =
      mongoc_client_new ("mongodb+srv://admin:admin@sherpa.5ckgp.mongodb.net/?retryWrites=true&w=majority&appName=sherpa");
   collection = mongoc_client_get_collection (client, "test", "data");

   doc = bson_new ();
   bson_oid_init (&oid, NULL);
   BSON_APPEND_OID (doc, "_id", &oid);
   BSON_APPEND_UTF8 (doc, "hello2", "world");

   if (!mongoc_collection_insert_one (collection, doc, NULL, NULL, &error)) {
      fprintf (stderr, "Insert failed: %s\n", error.message);
   }

   bson_destroy (doc);

   doc = bson_new ();
   BSON_APPEND_OID (doc, "_id", &oid);

   if (!mongoc_collection_delete_one (
          collection, doc, NULL, NULL, &error)) {
      fprintf (stderr, "Delete failed: %s\n", error.message);
   }

   bson_destroy (doc);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
   mongoc_cleanup ();

   return 0;
}