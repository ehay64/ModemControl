This document provides information about the encoding decoding methods supported by SMS helper functions 

**************************
CDMA
**************************
Encoding
---------
1 Data encoding scheme
 - QCWWAN_SMS_DATAENCODE_7bitAscii (helper_sms.h, section 4.5.2 of 3GPP2 C.S0015-B)

2 SMS Transport layer message type
  - SMS Point-to-Point only(section 3.4 of 3GPP2 C.S0015-B)

2 SMS Teleservice message identifier type
 - SMS Submit MO->SC( section 4.5.1 of 3GPP2 C.S0015-B)

3 TeleService Identifier
 - WMT-0x1002, used for short sms (section 3.4.3.1 of 3GPP2 C.S0015-B)

4 Encoding of sender/destination address
 - digit mode and number mode as 0 only (section 3.4.3.3 of 3GPP2 C.S0015-B)

5. Long SMS
 - Not supported
 

Decoding
--------- 
1 Data encoding scheme
   For Teleservice id WMT-0x1002, this tele id used for short sms (section 3.4.3.1 of 3GPP2 C.S0015-B)
     - QCWWAN_SMS_DATAENCODE_7bitAscii (value 2)
     - QCWWAN_SMS_DATAENCODE_8bitAscii (value 0) 
     - QCWWAN_SMS_DATAENCODE_Latin (value 8)

   For Teleservice id  WEMT-0x1005, this tele id used for long sms (section 3.4.3.1 of 3GPP2 C.S0015-B)
     - QCWWAN_SMS_DATAENCODE_GSM7BitDefault (value 9)
  

2. SMS Message type
   SC->MT ( section 4.5.1 of 3GPP2 C.S0015-B)
   - sms deliver (yes) 
   - sms deliver acknowledgement (yes)
   - sms user ack (yes) 
   - sms read ack (yes) 
   - sms submit report (no)

3 Decoding of sender/destination address
  - digit mode and number mode as 0 only (section 3.4.3.3 of 3GPP2 C.S0015-B)

4. Long SMS
 - Supported



**************************
WCDMA
**************************
Encoding
---------

1 SMS Message type (section 9.2.3.1 of 3GPP TS 23.040 V14.0.0)
 - SMS Submit MO->SC only

2. Data encoding : (section 4 of 3G TS 23.038 V2.0.0)
    - UCSENCODING (value 8)
    - EIGHTBITENCODING (value 4)
    - DEFAULTENCODING (value 0)

3. For UCS2 encoding, SMS user data must be encoded by user application. 

4. Long SMS
   - supported


Decoding
--------
1 SMS Message type (section 9.2.3.1 of 3GPP TS 23.040 V14.0.0)
  SC->MT
   - SMS Deliver
   - SMS Sent Report

2. Data encoding : (section 4 of 3G TS 23.038 V2.0.0)
    - UCSENCODING (value 8)
    - EIGHTBITENCODING (value 4)
    - DEFAULTENCODING (value 0)

3. For UCS2 encoding, SMS user data must be decoded by user application.

4. Long SMS
   - Supported

