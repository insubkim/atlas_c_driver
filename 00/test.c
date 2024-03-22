#include <mongoc/mongoc.h>
#include <stdio.h>

static void print_all_documents (mongoc_collection_t *collection)
{
   mongoc_cursor_t *cursor;
   bson_error_t error;
   const bson_t *doc;
   char *str;
   bson_t *query;

//    query = BCON_NEW ("$query",
//                      "{",
//                      "foo",
//                      BCON_INT32 (1),
//                      "}",
//                      "$orderby",
//                      "{",
//                      "bar",
//                      BCON_INT32 (-1),
//                      "}");
    query = BCON_NEW("$query" , "{", "}");
    // bson_t *filter = BCON_NEW("name" , "cat");
    bson_t *filter = BCON_NEW("{}");

    bson_t *opts;
   cursor = mongoc_collection_find_with_opts(
      collection, filter, NULL, NULL);

   while (mongoc_cursor_next (cursor, &doc)) {
      str = bson_as_canonical_extended_json (doc, NULL);
      printf ("%s\n", str);
      bson_free (str);
   }

   if (mongoc_cursor_error (cursor, &error)) {
      fprintf (stderr, "An error occurred: %s\n", error.message);
   }

   mongoc_cursor_destroy (cursor);
   bson_destroy (query);
   bson_destroy (filter);

}

static void run_command (void)
{
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *command;
    bson_t reply;
    char *str;

    client = mongoc_client_new ("mongodb+srv://insub:tkwk2828@cluster0.ekqqkuy.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0");
    collection = mongoc_client_get_collection (client, "test", "inventory");

    // command = BCON_NEW ("lockInfo", BCON_INT32 (1));
    // if (mongoc_collection_command_simple (collection, command, NULL, &reply, &error)) {
    //     str = bson_as_json (&reply, NULL);
    //     printf ("%s\n", str);
    //     bson_free (str);
    // } else {
    //     fprintf (stderr, "Failed to run command: %s\n", error.message);
    // }

    print_all_documents(collection);

    // bson_destroy (command);
    // bson_destroy (&reply);
    mongoc_collection_destroy (collection);
    mongoc_client_destroy (client);
}

int main (int argc, char *argv[])
{
    mongoc_init ();
    run_command ();
    mongoc_cleanup ();
    printf("END\n");
    return 0;
}