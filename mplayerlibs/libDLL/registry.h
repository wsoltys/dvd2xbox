#ifndef AVIFILE_REGISTRY_H
#define AVIFILE_REGISTRY_H

/********************************************************
 *
 *       Declaration of registry access functions
 *       Copyright 2000 Eugene Kuznetsov  (divx@euro.ru)
 *
 ********************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif
#define REG_DWORD                   ( 4 )   // 32-bit number
#define HKEY_CLASSES_ROOT       ((HKEY) 0x80000000)
#define HKEY_CURRENT_USER       ((HKEY) 0x80000001)
#define HKEY_LOCAL_MACHINE      ((HKEY) 0x80000002)
#define HKEY_USERS              ((HKEY) 0x80000003)
#define HKEY_PERFORMANCE_DATA   ((HKEY) 0x80000004)
#define HKEY_CURRENT_CONFIG     ((HKEY) 0x80000005)
#define HKEY_DYN_DATA           ((HKEY) 0x80000006)
#define REG_CREATED_NEW_KEY	0x00000001

void free_registry(void);

long RegOpenKeyExA(long key, const char* subkey, long reserved,
		   long access, int* newkey);
long RegCloseKey(long key);
long RegQueryValueExA(long key, const char* value, int* reserved,
		      int* type, int* data, int* count);
long RegCreateKeyExA(long key, const char* name, long reserved,
		     void* classs, long options, long security,
		     void* sec_attr, int* newkey, int* status);
long RegSetValueExA(long key, const char* name, long v1, long v2,
		    const void* data, long size);

long RegEnumKeyExA(HKEY hKey, DWORD dwIndex, LPSTR lpName, LPDWORD lpcbName,
		   LPDWORD lpReserved, LPSTR lpClass, LPDWORD lpcbClass,
		   LPFILETIME lpftLastWriteTime);
long RegEnumValueA(HKEY hkey, DWORD index, LPSTR value, LPDWORD val_count,
		   LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count);

#ifdef __cplusplus
}
#endif

#endif // AVIFILE_REGISTRY_H
