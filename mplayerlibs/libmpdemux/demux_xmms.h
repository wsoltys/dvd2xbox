#ifndef DEMUX_XMMS_H
#define DEMUX_XMMS_H

typedef struct
{
    uint64_t spos;   // stream position in number of output bytes from 00:00:00
    InputPlugin* ip;
}
xmms_priv_t;
#endif