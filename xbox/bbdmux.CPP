/*************************************************************************
    bbDMUX by Brent Beyeler, beyeler@home.com
*************************************************************************/

#include "bits.h"
#include <string.h>

#undef SHOW_RESYNCHS

#define MPEG_PROGRAM_END_CODE    0x000001B9
#define PACK_START_CODE          0x000001BA
#define SYSTEM_HEADER_START_CODE 0x000001BB
#define PACKET_START_CODE_PREFIX 0x000001

#define PROGRAM_ASSOCIATION_TABLE 0x0000
#define CONDITIONAL_ACCESS_TABLE  0x0001
#define NULL_PACKET               0x1FFF

#define TRANSPORT_SYNC_BYTE      0x47
#define PROGRAM_STREAM_MAP       0xBC
#define PRIVATE_STREAM_1         0xBD
#define PADDING_STREAM           0xBE
#define PRIVATE_STREAM_2         0xBF
#define ECM_STREAM               0xF0
#define EMM_STREAM               0xF1
#define PROGRAM_STREAM_DIRECTORY 0xFF
#define DSMCC_STREAM             0xF2
#define ITUTRECH222TYPEE_STREAM  0xF8

#define SUBSTREAM_AC3_0          0x80
#define SUBSTREAM_AC3_8          0x87
#define SUBSTREAM_DTS_0          0x88
#define SUBSTREAM_DTS_8          0x8F
#define SUBSTREAM_PCM_0          0xA0
#define SUBSTREAM_PCM_F          0xAF
#define SUBSTREAM_SUBPIC_0       0x20
#define SUBSTREAM_SUBPIC_1F      0x3F

#define BUFFER_SIZE              32768

static unsigned char buffer[BUFFER_SIZE];
static int bufidx;
static FILE *outfp;
static char outname[256];
static bool vob_flag;

static void transport_stream(char *fname, int argc, int PID_to_get);

void write_buffer()
{
  if (fwrite(buffer, sizeof(unsigned char), bufidx + 1, outfp) != (unsigned int)bufidx + 1)
  {
    printf("Error writing to output file %s.", outname);
    exit (1);
  }
  bufidx = -1;
}

void flush_buffer()
{
  if (bufidx >= 0)
    write_buffer();
}

