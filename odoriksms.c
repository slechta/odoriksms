/* -----------------------------------------------------------------------------
 * Copyright (c) 2013 Pavel SLECHTA <slechta@email.cz>                         |
 * Permission is hereby granted, free of charge, to any person obtaining a     |
 * copy of this software and associated documentation files                    |
 * (the "Software"), to deal in the Software without restriction, including    |
 * without limitation the rights to use, copy, modify, merge, publish,         |
 * distribute, sublicense, and/or sell copies of the Software, and to          |
 * permit persons to whom the Software is furnished to do so, subject          |
 * to the following conditions:                                                |
 *                                                                             |
 * The above copyright notice and this permission notice shall be included     |
 * in all copies or substantial portions of the Software.                      |
 *                                                                             |
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS     |
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                  |
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      |
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        |
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        |
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           |
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      |
------------------------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <curl/curl.h>

#include "odoriksms.h"

// 0 - does not start with ...
int string_starts_with(char *szText, char *toFind)
{
	size_t originalLen = strlen(szText);
	size_t toFindLen = strlen(toFind);
	
	if (originalLen == 0) return -1;
	if (toFindLen == 0) return -1;
	
	if (toFindLen > originalLen) return 0;
	
	return !memcmp(szText, toFind, toFindLen);
}

struct OdorikSmsResponse
{
	size_t responseLength;
	char response[ODORIK_SMS_RESPONSE_MAXLENGTH + 1];
};
 
void odorik_sms_response_init(struct OdorikSmsResponse *osm)
{
	osm->responseLength = 0;
}

int odorik_sms_response_code(struct OdorikSmsResponse *osm)
{
	if (osm->responseLength > ODORIK_SMS_RESPONSE_MAXLENGTH) return -1;
	osm->response[osm->responseLength] = 0; // terminate string
	
	char *response = osm->response;	
	
	if (string_starts_with(response, "error"))
	{
		char *errstring = response + strlen("error");
		
		if (string_starts_with(errstring, " authentication_required")) return SMS_RET_AUTHENTICATION_REQUIRED;
		if (string_starts_with(errstring, " authentication_failed")) return SMS_RET_AUTHENTICATION_FAILED;
		if (string_starts_with(errstring, " missing_argument"))	return SMS_RET_MISSING_ARGUMENT;
		if (string_starts_with(errstring, " empty_message")) return SMS_RET_EMPTY_MESSAGE;
		if (string_starts_with(errstring, " forbidden_sender"))	return SMS_RET_FORBIDDEN_SENDER;
		if (string_starts_with(errstring, " unsupported_recipient")) return SMS_RET_UNSUPPORTED_RECIPIENT;
		if (string_starts_with(errstring, " low_balance")) return SMS_RET_LOW_BALANCE;
		if (string_starts_with(errstring, " gateway_failed")) return SMS_RET_GATEWAY_FAILED;
		if (string_starts_with(errstring, " invalid_delay_format"))	return SMS_RET_INVALID_DELAY;
		if (string_starts_with(errstring, " delayed_into_past")) return SMS_RET_DELAYED_INTO_PAST;
		
		printf("### urecognized error\n");
		return SMS_RET_FAILURE;
	}
	
	if (string_starts_with(response, "successfully_enqueued")) return SMS_RET_QUEUED;
	if (string_starts_with(response, "successfully_sent")) return SMS_RET_SUCCESS;
	return SMS_RET_FAILURE;
}

size_t odorik_sms_writefunction(char *ptr, size_t size, size_t nmemb, void *userdata)
{	
	struct OdorikSmsResponse *osm = (struct OdorikSmsResponse*) userdata;
	
	if (nmemb == 0)	return 0;
	
	if (nmemb + osm->responseLength > ODORIK_SMS_RESPONSE_MAXLENGTH)
	{
		return 0; // signal error
	}
	
	memcpy(osm->response + osm->responseLength, ptr, nmemb);
	osm->responseLength += nmemb;

	return nmemb;
}
 
int odorik_send_sms(char *szUsername, char *szPassword, char *szSender, char *szRecipient, char *szMessage)
{
  CURL *curl;
  CURLcode res;
  struct curl_httppost *formpost = NULL;
  struct curl_httppost *lastptr = NULL;

  if (curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, ODORIK_API_USER, CURLFORM_COPYCONTENTS, szUsername, CURLFORM_END))
  {
		printf("ferror: curl_formadd() failed");
		return SMS_RET_FAILURE;
  }
  
  if (curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, ODORIK_API_PASSWORD, CURLFORM_COPYCONTENTS, szPassword, CURLFORM_END))
  {
		printf("ferror: curl_formadd() failed");
		return SMS_RET_FAILURE;
  }
  
  if (curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, ODORIK_API_SENDER, CURLFORM_COPYCONTENTS, szSender, CURLFORM_END))
  {
		printf("ferror: curl_formadd() failed");
		return SMS_RET_FAILURE;
  }
  
  if (curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, ODORIK_API_RECIPIENT, CURLFORM_COPYCONTENTS, szRecipient, CURLFORM_END))
  {
		printf("ferror: curl_formadd() failed");
		return SMS_RET_FAILURE;
  }
  
  if (curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, ODORIK_API_MESSAGE, CURLFORM_COPYCONTENTS, szMessage, CURLFORM_END))
  {
		printf("ferror: curl_formadd() failed");
		return SMS_RET_FAILURE;
  }
  
  curl = curl_easy_init();
  
  if (curl == NULL)
  {
	  printf("ferror: curl_easy_init() failed");
	  return SMS_RET_FAILURE;
  } 
 
  if (curl_easy_setopt(curl, CURLOPT_URL, ODORIK_API_SMSURLREQ) != CURLE_OK)
  {
	  printf("ferror: curl_easy_setopt() failed");
	  return SMS_RET_FAILURE;
  }
  
  if (curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost) != CURLE_OK)
  {
	  printf("ferror: curl_easy_setopt() failed");
	  return SMS_RET_FAILURE;
  }

#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */ 
    if (curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L) != CURLE_OK)
    {
		printf("ferror: curl_easy_setopt() failed");
	    return SMS_RET_FAILURE;
	}
