# dvd2xbox
dvd2xbox source code archive

### dvd2xbox is a disc to hdd/smb/ftp copy program for the original xbox which supports the following formats:

* XBox Games: makes a backup of your original games. You're done with a few clicks. Use game name as directory name. Disables all known media checks and let you run the game from any medium.
* Video DVD: makes a backup of your original movie DVD. Supports css to decrypt the movie during copy.
* Audio CD: makes a backup of your original CDDA. Rips Audiotracks and encodes them to ogg vorbis, MP3 or WAV. Supports freecddb to name your tracks.
* ISO9660: copy your burned S/VCD and mp3 CDs.

### Additional features:
* Action Control List: control the postprocessing (patching) of each title individual using the titleID.
* Disk Browser: browse you hdds/DVDs/CD-RW/CD-Rs, delete files or directories, launch apps directly within dvd2xbox, remove media checks from all *.xbe's,patch all files with adr style input data, copy files between partitions or from DVD/CD-RW/CD-R, see filesizes and stored xbe titles, rename xbe titles, start ACL processing manually.
* Game manager: List and launch your games with dvd2xbox. Supports deletion of game, gamesaves, game and gamesaves.
* FTP server: based on XBMC's xbfilezilla.
* Support for DVD/CD-RW/CD-R in UDF/ISO9660/CDDA format
* LCD support for SmartXX, X3 and Xenium chips
* Progress bar during file copy for the formats UDF/ISO9660/CDDA+encoding
* Progress bar for total bytes to copy for the formats UDF and ISO9660
* Display of remaining bytes to copy (UDF/DVD) and remaining free space (all types)
* Destination names are automatically changed to fit the fatx naming convention
* Support for partition7 (g: drive).
* Support for LBA48.
* Support for smb shares via included samba client.
* Support for xbox to xbox transfers (ftp).
* Copy report which displays files copied,files failed to copy and files renamed
* Retry mode to copy failed files without copy the whole disc again (udf)
* Logfile writer: enable it via the settings menu to see what happens during copy.
* Simple and intuitive gui.
* Clear configuration file using XML.
* Reliable

### Thanks:
* XBMC for the great work they've done. I use a lot of their sources in dvd2xbox
* Piet for the nice background picture and the Xcddb library for using freecddb.org
* Jezz_X for all the nice skins
    
    
 ### Note:
 This program works only on microsofts first xbox the so called original xbox. To compile the code you need the official xbox developer kit xdk.
