typedef unsigned char byte;

extern void CryptKey1(int varient, byte const *challenge, byte *key);
extern void CryptKey2(int varient, byte const *challenge, byte *key);
extern void CryptBusKey(int varient, byte const *challenge, byte *key);

