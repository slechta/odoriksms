

#include <stdio.h>
#include "odoriksms.h"

int main(int argc, char *argv[])
{	
  if (argc != 6)
  {
	   printf("Odorik.cz APIv1 SMS sender v1.0\n");
       printf("Usage: sendsms <api_username> <api_password> <sender#> <recipient#> <text of SMS>\n");
       return -1;
  }
		
  char *szApiUsername = argv[1];
  char *szApiPassword = argv[2];
  char *szSender      = argv[3];
  char *szRecipient   = argv[4];
  char *szText        = argv[5];
		
  curl_global_init(CURL_GLOBAL_DEFAULT);

	int errcode = odorik_send_sms(szApiUsername, szApiPassword, szSender, szRecipient, szText);
	printf("%s\n", odorik_sms_translate_error(errcode));
	
  curl_global_cleanup();
  return errcode;
}

