#ifndef __CSS_H__
#define __CSS_H__

struct dvd_device;

//css.c

int CSSisEncrypted (struct dvd_device *dvd);
int CSSAuthDisc (struct dvd_device *dvd, char *key_disc);
int CSSAuthTitle (struct dvd_device *dvd, char *key_title, int lba);
int CSSAuthTitlePath (struct dvd_device *dvd, char *key_title, char *path);
int CSSGetASF (struct dvd_device *dvd);

//css-descrable.c

int CSSDecryptTitleKey (int8_t *key_title, int8_t *key_disc);
void CSSDescramble (uint8_t *sec, uint8_t *key);

void css_init_tables (void);

#define LEN_KEY	5

#endif

