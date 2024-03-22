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

int main (void)
{
  // 연결 application code 
  // const char *uri_string = "mongodb+srv://admin:admin@sherpa.5ckgp.mongodb.net/?retryWrites=true&w=majority&appName=sherpa";
  // const char *uri_string = "mongodb+srv://jjkant1948:tkwk2828!@cluster0.ekqqkuy.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";
  const char *uri_string = "mongodb://jjkant1948:tkwk2828!@ac-fpdg6sc-shard-00-00.ekqqkuy.mongodb.net/?authSource=admin&replicaSet=atlas-3kezsv-shard-0&tls=true&directConnection=true";

  // "ac-fpdg6sc-shard-00-00.ekqqkuy.mongodb.net:27017", "atlas-3kezsv-shard-0"

  mongoc_uri_t *uri;
  mongoc_client_t *client;
  mongoc_database_t *database;
  mongoc_collection_t *collection;
  mongoc_cursor_t *cursor;
  const bson_t *doc;
  bson_t *query;
  bson_t *command, reply, *insert, *update;
  bson_error_t error;
  char *str;
  bool retval;

  // C driver 초기화
  mongoc_init ();

  // MongoDB URI object 생성
  uri = mongoc_uri_new_with_error(uri_string, &error);
  assert(uri);

  // client instance 생성
  client = mongoc_client_new_from_uri(uri);
  assert(client);

  database = mongoc_client_get_database(client, "test");
  collection = mongoc_client_get_collection(client, "test", "data");

  goto ping;

ping :
  {
    command = BCON_NEW ("dbstats", BCON_INT32 (1));
    retval = mongoc_client_command_simple (
      client, "admin", command, NULL, &reply, &error);
    assert(retval);

    // 응답 BSON -> JSON 변환 후 출력
    str = bson_as_json (&reply, NULL);
    print_json(str);
    bson_free (str);
    goto clean_up;
  }

insert :
  {
    insert = BCON_NEW ("sherpa", BCON_UTF8 ("atlas mongoDB"));
    retval = mongoc_collection_insert_one (collection, insert, NULL, NULL, &error);
    assert(retval);
    bson_free (insert);
    goto find;
  }

update :
  {
   query = BCON_NEW ("sherpa", BCON_UTF8 ("atlas mongoDB"));
   update = BCON_NEW ("$set",
                      "{",
                      "sherpa",
                      BCON_UTF8 ("cloud APM"),
                      "}");
    retval = mongoc_collection_update_many (
          collection, query, update, NULL, NULL, &error);
    assert(retval);
    bson_free (query);
    bson_free (update);
    goto find;
  }

delete :
  {
    query = BCON_NEW ("sherpa", BCON_UTF8 ("cloud APM"));
    retval = mongoc_collection_delete_many (
          collection, query, NULL, NULL, &error);
    bson_free (query);
    goto find;
  }

find :
  {
    collection = mongoc_client_get_collection (client, "test", "data");
    query = bson_new ();

    cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);

    while (mongoc_cursor_next (cursor, &doc)) {
      str = bson_as_canonical_extended_json (doc, NULL);
      print_json(str);
      bson_free (str);
    }
    bson_free (query);
    mongoc_cursor_destroy (cursor);
  }

clean_up :
  /*
  * Release our handles and clean up libmongoc
  */
  mongoc_collection_destroy (collection);
  mongoc_database_destroy (database);
  mongoc_uri_destroy (uri);
  mongoc_client_destroy (client);
  mongoc_cleanup ();

  return EXIT_SUCCESS;
}