#include <string.h>
// https://github.com/adamvr/arduino-base64
#include <Base64.h>

void setup() 
{
  Serial.begin(9600);
  char key[]  = "SecureKey";
  char data[] = "Data to be encrypted";
  Serial.print("Data : ");
  Serial.println(data);

  // Store dataLen for use in base64_encode as can't trust strlen due to possible null bytes in string
  int dataLen    = strlen(data);
  int encodedLen = base64_enc_len(dataLen);
  char encoded[encodedLen];

  // Input string is overwritten with encrypted data
  rc4(key,strlen(key),data,dataLen);

  Serial.print("Encrypted : ");
  Serial.println(data);

  base64_encode(encoded, data, dataLen); 

  Serial.print("Encoded : ");
  Serial.println(encoded);

  int decodedLen = base64_dec_len(encoded, strlen(encoded));

  char decoded[decodedLen];

  base64_decode(decoded, encoded, strlen(encoded));

	// Decoded is overwritted with decrypted string	
  rc4(key,strlen(key),decoded,decodedLen);

  Serial.print("Decrypted : ");
  Serial.println(decoded);
  Serial.println(strlen(decoded));

}

void loop()
{

}

void rc4(char *key, uint16_t keySize, char *data, uint16_t dataSize)
{

  uint8_t S[256];

  for (uint16_t i = 0; i < 256; i++){
    S[i] = i;
  }

  uint16_t j = 0;
  for (uint16_t i = 0; i < 256; i++){
    j = (j + S[i] + key[i % keySize] ) % 256;   
    // Start Swap
    uint8_t temp = S[i]; 
    S[i] = S[j];
    S[j] = temp; 
    // Completed Swap
  }

  j = 0;
  uint16_t k = 0;
  for (uint16_t i = 0; i < dataSize; i++) {
    k = ( k + 1 ) % 256;
    j = ( j + S[k] ) % 256;
    //Start Swap
    uint8_t temp = S[k]; 
    S[k] = S[j];
    S[j] = temp; 
    // Completed Swap
    data[i] = data[i] ^ S[(S[k]+S[j]) % 256];
  }

}

