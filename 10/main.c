#include <bson/bson.h>
#include "cJSON.h"
/*
 * bson make test
 */
int
main (void)
{
   bson_error_t error;
   bson_t      *bson;
   char        *string;

   const char *json = "{\"name\": {\"first\":\"Grace\", \"last\":\"Hopper\"}}";
   bson = bson_new_from_json ((const uint8_t *)json, -1, &error);

   if (!bson) {
      fprintf (stderr, "%s\n", error.message);
      return EXIT_FAILURE;
   }

   string = bson_as_canonical_extended_json (bson, NULL);
   printf ("%s\n", string);


   cJSON *cjson = cJSON_Parse(string);
   char *s = cJSON_Print(cjson);
   printf("\n");   
   printf("%s\n", s);
   printf("\n");
   bson_free (string);


   bson_t *b;
   bson_iter_t iter;
   bson_iter_t baz;

   b = BCON_NEW ("foo", "{", "bar", "[", "{", "baz", BCON_INT32 (1), "}", "]", "}");

   if (bson_iter_init (&iter, b) &&
      bson_iter_find_descendant (&iter, "foo.bar.0.baz", &baz) &&
      BSON_ITER_HOLDS_INT32 (&baz)) {
      printf ("baz = %d\n", bson_iter_int32 (&baz));
   }

   {
      char  *string = bson_as_canonical_extended_json (b, NULL);
      cJSON *cjson = cJSON_Parse(string);
      char  *s = cJSON_Print(cjson);
      printf("\n");   
      printf("%s\n", s);
      printf("\n");
      bson_free (string);
   }

   bson_destroy (b);


   return 0;
}