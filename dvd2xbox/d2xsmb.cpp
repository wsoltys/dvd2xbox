#include "D2Xsmb.h"
#include <stdstring.h>
#include <xtl.h>


char szPassWd[128];
char szUserName[128];
class MyCallback : public SmbAnswerCallback
{
protected:
	// Warning: don't use a fixed size buffer in a real application.
	// This is a security hazard.
	char buf[200];
public:
	char *getAnswer(int type, const char *optmessage) {
		switch (type) {
			case ANSWER_USER_NAME:
				strcpy(buf, szUserName);
				break;
			case ANSWER_USER_PASSWORD:
//				cout<<"Password for user "<<optmessage<<": ";
//				cin>>buf;
				strcpy(buf, szPassWd);
				break;
			case ANSWER_SERVICE_PASSWORD:
				strcpy(buf, szPassWd);
				break;
		}
		return buf;
	}
} cb;

D2Xsmb::D2Xsmb()
{
	smb = new SMB();
	smbfd = 0;
}

D2Xsmb::~D2Xsmb()
{
	delete smb;
}

HANDLE D2Xsmb::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{
	if(smbfd != 0)
	{
		smb->closedir(smbfd);
		smbfd = 0;
	}

	char* pPasswd=strstr(&lpFileName[4],":");
	if (pPasswd)
	{
		int i=0;
		pPasswd++;
		while (pPasswd[i] && pPasswd[i] != '@') i++;
		strncpy(szPassWd,pPasswd,i);
		szPassWd[i]=0;
	}
	else strcpy(szPassWd,"");
	
	smb->setNBNSAddress("192.168.1.1");
	smb->setPasswordCallback(&cb);
	smbfd = smb->opendir(lpFileName);
	if (smbfd < 0) 
	{
		DPf_H("Could not open smb dir %s",lpFileName);
		return INVALID_HANDLE_VALUE;
	}
	DPf_H("open smb dir");
	dirEnt = smb->readdir(smbfd);
	if (dirEnt->st_mode & S_IFDIR) lpFindFileData->dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
	else lpFindFileData->dwFileAttributes =  FILE_ATTRIBUTE_NORMAL;
	if (dirEnt->d_name && strcmp(dirEnt->d_name,".") && strcmp(dirEnt->d_name,".."))
	{
		DPf_H("found first entry %s",dirEnt->d_name);
		strcpy(lpFindFileData->cFileName,dirEnt->d_name);
		return NULL;
	} else 
	{
		DPf_H("No entry found");
		return INVALID_HANDLE_VALUE;
	}
}

BOOL D2Xsmb::FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)
{
	dirEnt = smb->readdir(smbfd);
	if (dirEnt->st_mode & S_IFDIR) lpFindFileData->dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
	else lpFindFileData->dwFileAttributes =  FILE_ATTRIBUTE_NORMAL;
	if (dirEnt->d_name && strcmp(dirEnt->d_name,".") && strcmp(dirEnt->d_name,".."))
	{
		DPf_H("Found entry %s",dirEnt->d_name);
		strcpy(lpFindFileData->cFileName,dirEnt->d_name);
		return true;
	} else
	{
		DPf_H("No entry found");
		return false;
	}
}

BOOL D2Xsmb::FindClose(HANDLE hFindFile)
{
	hFindFile = NULL;
	smb->closedir(smbfd);
	smbfd = 0;
	return true;
}

void D2Xsmb::SetLogin(const char* strLogin, const char* strPassword)
{
	strcpy(szUserName,strLogin);
	strcpy(szPassWd,strPassword);
}