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
   bson_free (string);

   bson_destroy (b);
   return 0;
}