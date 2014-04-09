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

#ifdef __cplusplus
}
#endif  /* __cplusplus */
