#pragma once


#define WM_CONFIG_UPDATE	(WM_USER + 1)
#define WM_CONFIG_GET_SEL	(WM_USER + 2)
#define WM_DISAPLY_MODE		(WM_USER + 3)


typedef enum {
	BUFFER_MODE = 0,
	IDENT_MODE
} GRID_MODE;

typedef enum {
	DISPLAY_MODE = 0x13FF,
	FILTER,
	GRID_SETTING
} LIST_CONTENT;

typedef enum {
	C_DISPLAY_MODE = 0x1333,
	C_FILTER_ACTIVE,
	C_FILTER_ID,
	C_FILTER_MODE,
	C_COMBO_UNDEF
} COMBOBOX_CONTENT;

typedef struct {
	CWnd *pParent;
	int item;
	int subitem;
	CRect rect;
	LIST_CONTENT conIdx;
	int showCurIdx;
} COMBODRAWINFO, *LPCOMBODRAWINFO;
