#if 1
#ifdef _EXPORTING
#define DLLEXPORT   __declspec( dllexport )
#else
#define DLLEXPORT   __declspec( dllimport )
#endif
#else
#define DLLEXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

DLLEXPORT BOOL WINAPI InitCan(void);
DLLEXPORT int WINAPI GetCanNo(void);
DLLEXPORT void WINAPI StringTest(CString*);
DLLEXPORT HANDLE WINAPI GetTerminalHnd(void);
DLLEXPORT POSITION WINAPI RegisterAcquire(CString slotName, unsigned int slotKey);
DLLEXPORT POSITION WINAPI RegisterAcquire01(CString slotName);
DLLEXPORT HANDLE WINAPI MailSlotAcquire(POSITION pos);
DLLEXPORT HANDLE WINAPI InforEventAcquire(POSITION pos);
DLLEXPORT void WINAPI DecRefCount(POSITION *p);
DLLEXPORT LPVOID WINAPI ReadRawList(POSITION *p);
DLLEXPORT LPVOID WINAPI ReadRawCanRaw(POSITION *p);
DLLEXPORT void WINAPI CloseCan(void);
DLLEXPORT void WINAPI DeregisterAcquire(POSITION pos, unsigned int slotKey);
DLLEXPORT void WINAPI DeregisterAcquire01(POSITION pos);
//DLLEXPORT void WINAPI DecMailslot(POSITION);
//DLLEXPORT const char* WINAPI GetDecMailslotName(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