#endif
 
#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */ 
    if (curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L) != CURLE_OK)
    {
		printf("ferror: curl_easy_setopt() failed");
	    return SMS_RET_FAILURE;	
	}
#endif
	
	struct OdorikSmsResponse osm;
	odorik_sms_response_init(&osm);
	
	if (curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &osm) != CURLE_OK)
	{
		printf("ferror: curl_easy_setopt() failed");
	    return SMS_RET_FAILURE;
	}
	
	if (curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, odorik_sms_writefunction) != CURLE_OK)
	{
		printf("ferror: curl_easy_setopt() failed");
	    return SMS_RET_FAILURE;	
	}
	
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
 
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    
    return odorik_sms_response_code(&osm);
}

const char *odorik_sms_translate_error(int smsErrorCode)
{
	switch(smsErrorCode)
	{
		case SMS_RET_SUCCESS:
			return "success";		
		case SMS_RET_AUTHENTICATION_FAILED:
			return "authentication failed";
		case SMS_RET_AUTHENTICATION_REQUIRED:
			return "authentication required";
		case SMS_RET_MISSING_ARGUMENT:
			return "missing argument";
		case SMS_RET_EMPTY_MESSAGE:
			return "empty message";
		case SMS_RET_FORBIDDEN_SENDER:
			return "forbidden sender";
		case SMS_RET_UNSUPPORTED_RECIPIENT:
			return "unsupported client";
		case SMS_RET_LOW_BALANCE:
			return "low balance";
		case SMS_RET_GATEWAY_FAILED:
			return "gateway failed";
		case SMS_RET_INVALID_DELAY:
			return "ivalid delay";
		case SMS_RET_DELAYED_INTO_PAST:
			return "delayed into past";
		case SMS_RET_FAILURE:
			return "general failure";
		default:
			return "fatal - unknown error";
	}	
}



