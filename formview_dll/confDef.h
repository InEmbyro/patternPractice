#pragma once


#define WM_CONFIG_UPDATE	(WM_USER + 1)
#define WM_CONFIG_GET_SEL	(WM_USER + 2)
#define WM_DISPLAY_MODE		(WM_USER + 3)
#define WM_DISPLAY_ROWS		(WM_USER + 4)


typedef enum {
	BUFFER_MODE = 0,
	IDENT_MODE
} GRID_MODE;

typedef struct {
	GRID_MODE mode;
	unsigned int rows;
} DISPLAY_MODE_SET;

typedef enum {
	DISPLAY_MODE = 0x13FF,
	FILTER,
	GRID_SETTING
} LIST_CONTENT;

typedef enum {
	C_DISPLAY_MODE = 0x1333,
	C_DISPLAY_ROWS,
	C_FILTER_ACTIVE,
	C_FILTER_MODE,
	C_COMBO_UNDEF
} COMBOBOX_CONTENT, EDITBOX_CONTENT;

typedef struct {
	CWnd *pParent;
	int item;
	int subitem;
	CRect rect;
	LIST_CONTENT conIdx;
	int showCurIdx;
} EDITDRAWINFO, COMBODRAWINFO, *LPCOMBODRAWINFO, *LPEDITDRAWINFO;