int main(int argc, char* argv[])
{
  unsigned long i, j, k, PES_packet_length, PES_header_data_length;
  unsigned long PTS_DTS_flags, ESCR_flag, ES_rate_flag, DSM_trick_mode_flag;
  unsigned long additional_copy_info_flag, PES_CRC_flag, PES_extension_flag;
  unsigned long PES_private_data_flag, pack_header_field_flag;
  unsigned long program_packet_sequence_counter_flag, PSTD_buffer_flag;
  unsigned long PES_extension_flag_2, PES_extension_field_length;
  unsigned long bytes_used, encrypted;
  int substream_to_get, stream_to_get, stream_id, substream_id;
  bool firsttime = true;
  unsigned long stream_packets[256], stream_bytes[256], pack_packets;
  unsigned long system_packets, mpeg_type;
  unsigned long ac3_bytes[8], dts_bytes[8], pcm_bytes[16], subpic_bytes[32];;
  unsigned long substream_packets[256], substream_bytes[256];
  bool streams_found[256];
  bool stream_encrypted[256];
  bool substream_encrypted[256];
  bool substreams_found[256];
  bool mpeg2;
  char tmpStr[256];

  for (i = 0; i < 256; i++)
  {
    streams_found[i] = false;
    stream_packets[i] = 0;
    stream_bytes[i] = 0;
    substreams_found[i] = false;
    stream_encrypted[i] = false;
    substream_encrypted[i] = false;
    substream_packets[i] = 0;
    substream_bytes[i] = 0;
  }
  for (i = 0; i < 8; i++)
  {
    ac3_bytes[i] = 0;
    dts_bytes[i] = 0;
  }
  for (i = 0; i < 16; i++)
    pcm_bytes[i] = 0;
  for (i = 0; i < 32; i++)
    subpic_bytes[i] = 0;
  pack_packets = 0;
  system_packets = 0;
  outfp = NULL;
  stream_to_get = -1;
  substream_to_get = -1;
  mpeg2 = false;
  bufidx = -1;

  printf("bbDMUX - version 1.9, by Brent Beyeler (beyeler@home.com)\n");
  printf("  speed increases by, Apachez and Christian Vogelgsang\n\n");
  if (argc < 2 || argc == 3)
  {
    printf("bbDUMX is an MPEG transport and program stream de-multiplexor\n\n");
    printf("Usage: bbDMUX  MPEG filename  <stream id/PID  saved name>  <substream id>\n\n");
    printf("Program stream examples:\n");
    printf("  To get a list of the stream and substream id's present in an MPEG file\n");
    printf("    bbDMUX  testfile.mpg\n\n");
    printf("  To save a video stream (id 0xe0) to the file video.m2v\n");
    printf("    bbDMUX  testfile.m2p  0xe0  video.m2v\n\n");
    printf("  To save an audio stream (id 0xc0) to the file audio.m2a\n");
    printf("    bbDMUX  testfile.mpg  0xc0  audio.m2a\n\n");
    printf("  To save an AC3 audio stream (substream 0x80) to the file audio.ac3\n");
    printf("    bbDMUX  testfile.vob  0xbd  audio.ac3  0x80\n\n");
    printf("Transport stream example:\n");
    printf("  To save an video stream (PID = 0x001B) to the file video.m2v\n");
    printf("    bbDMUX  testfile.vob  0x001B  video.m2v\n");
	 exit (1);
  }

  init_getbits(argv[1]);
  strcpy(tmpStr, argv[1]);
  strlwr(tmpStr);
  vob_flag = (strstr(tmpStr, ".vob") != NULL);
  mpeg_type = 0;
  while (!mpeg_type)
  {
    if (look_ahead(32) == PACK_START_CODE)
      mpeg_type = 1;
    else
      if (look_ahead(8) == TRANSPORT_SYNC_BYTE)
        mpeg_type = 2;
      else
        getbits(8);
  }
  if (!mpeg_type)
  {
    printf("\nUnknown file type.\n");
    exit(1);
  }

  if (argc > 2)
  {
    sscanf(argv[2], "0x%x", &i);
    if (mpeg_type == 1)
    {
      if (i < 0 || i > 0xFF)
      {
        printf("Stream ID must be in the range 0..0xFF\n");
        exit (1);
      }
    }
    else
    {
      if (i < 0 || i > 0x1FFF)
      {
        printf("PID must be in the range 0..0x1FFF\n");
        exit (1);
      }
    }
    stream_to_get = i;
    if (argc > 3)
    {
      outfp = fopen(argv[3], "wb");
      if (!outfp)
      {
        printf("Unable to open %s for output\n", argv[3]);
        exit (1);
      }
      strcpy(outname, argv[3]);
    }
    else
    {
      printf("An output filename must be specified\n");
      exit(1);
    }

    if (argc > 4)
    {
      sscanf(argv[4], "0x%x", &i);
      if (i < 0 || i > 0xFF)
      {
        printf("Substream ID must be in the range 0..0xFF\n");
        exit (1);
      }
      substream_to_get = i;
      vob_flag = true;
    }
  }
  else
    if (mpeg_type == 1)
      printf("Scanning for stream id's, press control-c to quit ...\n");
    else
      printf("Scanning for PID's, press control-c to quit ...\n");

  if (outfp)
  {
    if (mpeg_type == 1)
    {
      if ((vob_flag) && (stream_to_get == PRIVATE_STREAM_1) && (substream_to_get > -1))
        printf("Saving stream 0x%02X, substream 0x%02X into file %s...\n", stream_to_get, substream_to_get, argv[3]);
      else
        printf("Saving stream 0x%02X into file %s...\n", stream_to_get, argv[3]);
    }
    else
      printf("Saving PID stream 0x%04X into file %s...\n", stream_to_get, argv[3]);
  }

  if (mpeg_type == 2)
  {
    transport_stream(argv[1], argc, stream_to_get);
    exit(0);
  }

  do
  {
    i = getbits(32);
//nextpass:
    switch (i)
    {
      case PACK_START_CODE:
        pack_packets++;
        if (firsttime)
        {
          mpeg2 = look_ahead(2) == 1; // check if MPEG-1 or MPEG-2
          if (mpeg2)
            printf("\nFile %s is an MPEG-2 Program Stream\n\n", argv[1]);
          else
            printf("\nFile %s is an MPEG-1 Program Stream\n\n", argv[1]);
          firsttime = false;
        }
        if (mpeg2)
        {
          getbits(32);   // MPEG2 pack header
          getbits(32);
          getbits(13);
          i = getbits(3);
          for (j = 0; j < i; j++)
            getbits(8);  // stuffing bytes
        }
        else
        {
          getbits(32);   // MPEG1 pack header
          getbits(32);
        }
        break;

      case SYSTEM_HEADER_START_CODE:
        system_packets++;
        getbits(32);   // system header
        getbits(32);
        while (look_ahead(1) == 1)
          getbits(24); // P-STD info
        break;

      case MPEG_PROGRAM_END_CODE:
        break;

      default:
        if ((i >> 8) != PACKET_START_CODE_PREFIX)
//        {
          break;
//#ifdef SHOW_RESYNCHS
//          if (argc > 3)
//          {
//            printf("\nUnknown bits in stream = 0x%08X at byte %.0f\n", i, bitcount() / 8 - 4);
//            printf("\nAttempting resynch ... ");
//          }
//#endif
//          if (seek_sync(PACKET_START_CODE_PREFIX, 24))
//          {
//            i = 0x00000100 | getbits(8);
//#ifdef SHOW_RESYNCHS
//            if (argc > 3)
//              printf("succesfully resynched\n");
//#endif
//            goto nextpass;
//          }
//#ifdef SHOW_RESYNCHS
//          if (argc > 3)
//            printf("could not resynch\n");
//          else
//#endif
//            printf("\nUnknown bits in stream = 0x%08X at byte %.0f\n", i, bitcount() / 8 - 4);
//          if (outfp)
//          {
//            flush_buffer();
//            fclose(outfp);
//          }
//          exit(1);
//        }
        stream_id = i & 0x000000FF;
        stream_packets[stream_id]++;

        PES_packet_length = getbits(16);
        encrypted = 0;
        switch (stream_id)
        {
          case PROGRAM_STREAM_MAP:
          case ECM_STREAM:
          case EMM_STREAM:
          case PROGRAM_STREAM_DIRECTORY:
          case DSMCC_STREAM:
          case ITUTRECH222TYPEE_STREAM:
          case PRIVATE_STREAM_2:
          case PADDING_STREAM:
            if (stream_id == stream_to_get)
            {
              for (j = 0; j < PES_packet_length; j++)
              {
                buffer[++bufidx] = (unsigned char)getbits(8);
                if (bufidx == BUFFER_SIZE - 1)
                  write_buffer();
              }
            }
            for (j = 0; j < PES_packet_length; j++)
              getbits(8);
            stream_bytes[stream_id] += j;
            break;

          default:
            bytes_used = 0;
            if (!mpeg2)
            {
              /* flush stuffing bytes */
              while (look_ahead(8) == 0xff)
              {
                getbits(8);
                bytes_used++;
              }

              if (look_ahead(2) == 1)
              {
                /* Get STD buffer size */
                getbits(16);
                bytes_used += 2;
              }

              if (look_ahead(4) == 2)
              {
                /* Get presentation time stamp and flag it as present in packet*/
                getbits(32);
                getbits(8);
                bytes_used += 5;
              }
              else
                if (look_ahead(4) == 3)
                {
                  /* Get presentation time stamp and decoding time stamp */
                  getbits(32);
                  getbits(8);
                  getbits(32);
                  getbits(8);
                  bytes_used += 10;
                }
                else
                {
                  if (getbits(8) != 0x0f)
                  {
                    printf("Invalid bits in MPEG-1 file %s.", argv[1]);
                    exit (1);
                  }
                  bytes_used++;
                }
              stream_bytes[stream_id] += PES_packet_length - bytes_used;
              if (stream_id == stream_to_get)
              {
                for (j = 0; j < PES_packet_length - bytes_used; j++)
                {
                  buffer[++bufidx] = (unsigned char)getbits(8);
                  if (bufidx == BUFFER_SIZE - 1)
                     write_buffer();
                }
              }
              else
                for (j = 0; j < PES_packet_length - bytes_used; j++)
                  getbits(8);
            }
            else
            {
              getbits(2);
              encrypted = getbits(2);   // PES_scrambling_control
              getbits(4);
              PTS_DTS_flags = getbits(2);
              ESCR_flag = get1bit();
              ES_rate_flag = get1bit();
              DSM_trick_mode_flag = get1bit();
              additional_copy_info_flag = get1bit();
              PES_CRC_flag = get1bit();
              PES_extension_flag = get1bit();
              PES_header_data_length = getbits(8);
              if (PTS_DTS_flags > 1)
              {
                getbits(32);   // PTS stamp
                getbits(8);
                bytes_used += 5;
              }
              if (PTS_DTS_flags > 2)
              {
                getbits(32);   // DTS stamp
                getbits(8);
                bytes_used += 5;
              }

              if (ESCR_flag == 1)
              {
                getbits(32);
                getbits(16);
                bytes_used += 6;
              }

              if (ES_rate_flag == 1)
              {
                getbits(24);
                bytes_used += 3;
              }

              if (DSM_trick_mode_flag == 1)
              {
                getbits(8);
                bytes_used++;
              }

              if (additional_copy_info_flag == 1)
              {
                getbits(8);
                bytes_used++;
              }

              if (PES_CRC_flag == 1)
              {
                getbits(16);
                bytes_used += 2;
              }

              if (PES_extension_flag == 1)
              {
                PES_private_data_flag = get1bit();
                pack_header_field_flag = get1bit();
                program_packet_sequence_counter_flag = get1bit();
                PSTD_buffer_flag = get1bit();
                getbits(3);
                PES_extension_flag_2 = get1bit();
                bytes_used++;

                if (PES_private_data_flag == 1)
                {
                  for (j = 0; j < 128; j++)
                    getbits(8);
                  bytes_used += 128;
                }

                if (pack_header_field_flag == 1)
                {
                  printf("    pack header field flag value not allowed in program streams\n");
                  exit(1);
                }

                if (program_packet_sequence_counter_flag == 1)
                {
                  getbits(16);
                  bytes_used += 2;
                }

                if (PSTD_buffer_flag == 1)
                {
                  getbits(16);
                  bytes_used += 2;
                }

                if (PES_extension_flag_2 == 1)
                {
                  get1bit();
                  PES_extension_field_length = getbits(7);
                  bytes_used++;
                  for (j = 0; j < PES_extension_field_length; j++)
                  {
                    getbits(8);
                    bytes_used++;
                  }
                }
              }
              for (j = 0; j < PES_header_data_length - bytes_used; j++)
                getbits(8);

              substream_id = getbits(8);
              stream_bytes[stream_id] += PES_packet_length - PES_header_data_length - 3;
              if ((stream_id == PRIVATE_STREAM_1) && (vob_flag))
              {
                substream_packets[substream_id]++;
                j = PES_packet_length - PES_header_data_length - 3;
                substream_bytes[substream_id] += j;
                if ((substream_id >= SUBSTREAM_AC3_0) && (substream_id <= SUBSTREAM_AC3_8))
                  ac3_bytes[substream_id - SUBSTREAM_AC3_0] += j - 4;
                if ((substream_id >= SUBSTREAM_DTS_0) && (substream_id <= SUBSTREAM_DTS_8))
                  dts_bytes[substream_id - SUBSTREAM_DTS_0] += j - 4;
                if ((substream_id >= SUBSTREAM_PCM_0) && (substream_id <= SUBSTREAM_PCM_F))
                  pcm_bytes[substream_id - SUBSTREAM_PCM_0] += j - 7;
                if ((substream_id >= SUBSTREAM_SUBPIC_0) && (substream_id <= SUBSTREAM_SUBPIC_1F))
                  subpic_bytes[substream_id - SUBSTREAM_SUBPIC_0] += j - 1;
              }
              if (stream_id == stream_to_get)
              {
                if ((stream_id == PRIVATE_STREAM_1) && (vob_flag) && (substream_to_get > -1))
                {
                  if (substream_id == substream_to_get)
                  {
                    if ((substream_id >= SUBSTREAM_AC3_0) && (substream_id <= SUBSTREAM_AC3_8))
                    {
                      getbits(24);  // discard the 3 AC3 info bytes
                      j = 4;
                    }
                    else
                      if ((substream_id >= SUBSTREAM_DTS_0) && (substream_id <= SUBSTREAM_DTS_8))
                      {
                        getbits(24);  // discard the 3 DTS info bytes
                        j = 4;
                      }
                      else
                        if ((substream_id >= SUBSTREAM_PCM_0) && (substream_id <= SUBSTREAM_PCM_F))
                        {
                          getbits(24);  // discard the 6 PCM info bytes
                          getbits(24);
                          j = 7;
                        }
                        else
                          if ((substream_id >= SUBSTREAM_SUBPIC_0) && (substream_id <= SUBSTREAM_SUBPIC_1F))
                            j = 1;
                          else
                          {
                            buffer[++bufidx] = (unsigned char)substream_id;
                            j = 1;
                          }

                    for (k = 0; k < PES_packet_length - PES_header_data_length - 3 - j; k++)
                    {
                      buffer[++bufidx] = (unsigned char)getbits(8);
                      if (bufidx == BUFFER_SIZE - 1)
                        write_buffer();
                    }
                  }
                  else
                    for (j = 0; j < PES_packet_length - PES_header_data_length - 4; j++)
                      getbits(8);
                }
                else
                {
                  buffer[++bufidx] = (unsigned char)substream_id;
                  if (bufidx == BUFFER_SIZE - 1)
                    write_buffer();
                  for (j = 0; j < PES_packet_length - PES_header_data_length - 4; j++)
                  {
                    buffer[++bufidx] = (unsigned char)getbits(8);
                    if (bufidx == BUFFER_SIZE - 1)
                      write_buffer();
                  }
                }
              }
              else
                for (j = 0; j < PES_packet_length - PES_header_data_length - 4; j++)
                  getbits(8);
            }
            break;
        }
        if (argc == 2)
        {
          if ((!streams_found[stream_id]) || ((vob_flag) && (stream_id == PRIVATE_STREAM_1) && (!substreams_found[substream_id])))
          {
            switch (stream_id)
            {
              case PROGRAM_STREAM_MAP:
                printf("Found stream id 0x%02X  = Program Stream Map\n", stream_id);
                break;
              case PRIVATE_STREAM_1:
                if (vob_flag)
                {
                  if ((substream_id >= SUBSTREAM_AC3_0) && (substream_id <= SUBSTREAM_AC3_8))
                    printf("Found stream id 0x%02X  = Private Stream 1, substream 0x%02X, AC3 Audio stream %u\n", stream_id,  substream_id, substream_id - SUBSTREAM_AC3_0);
                  else
                    if ((substream_id >= SUBSTREAM_DTS_0) && (substream_id <= SUBSTREAM_DTS_8))
                      printf("Found stream id 0x%02X  = Private Stream 1, substream 0x%02X, DTS Audio stream %u\n", stream_id, substream_id, substream_id - SUBSTREAM_DTS_0);
                    else
                      if ((substream_id >= SUBSTREAM_PCM_0) && (substream_id <= SUBSTREAM_PCM_F))
                        printf("Found stream id 0x%02X  = Private Stream 1, substream 0x%02X, PCM Audio stream %u\n", stream_id, substream_id, substream_id - SUBSTREAM_PCM_0);
                      else
                        if ((substream_id >= SUBSTREAM_SUBPIC_0) && (substream_id <= SUBSTREAM_SUBPIC_1F))
                          printf("Found stream id 0x%02X  = Private Stream 1, substream 0x%02X, Subpicture stream %u\n", stream_id, substream_id, substream_id - SUBSTREAM_SUBPIC_0);
                        else
                          printf("Found stream id 0x%02X  = Private Stream 1, substream 0x%02X\n", stream_id, substream_id);
                  substreams_found[substream_id] = true;
                }
                else
                  printf("Found stream id 0x%02X  = Private Stream 1\n", stream_id);
                break;
              case PRIVATE_STREAM_2:
                printf("Found stream id 0x%02X  = Private Stream 2\n", stream_id);
                break;
              case ECM_STREAM:
                printf("Found stream id 0x%02X  = ECM Stream\n", stream_id);
                break;
              case EMM_STREAM:
                printf("Found stream id 0x%02X  = EMM Stream\n", stream_id);
                break;
              case PROGRAM_STREAM_DIRECTORY:
                printf("Found stream id 0x%02X  = Program Stream Directory\n", stream_id);
                break;
              case DSMCC_STREAM:
                printf("Found stream id 0x%02X  = DSMCC Stream\n", stream_id);
                break;
              case ITUTRECH222TYPEE_STREAM:
                printf("Found stream id 0x%02X  = ITU-T Rec. H.222.1 type E\n", stream_id);
                break;
              case PADDING_STREAM:
                printf("Found stream id 0x%02X  = Padding Stream\n", stream_id);
                break;
              default:
                if ((stream_id >= 0xE0) && (stream_id <= 0xEF))
                  printf("Found stream id 0x%02X  = Video Stream %d\n", stream_id, stream_id - 0xE0);
                else
                  if ((stream_id >= 0xC0) && (stream_id <= 0xDF))
                    printf("Found stream id 0x%02X  = MPEG Audio Stream %d\n", stream_id, stream_id - 0xC0);
                  else
                    printf("Found stream id 0x%02X  = other stream\n", stream_id);
            }
            streams_found[stream_id] = true;
          }
          if ((stream_id == PRIVATE_STREAM_1) && (vob_flag))
          {
            if (encrypted)
            {
              if (!substream_encrypted[substream_id])
              {
                printf("Stream id 0x%02X, substream 0x%02X is encrypted\n", stream_id, substream_id);
                substream_encrypted[substream_id] = true;
              }
            }
          }
          else
          {
            if (encrypted)
            {
              if (!stream_encrypted[stream_id])
              {
                printf("Stream id 0x%02X is encrypted\n", stream_id);
                stream_encrypted[stream_id] = true;
              }
            }
          }
        }
    }
  } while ((i != MPEG_PROGRAM_END_CODE) && (!end_bs()));

  printf("\nSummary:\n");
  printf("\nMPEG Packs = %d\n", pack_packets);
  if (system_packets)
    printf("System headers = %d\n", system_packets);
  for (i = 0; i < 256; i++)
  {
    if (stream_packets[i])
    {
      switch (i)
      {
        case PROGRAM_STREAM_MAP:
          printf("Program Stream Map packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          break;
        case PRIVATE_STREAM_1:
          printf("Private Stream 1 packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          if (vob_flag)
          {
            for (j = 0; j < 256; j++)
            {
              if (substream_packets[j])
              {
                if ((j >= SUBSTREAM_AC3_0) && (j <= SUBSTREAM_AC3_8))
                  printf("    AC3 Audio stream %u packets = %u, total bytes = %u\n", j - SUBSTREAM_AC3_0, substream_packets[j], ac3_bytes[j - SUBSTREAM_AC3_0]);
                else
                  if ((j >= SUBSTREAM_DTS_0) && (j <= SUBSTREAM_DTS_8))
                    printf("    DTS Audio stream %u packets = %u, total bytes = %u\n", j - SUBSTREAM_DTS_0, substream_packets[j], dts_bytes[j - SUBSTREAM_DTS_0]);
                  else
                    if ((j >= SUBSTREAM_PCM_0) && (j <= SUBSTREAM_PCM_F))
                      printf("    PCM Audio stream %u packets = %u, total bytes = %u\n", j - SUBSTREAM_PCM_0, substream_packets[j], pcm_bytes[j - SUBSTREAM_PCM_0]);
                    else
                      if ((j >= SUBSTREAM_SUBPIC_0) && (j <= SUBSTREAM_SUBPIC_1F))
                        printf("    Subpicture stream %u packets = %u, total bytes = %u\n", j - SUBSTREAM_SUBPIC_0, substream_packets[j], subpic_bytes[j - SUBSTREAM_SUBPIC_0]);
                      else
                        printf("    Substream 0x%02X packets = %u, total bytes = %u\n", j, substream_packets[j], substream_bytes[j]);
              }
            }
          }
          break;
        case PRIVATE_STREAM_2:
          printf("Private Stream 2 packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          break;
        case ECM_STREAM:
          printf("ECM Stream packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          break;
        case EMM_STREAM:
          printf("EMM Stream packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          break;
        case PROGRAM_STREAM_DIRECTORY:
          printf("Program Stream Directory packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          break;
        case DSMCC_STREAM:
          printf("DSMCC Stream packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          break;
        case ITUTRECH222TYPEE_STREAM:
          printf("ITU-T Rec. H.222.1 type E packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          break;
        case PADDING_STREAM:
          printf("Padding Stream packets = %u, total bytes = %u\n", stream_packets[i], stream_bytes[i]);
          break;
        default:
          if ((i >= 0xE0) && (i <= 0xEF))
            printf("Video stream %d packets = %u, total bytes = %u\n", i - 0xE0, stream_packets[i], stream_bytes[i]);
          else
            if ((i >= 0xC0) && (i <= 0xDF))
              printf("MPEG Audio stream %d packets = %u, total bytes = %u\n", i - 0xC0, stream_packets[i], stream_bytes[i]);
            else
              printf("Other stream 0x%02X packets = %u, total bytes = %u\n", i, stream_packets[i], stream_bytes[i]);
      }
    }
  }
  finish_getbits();
  if (outfp)
  {
    flush_buffer();
    fclose(outfp);
  }
  return (0);
}

struct stream_entry {
  unsigned long stream_id;
  unsigned long substream_id;
  unsigned long stream_packets;
  unsigned long stream_bytes;
  int stream_encrypted;
  int stream_found;
  int pcm_bytes;
  int subpic_bytes;
  int ac3_bytes;
  int dts_bytes;
};


void transport_stream(char *fname, int argc, int PID_to_get)
{
  unsigned long i, j, k, PES_packet_length, PES_header_data_length;
  unsigned long adaptation_bytes;
  int payload_unit_start_indicator, PID;
  bool firsttime = true;
  unsigned long transport_packets, packet_data_bytes;
  stream_entry *streams[8192];

  for (i = 0; i < 8192; i++)
    streams[i] = NULL;
  transport_packets = 0;
  bufidx = -1;

  do
  {
    i = getbits(8);
//nextpass:
    if (i == TRANSPORT_SYNC_BYTE)
    {
      transport_packets++;
      if (firsttime)
      {
        printf("\nFile %s is an MPEG-2 Transport Stream\n\n", fname);
        firsttime = false;
      }
      j = getbits(3);
      payload_unit_start_indicator = (j & 0x2) >> 1;

      PID = getbits(13);
      if (streams[PID] == NULL)
      {
        streams[PID] = (stream_entry*)malloc(sizeof(stream_entry));
        if (streams[PID] == NULL)
        {
          printf("Unable to allocate memory for stream entry.\n");
          exit(1);
        }
        streams[PID]->stream_id = 0;
        streams[PID]->substream_id = 0;
        streams[PID]->stream_packets = 0;
        streams[PID]->stream_bytes = 0;
        streams[PID]->stream_encrypted = 0;
        streams[PID]->stream_found = 0;
        streams[PID]->pcm_bytes = 0;
        streams[PID]->subpic_bytes = 0;
        streams[PID]->ac3_bytes = 0;
        streams[PID]->dts_bytes = 0;
      }
      streams[PID]->stream_packets++;
      getbits(2);
      i = getbits(2);
      getbits(4);
      if (i > 1)
      {
        j = getbits(8);
        for (k = 0; k < j; k++)
          getbits(8);
        adaptation_bytes = j + 1;
      }
      else
        adaptation_bytes = 0;
      if ((i == 1) || (i == 3))
      {
        packet_data_bytes = 184 - adaptation_bytes;
        if (packet_data_bytes > 0)
        {
          if ((look_ahead(24) == 0x000001) && payload_unit_start_indicator && (PID > 0x0F))
          {
            i = getbits(32);
            packet_data_bytes -= 4;
            if ((i >> 8) != PACKET_START_CODE_PREFIX)
            {
              printf("Packet start code not found.\n");
              exit(1);
            }
            streams[PID]->stream_id = i & 0x000000FF;
            PES_packet_length = getbits(16);
            packet_data_bytes -= 2;
            switch (streams[PID]->stream_id)
            {
              case PROGRAM_STREAM_MAP:
              case ECM_STREAM:
              case EMM_STREAM:
              case PROGRAM_STREAM_DIRECTORY:
              case DSMCC_STREAM:
              case ITUTRECH222TYPEE_STREAM:
              case PRIVATE_STREAM_2:
              case PADDING_STREAM:
                break;

              default:
                getbits(2);
                streams[PID]->stream_encrypted = getbits(2);   // PES_scrambling_control
                getbits(12);
                PES_header_data_length = getbits(8);
                packet_data_bytes -= (3 + PES_header_data_length);
                for (j = 0; j < PES_header_data_length; j++)
                  getbits(8);

                streams[PID]->substream_id = getbits(8);
                if ((streams[PID]->stream_id == PRIVATE_STREAM_1) && (vob_flag))
                {
                  j = PES_packet_length - PES_header_data_length - 3;
                  if ((streams[PID]->substream_id >= SUBSTREAM_AC3_0) && (streams[PID]->substream_id <= SUBSTREAM_AC3_8))
                    streams[PID]->ac3_bytes += j - 4;
                  if ((streams[PID]->substream_id >= SUBSTREAM_DTS_0) && (streams[PID]->substream_id <= SUBSTREAM_DTS_8))
                    streams[PID]->dts_bytes += j - 4;
                  if ((streams[PID]->substream_id >= SUBSTREAM_PCM_0) && (streams[PID]->substream_id <= SUBSTREAM_PCM_F))
                    streams[PID]->pcm_bytes += j - 7;
                  if ((streams[PID]->substream_id >= SUBSTREAM_SUBPIC_0) && (streams[PID]->substream_id <= SUBSTREAM_SUBPIC_1F))
                    streams[PID]->subpic_bytes += j - 1;
                }

                if ((streams[PID]->stream_id == PRIVATE_STREAM_1) && (vob_flag))
                {
                  if ((streams[PID]->substream_id >= SUBSTREAM_AC3_0) && (streams[PID]->substream_id <= SUBSTREAM_AC3_8))
                  {
                    getbits(24);  // discard the 3 AC3 info bytes
                    j = 4;
                  }
                  else
                    if ((streams[PID]->substream_id >= SUBSTREAM_DTS_0) && (streams[PID]->substream_id <= SUBSTREAM_DTS_8))
                    {
                      getbits(24);  // discard the 3 DTS info bytes
                      j = 4;
                    }
                    else
                      if ((streams[PID]->substream_id >= SUBSTREAM_PCM_0) && (streams[PID]->substream_id <= SUBSTREAM_PCM_F))
                      {
                        getbits(24);  // discard the 6 PCM info bytes
                        getbits(24);
                        j = 7;
                      }
                      else
                        if ((streams[PID]->substream_id >= SUBSTREAM_SUBPIC_0) && (streams[PID]->substream_id <= SUBSTREAM_SUBPIC_1F))
                          j = 1;
                        else
                        {
                          if (PID == PID_to_get)
                          {
                            buffer[++bufidx] = (unsigned char)streams[PID]->substream_id;
                            if (bufidx == BUFFER_SIZE - 1)
                              write_buffer();
                          }
                          j = 1;
                          streams[PID]->stream_bytes++;
                        }
                  packet_data_bytes -= j;
                }
                else
                {
                  if (PID == PID_to_get)
                  {
                    buffer[++bufidx] = (unsigned char)streams[PID]->substream_id;
                    if (bufidx == BUFFER_SIZE - 1)
                      write_buffer();
                  }
                  streams[PID]->stream_bytes++;
                  packet_data_bytes--;
                }
                break;
            }
            if (packet_data_bytes > 0)
            {
              if (PID == PID_to_get)
              {
                for (j = 0; j < packet_data_bytes; j++)
                {
                  buffer[++bufidx] = (unsigned char)getbits(8);
                  if (bufidx == BUFFER_SIZE - 1)
                    write_buffer();
                }
              }
              else
              {
                for (j = 0; j < packet_data_bytes; j++)
                  getbits(8);
              }
              streams[PID]->stream_bytes += j;
            }
          }
          else
          {
            if (PID == PID_to_get)
            {
              for (j = 0; j < packet_data_bytes; j++)
              {
                buffer[++bufidx] = (unsigned char)getbits(8);
                if (bufidx == BUFFER_SIZE - 1)
                  write_buffer();
              }
            }
            else
            {
              for (j = 0; j < packet_data_bytes; j++)
                getbits(8);
            }
            streams[PID]->stream_bytes += j;
          }
        }

        if ((argc == 2) && (!streams[PID]->stream_found))
        {
          switch (streams[PID]->stream_id)
          {
            case PROGRAM_STREAM_MAP:
              printf("Found PID 0x%04X, stream id 0x%02X = Program Stream Map\n", PID, streams[PID]->stream_id);
              break;
            case PRIVATE_STREAM_1:
              if (vob_flag)
              {
                if ((streams[PID]->substream_id >= SUBSTREAM_AC3_0) && (streams[PID]->substream_id <= SUBSTREAM_AC3_8))
                  printf("Found PID 0x%04X, stream id 0x%02X = Private Stream 1, substream 0x%02X, AC3 Audio stream %u\n", PID, streams[PID]->stream_id,  streams[PID]->substream_id, streams[PID]->substream_id - SUBSTREAM_AC3_0);
                else
                  if ((streams[PID]->substream_id >= SUBSTREAM_DTS_0) && (streams[PID]->substream_id <= SUBSTREAM_DTS_8))
                    printf("Found PID 0x%04X, stream id 0x%02X = Private Stream 1, substream 0x%02X, DTS Audio stream %u\n", PID, streams[PID]->stream_id, streams[PID]->substream_id, streams[PID]->substream_id - SUBSTREAM_DTS_0);
                  else
                    if ((streams[PID]->substream_id >= SUBSTREAM_PCM_0) && (streams[PID]->substream_id <= SUBSTREAM_PCM_F))
                      printf("Found PID 0x%04X, stream id 0x%02X = Private Stream 1, substream 0x%02X, PCM Audio stream %u\n", PID, streams[PID]->stream_id, streams[PID]->substream_id, streams[PID]->substream_id - SUBSTREAM_PCM_0);
                    else
                      if ((streams[PID]->substream_id >= SUBSTREAM_SUBPIC_0) && (streams[PID]->substream_id <= SUBSTREAM_SUBPIC_1F))
                        printf("Found PID 0x%04X, stream id 0x%02X = Private Stream 1, substream 0x%02X, Subpicture stream %u\n", PID, streams[PID]->stream_id, streams[PID]->substream_id, streams[PID]->substream_id - SUBSTREAM_SUBPIC_0);
                      else
                        printf("Found PID 0x%04X, stream id 0x%02X = Private Stream 1, substream 0x%02X\n", PID, streams[PID]->stream_id, streams[PID]->substream_id);
              }
              else
                printf("Found PID 0x%04X, stream id 0x%02X = Private Stream 1\n", PID, streams[PID]->stream_id);
              break;
            case PRIVATE_STREAM_2:
              printf("Found PID 0x%04X, stream id 0x%02X = Private Stream 2\n", PID, streams[PID]->stream_id);
              break;
            case ECM_STREAM:
              printf("Found PID 0x%04X, stream id 0x%02X = ECM Stream\n", PID, streams[PID]->stream_id);
              break;
            case EMM_STREAM:
              printf("Found PID 0x%04X, stream id 0x%02X = EMM Stream\n", PID, streams[PID]->stream_id);
              break;
            case PROGRAM_STREAM_DIRECTORY:
              printf("Found PID 0x%04X, stream id 0x%02X = Program Stream Directory\n", PID, streams[PID]->stream_id);
              break;
            case DSMCC_STREAM:
              printf("Found PID 0x%04X, stream id 0x%02X = DSMCC Stream\n", PID, streams[PID]->stream_id);
              break;
            case ITUTRECH222TYPEE_STREAM:
              printf("Found PID 0x%04X, stream id 0x%02X = ITU-T Rec. H.222.1 type E\n", PID, streams[PID]->stream_id);
              break;
            case PADDING_STREAM:
              printf("Found PID 0x%04X, stream id 0x%02X = Padding Stream\n", PID, streams[PID]->stream_id);
              break;
            default:
              if ((streams[PID]->stream_id >= 0xE0) && (streams[PID]->stream_id <= 0xEF))
                printf("Found PID 0x%04X, stream id 0x%02X = Video Stream %d\n", PID, streams[PID]->stream_id, streams[PID]->stream_id - 0xE0);
              else
                if ((streams[PID]->stream_id >= 0xC0) && (streams[PID]->stream_id <= 0xDF))
                  printf("Found PID 0x%04X, stream id 0x%02X = MPEG Audio Stream %d\n", PID, streams[PID]->stream_id, streams[PID]->stream_id - 0xC0);
                else
                {
                  if (!streams[PID]->stream_id)
                  {
                    switch (PID)
                    {
                      case PROGRAM_ASSOCIATION_TABLE:
                        printf("Found PID 0x%04X, Program Association Table Stream\n", PID);
                        break;
                      case CONDITIONAL_ACCESS_TABLE:
                        printf("Found PID 0x%04X, Conditional Access Table Stream\n", PID);
                        break;
                      case NULL_PACKET:
                        printf("Found PID 0x%04X, Null Packet Stream\n", PID);
                        break;
                      default:
                        printf("Found PID 0x%04X, Other Stream\n", PID);
                        break;
                    }
                  }
                  else
                    printf("Found PID 0x%04X, stream id 0x%02X = other stream\n", PID, streams[PID]->stream_id);
                }
          }
          streams[PID]->stream_found = 1;

          if ((streams[PID]->stream_id == PRIVATE_STREAM_1) && (vob_flag))
          {
            if (streams[PID]->stream_encrypted)
              printf("PID 0x%04X, Stream id 0x%02X, substream 0x%02X is encrypted\n", PID, streams[PID]->stream_id, streams[PID]->substream_id);
          }
          else
          {
            if (streams[PID]->stream_encrypted)
              printf("PID 0x%04X, Stream id 0x%02X is encrypted\n", PID, streams[PID]->stream_id);
          }
        }
      }
    }
  } while (!end_bs());

  printf("\nSummary:\n");
  printf("\nMPEG Transport Packets = %d\n", transport_packets);
  for (i = 0; i < 8192; i++)
  {
    if (streams[i] != NULL)
    {
      if (streams[i]->stream_packets)
      {
        switch (streams[i]->stream_id)
        {
          case PROGRAM_STREAM_MAP:
            printf("PID 0x%04X, Program Stream Map packets = %u, total bytes = %u\n", i, streams[i]->stream_packets, streams[i]->stream_bytes);
            break;
          case PRIVATE_STREAM_1:
            printf("PID 0x%04X, Private Stream 1 packets = %u, total bytes = %u\n", i, streams[i]->stream_packets, streams[i]->stream_bytes);
            if (vob_flag)
            {
              j = streams[i]->substream_id;
              if ((j >= SUBSTREAM_AC3_0) && (j <= SUBSTREAM_AC3_8))
                printf("    AC3 Audio stream %u packets = %u, total bytes = %u\n", j - SUBSTREAM_AC3_0, streams[i]->stream_packets, streams[i]->ac3_bytes);
              else
                if ((j >= SUBSTREAM_DTS_0) && (j <= SUBSTREAM_DTS_8))
                  printf("    DTS Audio stream %u packets = %u, total bytes = %u\n", j - SUBSTREAM_DTS_0, streams[i]->stream_packets, streams[i]->dts_bytes);
                else
                  if ((j >= SUBSTREAM_PCM_0) && (j <= SUBSTREAM_PCM_F))
                    printf("    PCM Audio stream %u packets = %u, total bytes = %u\n", j - SUBSTREAM_PCM_0, streams[i]->stream_packets, streams[i]->pcm_bytes);
                  else
                    if ((j >= SUBSTREAM_SUBPIC_0) && (j <= SUBSTREAM_SUBPIC_1F))
                      printf("    Subpicture stream %u packets = %u, total bytes = %u\n", j - SUBSTREAM_SUBPIC_0, streams[i]->stream_packets, streams[i]->subpic_bytes);
                    else
                      printf("    Substream 0x%02X packets = %u, total bytes = %u\n", j, streams[i]->stream_packets, streams[i]->stream_bytes);
            }
            break;
          case PRIVATE_STREAM_2:
            printf("PID 0x%04X, Private Stream 2 packets = %u, total bytes = %u\n", PID, streams[i]->stream_packets, streams[i]->stream_bytes);
            break;
          case ECM_STREAM:
            printf("PID 0x%04X, ECM Stream packets = %u, total bytes = %u\n", PID, streams[i]->stream_packets, streams[i]->stream_bytes);
            break;
          case EMM_STREAM:
            printf("PID 0x%04X, EMM Stream packets = %u, total bytes = %u\n", PID, streams[i]->stream_packets, streams[i]->stream_bytes);
            break;
          case PROGRAM_STREAM_DIRECTORY:
            printf("PID 0x%04X, Program Stream Directory packets = %u, total bytes = %u\n", PID, streams[i]->stream_packets, streams[i]->stream_bytes);
            break;
          case DSMCC_STREAM:
            printf("PID 0x%04X, DSMCC Stream packets = %u, total bytes = %u\n", PID, streams[i]->stream_packets, streams[i]->stream_bytes);
            break;
          case ITUTRECH222TYPEE_STREAM:
            printf("PID 0x%04X, ITU-T Rec. H.222.1 type E packets = %u, total bytes = %u\n", PID, streams[i]->stream_packets, streams[i]->stream_bytes);
            break;
          case PADDING_STREAM:
            printf("PID 0x%04X, Padding Stream packets = %u, total bytes = %u\n", PID, streams[i]->stream_packets, streams[i]->stream_bytes);
            break;
          default:
            j = streams[i]->stream_id;
            if ((j >= 0xE0) && (j <= 0xEF))
              printf("PID 0x%04X, Video stream %d packets = %u, total bytes = %u\n", i, j - 0xE0, streams[i]->stream_packets, streams[i]->stream_bytes);
            else
              if ((j >= 0xC0) && (j <= 0xDF))
                printf("PID 0x%04X, MPEG Audio stream %d packets = %u, total bytes = %u\n", i, j - 0xC0, streams[i]->stream_packets, streams[i]->stream_bytes);
              else
                if (!j)
                {
                  switch (i)
                  {
                    case PROGRAM_ASSOCIATION_TABLE:
                      printf("PID 0x%04X, Program Association Table packets = %u, total bytes = %u\n", i, streams[i]->stream_packets, streams[i]->stream_bytes);
                      break;
                    case CONDITIONAL_ACCESS_TABLE:
                      printf("PID 0x%04X, Conditional Access Table packets = %u, total bytes = %u\n", i, streams[i]->stream_packets, streams[i]->stream_bytes);
                      break;
                    case NULL_PACKET:
                      printf("PID 0x%04X, Null packets = %u, total bytes = %u\n", i, streams[i]->stream_packets, streams[i]->stream_bytes);
                      break;
                    default:
                      printf("PID 0x%04X, Other packets = %u, total bytes = %u\n", i, streams[i]->stream_packets, streams[i]->stream_bytes);
                      break;
                  }
                }
                else
                  printf("PID %04X, Other stream 0x%02X packets = %u, total bytes = %u\n", i, j, streams[i]->stream_packets, streams[i]->stream_bytes);
        }
      }
      free(streams[i]);
    }
  }
  finish_getbits();
  if (outfp)
  {
    flush_buffer();
    fclose(outfp);
  }
}
