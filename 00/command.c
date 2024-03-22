#include <mongoc.h>
#include <stdio.h>

static void run_command (void)
{
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *command;
    bson_t reply;
    char *str;

    client = mongoc_client_new ("mongodb://localhost:27017/");
    // client = mongoc_client_new ("mongodb+srv://jjkant1948:tkwk2828!@cluster0.ekqqkuy.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0");

    collection = mongoc_client_get_collection (client, "admin", "test");

    command = BCON_NEW ("currentOp", BCON_INT32 (1)); 
    // command = BCON_NEW ("lockInfo", BCON_INT32 (1));

    if (mongoc_collection_command_simple (collection, command, NULL, &reply, &error)) {
        str = bson_as_json (&reply, NULL);
        printf ("%s\n", str);
        bson_free (str);
    } else {
        fprintf (stderr, "Failed to run command: %s\n", error.message);
    }

    bson_destroy (command);
    bson_destroy (&reply);
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