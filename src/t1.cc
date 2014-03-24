#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <openssl/md5.h>


int main(int argc, char** argv)
{
   for(int i = 0; i < 9999;i++) {
      FILE* fh = fopen("/tmp/test.dat", "w+b");
      if(fh != NULL) {
         printf("ok: %d   fd=%d\n", i, fileno(fh));
         fclose(fh);
         fh = NULL;
      }
      else {
         perror("failed");
      }
   }
   return 0;
}
