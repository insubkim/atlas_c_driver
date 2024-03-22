#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>

/*
 * update
 */

int
main (void)
{
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bson_error_t error;
   bson_oid_t oid;
   bson_t *doc = NULL;
   bson_t *update = NULL;
   bson_t *query = NULL;

   mongoc_init ();

   client =
      mongoc_client_new ("mongodb+srv://admin:admin@sherpa.5ckgp.mongodb.net/?retryWrites=true&w=majority&appName=sherpa");
   collection = mongoc_client_get_collection (client, "test", "data");

   bson_oid_init (&oid, NULL);
   doc = BCON_NEW ("_id", BCON_OID (&oid), "key", BCON_UTF8 ("old_value"));

   if (!mongoc_collection_insert_one (collection, doc, NULL, NULL, &error)) {
      fprintf (stderr, "%s\n", error.message);
      goto fail;
   }

   query = BCON_NEW ("_id", BCON_OID (&oid));
   update = BCON_NEW ("$set",
                      "{",
                      "key",
                      BCON_UTF8 ("new_value"),
                      "updated",
                      BCON_BOOL (true),
                      "}");

   if (!mongoc_collection_update_one (
          collection, query, update, NULL, NULL, &error)) {
      fprintf (stderr, "%s\n", error.message);
      goto fail;
   }

fail:
   if (doc)
      bson_destroy (doc);
   if (query)
      bson_destroy (query);
   if (update)
      bson_destroy (update);

   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
   mongoc_cleanup ();

   return 0;
}