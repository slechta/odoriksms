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

#ifndef ODORIK_H
#define ODORIK_H

#define ODORIK_API_USER "user"
#define ODORIK_API_PASSWORD "password"
#define ODORIK_API_SENDER "sender"
#define ODORIK_API_RECIPIENT "recipient"
#define ODORIK_API_MESSAGE "message"

#define ODORIK_API_SMSURLREQ "https://www.odorik.cz/api/v1/sms"
 
#define ODORIK_SMS_RESPONSE_MAXLENGTH 64

#define SMS_RET_SUCCESS 0
#define SMS_RET_QUEUED 1

#define SMS_RET_AUTHENTICATION_FAILED -3
#define SMS_RET_AUTHENTICATION_REQUIRED -4
#define SMS_RET_MISSING_ARGUMENT -5
#define SMS_RET_EMPTY_MESSAGE -6
#define SMS_RET_FORBIDDEN_SENDER -7
#define SMS_RET_UNSUPPORTED_RECIPIENT -8
#define SMS_RET_LOW_BALANCE -9
#define SMS_RET_GATEWAY_FAILED -10
#define SMS_RET_INVALID_DELAY -11
#define SMS_RET_DELAYED_INTO_PAST -12
#define SMS_RET_FAILURE -13

#include <curl/curl.h>

int odorik_send_sms(char *szUsername, char *szPassword, char *szSender, char *szRecipient, char *szMessage);
const char *odorik_sms_translate_error(int smsErrorCode);


#endif
