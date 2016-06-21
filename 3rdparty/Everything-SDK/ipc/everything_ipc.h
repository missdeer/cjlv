
// Everything IPC

#ifndef _EVERYTHING_IPC_H_
#define _EVERYTHING_IPC_H_

// C
#ifdef __cplusplus
extern "C" {
#endif

#define EVERYTHING_WM_IPC												(WM_USER)

// EVERYTHING_WM_IPC (send to the taskbar notification window)
// HWND everything_hwnd = FindWindow(EVERYTHING_IPC_WNDCLASS,0);
// SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_*,lParam)
// version format: major.minor.revision.build 
// example: 1.1.4.309
#define EVERYTHING_IPC_GET_MAJOR_VERSION								0 // int major_version = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_GET_MAJOR_VERSION,0);
#define EVERYTHING_IPC_GET_MINOR_VERSION								1 // int minor_version = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_GET_MINOR_VERSION,0);
#define EVERYTHING_IPC_GET_REVISION										2 // int revision = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_GET_REVISION,0);
#define EVERYTHING_IPC_GET_BUILD_NUMBER									3 // int build = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_GET_BUILD,0);
#define EVERYTHING_IPC_EXIT												4 // returns 1 if the program closes.

// uninstall options
#define EVERYTHING_IPC_DELETE_START_MENU_SHORTCUTS						100 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_DELETE_START_MENU_SHORTCUTS,0);
#define EVERYTHING_IPC_DELETE_QUICK_LAUNCH_SHORTCUT						101 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_DELETE_QUICK_LAUNCH_SHORTCUT,0);
#define EVERYTHING_IPC_DELETE_DESKTOP_SHORTCUT							102 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_DELETE_DESKTOP_SHORTCUT,0);
#define EVERYTHING_IPC_DELETE_FOLDER_CONTEXT_MENU						103 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_DELETE_FOLDER_CONTEXT_MENU,0);
#define EVERYTHING_IPC_DELETE_RUN_ON_SYSTEM_STARTUP						104 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_DELETE_RUN_ON_SYSTEM_STARTUP,0);
#define EVERYTHING_IPC_DELETE_URL_PROTOCOL								105 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_DELETE_URL_PROTOCOL,0);

// install options
#define EVERYTHING_IPC_CREATE_START_MENU_SHORTCUTS						200 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_CREATE_START_MENU_SHORTCUTS,0);
#define EVERYTHING_IPC_CREATE_QUICK_LAUNCH_SHORTCUT						201 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_CREATE_QUICK_LAUNCH_SHORTCUT,0);
#define EVERYTHING_IPC_CREATE_DESKTOP_SHORTCUT							202 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_CREATE_DESKTOP_SHORTCUT,0);
#define EVERYTHING_IPC_CREATE_FOLDER_CONTEXT_MENU						203 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_CREATE_FOLDER_CONTEXT_MENU,0);
#define EVERYTHING_IPC_CREATE_RUN_ON_SYSTEM_STARTUP						204 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_CREATE_RUN_ON_SYSTEM_STARTUP,0);
#define EVERYTHING_IPC_CREATE_URL_PROTOCOL								205 // SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_CREATE_URL_PROTOCOL,0);

// get option status; 0 = no, 1 = yes, 2 = indeterminate (partially installed)
#define EVERYTHING_IPC_IS_START_MENU_SHORTCUTS							300 // int ret = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_IS_START_MENU_SHORTCUTS,0);
#define EVERYTHING_IPC_IS_QUICK_LAUNCH_SHORTCUT							301 // int ret = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_IS_QUICK_LAUNCH_SHORTCUT,0);
#define EVERYTHING_IPC_IS_DESKTOP_SHORTCUT								302 // int ret = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_IS_DESKTOP_SHORTCUT,0);
#define EVERYTHING_IPC_IS_FOLDER_CONTEXT_MENU							303 // int ret = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_IS_FOLDER_CONTEXT_MENU,0);
#define EVERYTHING_IPC_IS_RUN_ON_SYSTEM_STARTUP							304 // int ret = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_IS_RUN_ON_SYSTEM_STARTUP,0);
#define EVERYTHING_IPC_IS_URL_PROTOCOL									305 // int ret = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_IS_URL_PROTOCOL,0);
#define EVERYTHING_IPC_IS_SERVICE										306 // int ret = (int)SendMessage(everything_hwnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_IS_SERVICE,0);

// indexing
#define EVERYTHING_IPC_IS_NTFS_DRIVE_INDEXED							400 // int is_indexed = (int)SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_IS_NTFS_DRIVE_INDEXED,drive_index); drive_index: 0-25 = 0=A:, 1=B:, 2=C:...

// requires Everything 1.4:
#define EVERYTHING_IPC_IS_DB_LOADED										401 // int is_db_loaded = (int)SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_IS_DB_LOADED,0); 
#define EVERYTHING_IPC_IS_DB_BUSY										402 // int is_db_busy = (int)SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_IS_DB_BUSY,0); // db is busy, issueing another action will cancel the current one (if possible).
#define EVERYTHING_IPC_IS_ADMIN											403 // int is_admin = (int)SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_IS_ADMIN,0);
#define EVERYTHING_IPC_IS_APPDATA										404 // int is_appdata = (int)SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_IS_APPDATA,0);
#define EVERYTHING_IPC_REBUILD_DB										405 // SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_REBUILD,0); // forces all indexes to be rescanned.
#define EVERYTHING_IPC_UPDATE_ALL_FOLDER_INDEXES						406 // SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_UPDATE_ALL_FOLDER_INDEXES,0); // rescan all folder indexes.
#define EVERYTHING_IPC_SAVE_DB											407 // SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_SAVE_DB,0); // save the db to disk.
#define EVERYTHING_IPC_SAVE_RUN_HISTORY									408 // SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_SAVE_RUN_HISTORY,0); // save run history to disk.
#define EVERYTHING_IPC_DELETE_RUN_HISTORY								409 // SendMessage(everything_hwnd,WM_USER,EVERYTHING_IPC_DELETE_RUN_HISTORY,0); // deletes all run history from memory and disk.

// main menus
#define	EVERYTHING_IPC_ID_FILE_MENU										10001
#define	EVERYTHING_IPC_ID_EDIT_MENU										10002
#define	EVERYTHING_IPC_ID_SEARCH_MENU									10003
#define	EVERYTHING_IPC_ID_TOOLS_MENU									10004
#define	EVERYTHING_IPC_ID_HELP_MENU										10005
#define	EVERYTHING_IPC_ID_TOOLBAR										10006
#define	EVERYTHING_IPC_ID_SEARCH_EDIT									10007
#define	EVERYTHING_IPC_ID_FILTER										10008
#define	EVERYTHING_IPC_ID_RESULTS_HEADER								10009
#define	EVERYTHING_IPC_ID_STATUS										10010
#define EVERYTHING_IPC_ID_VIEW_ZOOM_MENU								10012
#define	EVERYTHING_IPC_ID_VIEW_MENU										10013
#define EVERYTHING_IPC_ID_VIEW_WINDOW_SIZE_MENU							10019
#define EVERYTHING_IPC_ID_RESULT_LIST									10020
#define EVERYTHING_IPC_ID_BOOKMARKS_MENU								10021
#define EVERYTHING_IPC_ID_VIEW_SORT_BY_MENU								10022
#define EVERYTHING_IPC_ID_VIEW_GOTO_MENU								10024
#define EVERYTHING_IPC_ID_VIEW_ONTOP_MENU								10025
#define EVERYTHING_IPC_ID_PREVIEW										10026

// TRAY 
#define EVERYTHING_IPC_ID_TRAY_NEW_SEARCH_WINDOW						40001
#define EVERYTHING_IPC_ID_TRAY_CONNECT_TO_ETP_SERVER					40004
#define EVERYTHING_IPC_ID_TRAY_OPTIONS									40005
#define EVERYTHING_IPC_ID_TRAY_EXIT										40006
#define EVERYTHING_IPC_ID_TRAY_SHOW_SEARCH_WINDOW						40007
#define EVERYTHING_IPC_ID_TRAY_TOGGLE_SEARCH_WINDOW						40008

// FILE
#define EVERYTHING_IPC_ID_FILE_NEW_WINDOW								40010 
#define EVERYTHING_IPC_ID_FILE_CLOSE									40011 
#define EVERYTHING_IPC_ID_FILE_EXPORT									40012 
#define EVERYTHING_IPC_ID_FILE_EXIT										40013
#define EVERYTHING_IPC_ID_FILE_OPEN_FILELIST							40014
#define EVERYTHING_IPC_ID_FILE_CLOSE_FILELIST							40015

// EDIT
#define EVERYTHING_IPC_ID_EDIT_CUT										40020 
#define EVERYTHING_IPC_ID_EDIT_COPY										40021
#define EVERYTHING_IPC_ID_EDIT_PASTE									40022
#define EVERYTHING_IPC_ID_EDIT_SELECT_ALL								40023
#define EVERYTHING_IPC_ID_EDIT_INVERT_SELECTION							40029

// VIEW
#define EVERYTHING_IPC_ID_VIEW_ZOOM_IN									40030
#define EVERYTHING_IPC_ID_VIEW_ZOOM_OUT									40031
#define EVERYTHING_IPC_ID_VIEW_ZOOM_RESET								40032
#define EVERYTHING_IPC_ID_VIEW_TOGGLE_FULLSCREEN						40034
#define EVERYTHING_IPC_ID_VIEW_AUTO_FIT									40044
#define EVERYTHING_IPC_ID_VIEW_AUTO_SIZE_1								40045
#define EVERYTHING_IPC_ID_VIEW_AUTO_SIZE_2								40046
#define EVERYTHING_IPC_ID_VIEW_AUTO_SIZE_3								40047
#define EVERYTHING_IPC_ID_VIEW_REFRESH									40036
#define EVERYTHING_IPC_ID_VIEW_FILTERS									40035
#define EVERYTHING_IPC_ID_VIEW_SORT_BY_ASCENDING						40037
#define EVERYTHING_IPC_ID_VIEW_SORT_BY_DESCENDING						40038
#define EVERYTHING_IPC_ID_VIEW_STATUS_BAR								40039
#define EVERYTHING_IPC_ID_VIEW_GOTO_BACK								40040
#define EVERYTHING_IPC_ID_VIEW_GOTO_FORWARD								40041
#define EVERYTHING_IPC_ID_VIEW_ONTOP_NEVER								40042
#define EVERYTHING_IPC_ID_VIEW_ONTOP_ALWAYS								40043
#define EVERYTHING_IPC_ID_VIEW_ONTOP_WHILE_SEARCHING					40048
#define EVERYTHING_IPC_ID_VIEW_GOTO_HOME								40049
#define EVERYTHING_IPC_ID_VIEW_TOGGLE_LTR_RTL							40050
#define EVERYTHING_IPC_ID_VIEW_DETAILS									40051
#define EVERYTHING_IPC_ID_VIEW_MEDIUM_ICONS								40052
#define EVERYTHING_IPC_ID_VIEW_LARGE_ICONS								40053
#define EVERYTHING_IPC_ID_VIEW_EXTRA_LARGE_ICONS						40054
#define EVERYTHING_IPC_ID_VIEW_PREVIEW									40055
#define EVERYTHING_IPC_ID_VIEW_GOTO_SHOW_ALL_HISTORY					40056
#define EVERYTHING_IPC_ID_VIEW_INCREASE_THUMBNAIL_SIZE					40057
#define EVERYTHING_IPC_ID_VIEW_DECREASE_THUMBNAIL_SIZE					40058

// SEARCH
#define	EVERYTHING_IPC_ID_SEARCH_TOGGLE_MATCH_CASE						40060
#define EVERYTHING_IPC_ID_SEARCH_TOGGLE_MATCH_WHOLE_WORD				40061
#define EVERYTHING_IPC_ID_SEARCH_TOGGLE_MATCH_PATH						40062
#define EVERYTHING_IPC_ID_SEARCH_TOGGLE_REGEX							40063
#define	EVERYTHING_IPC_ID_SEARCH_TOGGLE_MATCH_DIACRITICS				40066
#define EVERYTHING_IPC_ID_SEARCH_FILTER_ADD								40067
#define EVERYTHING_IPC_ID_SEARCH_FILTER_ORGANIZE						40068
#define EVERYTHING_IPC_ID_SEARCH_ADVANCED_SEARCH						40069

// TOOLS
#define EVERYTHING_IPC_ID_TOOLS_CONNECT_TO_ETP_SERVER					40072
#define EVERYTHING_IPC_ID_TOOLS_DISCONNECT_FROM_ETP_SERVER				40073
#define EVERYTHING_IPC_ID_TOOLS_OPTIONS									40074
#define EVERYTHING_IPC_ID_TOOLS_CONSOLE									40075
#define EVERYTHING_IPC_ID_TOOLS_EDITOR									40076

// HELP
#define EVERYTHING_IPC_ID_HELP_VIEW_HELP_TOPICS							40080
#define EVERYTHING_IPC_ID_HELP_OPEN_EVERYTHING_WEBSITE					40081
#define EVERYTHING_IPC_ID_HELP_CHECK_FOR_UPDATES						40082
#define EVERYTHING_IPC_ID_HELP_ABOUT_EVERYTHING							40083
#define EVERYTHING_IPC_ID_HELP_SEARCH_SYNTAX							40084
#define EVERYTHING_IPC_ID_HELP_COMMAND_LINE_OPTIONS						40085
#define EVERYTHING_IPC_ID_HELP_REGEX_SYNTAX								40086
#define EVERYTHING_IPC_ID_HELP_DONATE									40087

// bookmarks
#define EVERYTHING_IPC_ID_BOOKMARK_ADD									40090
#define EVERYTHING_IPC_ID_BOOKMARK_ORGANIZE								40091
#define EVERYTHING_IPC_ID_BOOKMARK_START								44000
#define EVERYTHING_IPC_ID_BOOKMARK_END									45000 // exclusive

#define EVERYTHING_IPC_ID_FILTER_START									45000
#define EVERYTHING_IPC_ID_FILTER_END									46000 // exclusive

#define EVERYTHING_IPC_ID_VIEW_GOTO_START								46000
#define EVERYTHING_IPC_ID_VIEW_GOTO_END									47000 // exclusive

// files
#define EVERYTHING_IPC_ID_FILE_OPEN										41000
#define EVERYTHING_IPC_ID_FILE_OPEN_NEW									41048
#define EVERYTHING_IPC_ID_FILE_OPEN_WITH								41049
#define EVERYTHING_IPC_ID_FILE_EDIT										41050
#define EVERYTHING_IPC_ID_FILE_PLAY										41051
#define EVERYTHING_IPC_ID_FILE_PRINT									41052
#define EVERYTHING_IPC_ID_FILE_PREVIEW									41053
#define EVERYTHING_IPC_ID_FILE_PRINT_TO									41054
#define EVERYTHING_IPC_ID_FILE_RUN_AS									41055
#define EVERYTHING_IPC_ID_FILE_OPEN_WITH_DEFAULT_VERB					41056
#define EVERYTHING_IPC_ID_FILE_OPEN_AND_CLOSE							41057
#define EVERYTHING_IPC_ID_FILE_EXPLORE_PATH								41002
#define EVERYTHING_IPC_ID_FILE_OPEN_PATH								41003
#define EVERYTHING_IPC_ID_FILE_DELETE									41004
#define EVERYTHING_IPC_ID_FILE_PERMANENTLY_DELETE						41005
#define EVERYTHING_IPC_ID_FILE_RENAME									41006
#define EVERYTHING_IPC_ID_FILE_COPY_FULL_PATH_AND_NAME					41007
#define EVERYTHING_IPC_ID_FILE_COPY_PATH								41008
#define EVERYTHING_IPC_ID_FILE_PROPERTIES								41009
#define EVERYTHING_IPC_ID_FILE_READ_EXTENDED_INFORMATION				41064
#define EVERYTHING_IPC_ID_FILE_CREATE_SHORTCUT							41065
#define EVERYTHING_IPC_ID_FILE_SET_RUN_COUNT							41068
#define EVERYTHING_IPC_ID_FILE_COPY_NAME								41011
#define EVERYTHING_IPC_ID_FILE_OPEN_AND_DO_NOT_CLOSE					41076

// result list
#define EVERYTHING_IPC_ID_RESULT_LIST_EXPLORE							41001
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS								41010
#define EVERYTHING_IPC_ID_RESULT_LIST_AUTOFIT_COLUMNS					41012
#define EVERYTHING_IPC_ID_RESULT_LIST_DOWN								41018
#define EVERYTHING_IPC_ID_RESULT_LIST_UP								41019
#define EVERYTHING_IPC_ID_RESULT_LIST_PAGE_UP							41020
#define EVERYTHING_IPC_ID_RESULT_LIST_PAGE_DOWN							41021
#define EVERYTHING_IPC_ID_RESULT_LIST_START								41022
#define EVERYTHING_IPC_ID_RESULT_LIST_END								41023
#define EVERYTHING_IPC_ID_RESULT_LIST_DOWN_EXTEND						41024
#define EVERYTHING_IPC_ID_RESULT_LIST_UP_EXTEND							41025
#define EVERYTHING_IPC_ID_RESULT_LIST_PAGE_UP_EXTEND					41026
#define EVERYTHING_IPC_ID_RESULT_LIST_PAGE_DOWN_EXTEND					41027
#define EVERYTHING_IPC_ID_RESULT_LIST_START_EXTEND						41028
#define EVERYTHING_IPC_ID_RESULT_LIST_END_EXTEND						41029
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_DOWN						41030
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_UP							41031
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_PAGE_UP						41032
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_PAGE_DOWN					41033
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_START						41034
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_END							41035
#define EVERYTHING_IPC_ID_RESULT_LIST_SCROLL_LEFT						41036
#define EVERYTHING_IPC_ID_RESULT_LIST_SCROLL_RIGHT						41037
#define EVERYTHING_IPC_ID_RESULT_LIST_SCROLL_PAGE_LEFT					41038
#define EVERYTHING_IPC_ID_RESULT_LIST_SCROLL_PAGE_RIGHT					41039
#define EVERYTHING_IPC_ID_RESULT_LIST_SELECT_FOCUS						41040
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_FOCUS_SELECTION			41041
#define EVERYTHING_IPC_ID_RESULT_LIST_CONTEXT_MENU						41046
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_DOWN_EXTEND					41058
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_UP_EXTEND					41059
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_PAGE_UP_EXTEND				41060
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_PAGE_DOWN_EXTEND			41061
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_START_EXTEND				41062
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_END_EXTEND					41063
#define EVERYTHING_IPC_ID_RESULT_LIST_AUTOFIT							41066
#define EVERYTHING_IPC_ID_RESULT_LIST_COPY_CSV							41067
#define EVERYTHING_IPC_ID_RESULT_LIST_LEFT_EXTEND						41070
#define EVERYTHING_IPC_ID_RESULT_LIST_RIGHT_EXTEND						41071
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_LEFT_EXTEND					41072
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_RIGHT_EXTEND				41073
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_MOST_RUN					41074
#define EVERYTHING_IPC_ID_RESULT_LIST_FOCUS_LAST_RUN					41075

#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_NAME						41300
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_PATH						41301
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_SIZE						41302
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_EXTENSION					41303
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_TYPE						41304
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_DATE_MODIFIED				41305
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_DATE_CREATED				41306
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_ATTRIBUTES				41307
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_FILE_LIST_FILENAME		41308
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_RUN_COUNT					41309
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_DATE_RECENTLY_CHANGED		41310
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_DATE_ACCESSED				41311
#define EVERYTHING_IPC_ID_RESULT_LIST_SORT_BY_DATE_RUN					41312

#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_NAME_COLUMN				41400
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_PATH_COLUMN				41401
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_SIZE_COLUMN				41402
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_EXTENSION_COLUMN			41403
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_TYPE_COLUMN				41404
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_DATE_MODIFIED_COLUMN		41405
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_DATE_CREATED_COLUMN		41406
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_ATTRIBUTES_COLUMN			41407
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_FILE_LIST_FILENAME_COLUMN	41408
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_RUN_COUNT_COLUMN			41409
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_DATE_RECENTLY_CHANGED_COLUMN	41410
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_DATE_ACCESSED_COLUMN		41411
#define EVERYTHING_IPC_ID_RESULT_LIST_TOGGLE_DATE_RUN_COLUMN			41412

#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_NAME_COLUMN_TO_FIT			41600
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_PATH_COLUMN_TO_FIT			41601
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_SIZE_COLUMN_TO_FIT			41602
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_EXTENSION_COLUMN_TO_FIT		41603
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_TYPE_COLUMN_TO_FIT			41604
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_DATE_MODIFIED_COLUMN_TO_FIT	41605
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_DATE_CREATED_COLUMN_TO_FIT	41606
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_ATTRIBUTES_COLUMN_TO_FIT		41607
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_FILE_LIST_FILENAME_COLUMN_TO_FIT	41608
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_RUN_COUNT_COLUMN_TO_FIT		41609
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_DATE_RECENTLY_CHANGED_COLUMN_TO_FIT	41610
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_DATE_ACCESSED_COLUMN_TO_FIT	41611
#define EVERYTHING_IPC_ID_RESULT_LIST_SIZE_DATE_RUN_COLUMN_TO_FIT		41612

#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB01							41500
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB02							41501
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB03							41502
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB04							41503
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB05							41504
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB06							41505
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB07							41506
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB08							41507
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB09							41508
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB10							41509
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB11							41510
#define EVERYTHING_IPC_ID_FILE_CUSTOM_VERB12							41511

// search
#define EVERYTHING_IPC_ID_SEARCH_EDIT_FOCUS								42000
#define EVERYTHING_IPC_ID_SEARCH_EDIT_WORD_DELETE_TO_START				42019
#define	EVERYTHING_IPC_ID_SEARCH_EDIT_AUTO_COMPLETE						42020
#define EVERYTHING_IPC_ID_SEARCH_EDIT_SHOW_SEARCH_HISTORY				42021
#define EVERYTHING_IPC_ID_SEARCH_EDIT_SHOW_ALL_SEARCH_HISTORY			42022

#define EVERYTHING_IPC_ID_TRAY_EDITOR									41700
#define EVERYTHING_IPC_ID_TRAY_OPEN_FILELIST							41701

#define EVERYTHING_IPC_ID_INDEX_UPDATE_ALL_FOLDERS_NOW					41800
#define EVERYTHING_IPC_ID_INDEX_FORCE_REBUILD							41801

// find the everything IPC window
#define EVERYTHING_IPC_WNDCLASSW										L"EVERYTHING_TASKBAR_NOTIFICATION"
#define EVERYTHING_IPC_WNDCLASSA										"EVERYTHING_TASKBAR_NOTIFICATION"

// this global window message is sent to all top level windows when everything starts.
#define EVERYTHING_IPC_CREATEDW											L"EVERYTHING_IPC_CREATED"
#define EVERYTHING_IPC_CREATEDA											"EVERYTHING_IPC_CREATED"

// search flags for querys
#define EVERYTHING_IPC_MATCHCASE										0x00000001	// match case
#define EVERYTHING_IPC_MATCHWHOLEWORD									0x00000002	// match whole word
#define EVERYTHING_IPC_MATCHPATH										0x00000004	// include paths in search
#define EVERYTHING_IPC_REGEX											0x00000008	// enable regex
#define EVERYTHING_IPC_MATCHACCENTS										0x00000010	// match diacritic marks

// item flags
#define EVERYTHING_IPC_FOLDER											0x00000001	// The item is a folder. (its a file if not set)
#define EVERYTHING_IPC_DRIVE											0x00000002	// The folder is a drive. Path will be an empty string. 
													// (will also have the folder bit set)

typedef struct EVERYTHING_IPC_COMMAND_LINE
{
	DWORD show_command; // MUST be one of the SW_* ShowWindow() commands
	
	// null terminated variable sized command line text in UTF-8.
	unsigned char command_line_text[1];
	
}EVERYTHING_IPC_COMMAND_LINE;

// the WM_COPYDATA message for a query.
#define EVERYTHING_IPC_COPYDATA_COMMAND_LINE_UTF8						0  // Send a EVERYTHING_IPC_COMMAND_LINE structure.
#define EVERYTHING_IPC_COPYDATAQUERYA									1
#define EVERYTHING_IPC_COPYDATAQUERYW									2

// all results
#define EVERYTHING_IPC_ALLRESULTS										0xFFFFFFFF // all results

// macro to get the filename of an item
#define EVERYTHING_IPC_ITEMFILENAMEA(list,item) (CHAR *)((CHAR *)(list) + ((EVERYTHING_IPC_ITEMA *)(item))->filename_offset)
#define EVERYTHING_IPC_ITEMFILENAMEW(list,item) (WCHAR *)((CHAR *)(list) + ((EVERYTHING_IPC_ITEMW *)(item))->filename_offset)

// macro to get the path of an item
#define EVERYTHING_IPC_ITEMPATHA(list,item) (CHAR *)((CHAR *)(list) + ((EVERYTHING_IPC_ITEMW *)(item))->path_offset)
#define EVERYTHING_IPC_ITEMPATHW(list,item) (WCHAR *)((CHAR *)(list) + ((EVERYTHING_IPC_ITEMW *)(item))->path_offset)

#pragma pack (push,1)

//
// Varible sized query struct sent to everything.
//
// sent in the form of a WM_COPYDAYA message with EVERYTHING_IPC_COPYDATAQUERY as the 
// dwData member in the COPYDATASTRUCT struct.
// set the lpData member of the COPYDATASTRUCT struct to point to your EVERYTHING_IPC_QUERY struct.
// set the cbData member of the COPYDATASTRUCT struct to the size of the 
// EVERYTHING_IPC_QUERY struct minus the size of a TCHAR plus the length of the search string in bytes plus 
// one TCHAR for the null terminator.
//
// NOTE: to determine the size of this structure use 
// ASCII: sizeof(EVERYTHING_IPC_QUERYA) - sizeof(CHAR) + strlen(search_string)*sizeof(CHAR) + sizeof(CHAR)
// UNICODE: sizeof(EVERYTHING_IPC_QUERYW) - sizeof(WCHAR) + wcslen(search_string)*sizeof(WCHAR) + sizeof(WCHAR)
//
// NOTE: Everything will only do one query per window.
// Sending another query when a query has not completed 
// will cancel the old query and start the new one. 
//
// Everything will send the results to the reply_hwnd in the form of a 
// WM_COPYDAYA message with the dwData value you specify.
// 
// Everything will return TRUE if successful.
// returns FALSE if not supported.
//
// If you query with EVERYTHING_IPC_COPYDATAQUERYW, the results sent from Everything will be Unicode.
//

typedef struct EVERYTHING_IPC_QUERYW
{
	// the window that will receive the new results.
	// only 32bits are required to store a window handle. (even on x64)
	DWORD reply_hwnd;
	
	// the value to set the dwData member in the COPYDATASTRUCT struct 
	// sent by Everything when the query is complete.
	DWORD reply_copydata_message;
	
	// search flags (see EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHWHOLEWORD | EVERYTHING_IPC_MATCHPATH)
	DWORD search_flags; 
	
	// only return results after 'offset' results (0 to return from the first result)
	// useful for scrollable lists
	DWORD offset; 
	
	// the number of results to return 
	// zero to return no results
	// EVERYTHING_IPC_ALLRESULTS to return ALL results
	DWORD max_results;

	// null terminated string. variable lengthed search string buffer.
	WCHAR search_string[1];
	
}EVERYTHING_IPC_QUERYW;

// ASCII version
typedef struct EVERYTHING_IPC_QUERYA
{
	// the window that will receive the new results.
	// only 32bits are required to store a window handle. (even on x64)
	DWORD reply_hwnd;
	
	// the value to set the dwData member in the COPYDATASTRUCT struct 
	// sent by Everything when the query is complete.
	DWORD reply_copydata_message;
	
	// search flags (see EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHWHOLEWORD | EVERYTHING_IPC_MATCHPATH)
	DWORD search_flags; 
	
	// only return results after 'offset' results (0 to return from the first result)
	// useful for scrollable lists
	DWORD offset; 
	
	// the number of results to return 
	// zero to return no results
	// EVERYTHING_IPC_ALLRESULTS to return ALL results
	DWORD max_results;

	// null terminated string. variable lengthed search string buffer.
	CHAR search_string[1];
	
}EVERYTHING_IPC_QUERYA;

//
// Varible sized result list struct received from Everything.
//
// Sent in the form of a WM_COPYDATA message to the hwnd specifed in the 
// EVERYTHING_IPC_QUERY struct.
// the dwData member of the COPYDATASTRUCT struct will match the sent
// reply_copydata_message member in the EVERYTHING_IPC_QUERY struct.
// 
// make a copy of the data before returning.
//
// return TRUE if you processed the WM_COPYDATA message.
//

typedef struct EVERYTHING_IPC_ITEMW
{
	// item flags
	DWORD flags;

	// The offset of the filename from the beginning of the list structure.
	// (wchar_t *)((char *)everything_list + everythinglist->name_offset)
	DWORD filename_offset;

	// The offset of the filename from the beginning of the list structure.
	// (wchar_t *)((char *)everything_list + everythinglist->path_offset)
	DWORD path_offset;
	
}EVERYTHING_IPC_ITEMW;

typedef struct EVERYTHING_IPC_ITEMA
{
	// item flags
	DWORD flags;

	// The offset of the filename from the beginning of the list structure.
	// (char *)((char *)everything_list + everythinglist->name_offset)
	DWORD filename_offset;

	// The offset of the filename from the beginning of the list structure.
	// (char *)((char *)everything_list + everythinglist->path_offset)
	DWORD path_offset;
	
}EVERYTHING_IPC_ITEMA;

typedef struct EVERYTHING_IPC_LISTW
{
	// the total number of folders found.
	DWORD totfolders;
	
	// the total number of files found.
	DWORD totfiles;
	
	// totfolders + totfiles
	DWORD totitems;
	
	// the number of folders available.
	DWORD numfolders;
	
	// the number of files available.
	DWORD numfiles;
	
	// the number of items available.
	DWORD numitems;

	// index offset of the first result in the item list.
	DWORD offset;
	
	// variable lengthed item list. 
	// use numitems to determine the actual number of items available.
	EVERYTHING_IPC_ITEMW items[1];
	
}EVERYTHING_IPC_LISTW;

typedef struct EVERYTHING_IPC_LISTA
{
	// the total number of folders found.
	DWORD totfolders;
	
	// the total number of files found.
	DWORD totfiles;
	
	// totfolders + totfiles
	DWORD totitems;
	
	// the number of folders available.
	DWORD numfolders;
	
	// the number of files available.
	DWORD numfiles;
	
	// the number of items available.
	DWORD numitems;

	// index offset of the first result in the item list.
	DWORD offset;
	
	// variable lengthed item list. 
	// use numitems to determine the actual number of items available.
	EVERYTHING_IPC_ITEMA items[1];
	
}EVERYTHING_IPC_LISTA;

#pragma pack (pop)

#ifdef UNICODE
#define EVERYTHING_IPC_COPYDATAQUERY	EVERYTHING_IPC_COPYDATAQUERYW
#define EVERYTHING_IPC_ITEMFILENAME		EVERYTHING_IPC_ITEMFILENAMEW
#define EVERYTHING_IPC_ITEMPATH			EVERYTHING_IPC_ITEMPATHW
#define EVERYTHING_IPC_QUERY			EVERYTHING_IPC_QUERYW
#define EVERYTHING_IPC_ITEM				EVERYTHING_IPC_ITEMW
#define EVERYTHING_IPC_LIST				EVERYTHING_IPC_LISTW
#define EVERYTHING_IPC_WNDCLASS			EVERYTHING_IPC_WNDCLASSW
#define EVERYTHING_IPC_CREATED			EVERYTHING_IPC_CREATEDW
#else
#define EVERYTHING_IPC_COPYDATAQUERY	EVERYTHING_IPC_COPYDATAQUERYA
#define EVERYTHING_IPC_ITEMFILENAME		EVERYTHING_IPC_ITEMFILENAMEA
#define EVERYTHING_IPC_ITEMPATH			EVERYTHING_IPC_ITEMPATHA
#define EVERYTHING_IPC_QUERY			EVERYTHING_IPC_QUERYA
#define EVERYTHING_IPC_ITEM				EVERYTHING_IPC_ITEMA
#define EVERYTHING_IPC_LIST				EVERYTHING_IPC_LISTA
#define EVERYTHING_IPC_WNDCLASS			EVERYTHING_IPC_WNDCLASSA
#define EVERYTHING_IPC_CREATED			EVERYTHING_IPC_CREATEDA
#endif

// IPC v2
// requires Everything 1.4

// notifications are sent to the registered notification window in EVERYTHING_IPC_WM_QUERY_CREATE.
// Everything calls: SendMessage(notify_hwnd,notify_message,EVERYTHING_IPC_QUERY_NOTIFY_*,0);
// notifications should be handled as fast as possible.
// Everything will block until the notification message is handled.
// avoid API calls that reply with WM_COPYDATA when handling notifications, they 
// will work, but it is tricky to determine the source of the reply.
// Everything will continue to process other API calls (eg EVERYTHING_IPC_WM_QUERY_GET_RESULT_COUNT 
// and EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO_CHANGE) when handling notifications.

#define EVERYTHING_IPC_NOTIFY_RESULTS_CHANGED							0 // results have changed, redraw all visible items.
#define EVERYTHING_IPC_NOTIFY_DB_CHANGED								1 // db has been changed, the current search has been lost, please execute a new search.
#define EVERYTHING_IPC_NOTIFY_STATUS_CHANGED							2 // Call EVERYTHING_IPC_WM_QUERY_GET_STATUS_TEXT to get the new status text.
#define EVERYTHING_IPC_NOTIFY_INVALID_FILE_LIST							3 
#define EVERYTHING_IPC_NOTIFY_FILE_LIST_NOT_FOUND						4
#define EVERYTHING_IPC_NOTIFY_FILE_INFO_CHANGED							5 // file information has changed. call EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO_CHANGE for each visible item to determine if it has changed.
#define EVERYTHING_IPC_NOTIFY_READY										6 // db was busy and is now ready.
#define EVERYTHING_IPC_NOTIFY_ACCESS_DENIED								7 // unable to index a ntfs volume because access was denied.

// create a new Everything query:
// Create your own window to recieve EVERYTHING_IPC_QUERY_NOTIFY_* notifications, this window handle will be your query handle.
// A window can only have one query associated with it.
// pass your notify window handle and a notify message of your choice (for example: WM_USER) to EVERYTHING_IPC_WM_CREATE_QUERY.
// The query is destroyed when the notify_hwnds thread terminates or when your call EVERYTHING_IPC_WM_QUERY_DESTROY.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_CREATE_QUERY,(WPARAM)(HWND)notify_hwnd,(LPARAM)(UINT)notify_message);
// returns nonzero if the query was created successfully, 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_CREATE									(WM_USER + 100)

// Destroy a query.
// (BOOL)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_DESTROY,(WPARAM)(HWND)notify_hwnd,0);
// returns nonzero if successful, 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_DESTROY									(WM_USER + 101)

// Set the reply WM_COPYDATA message.
// this is the dwData value that will be passed with WM_COPYDATA reply.
// By default this value is 0.
// all wm_copydata replys share the same message, use a global state variable to determine the source of a reply.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_COPYDATA_MESSAGE,(WPARAM)(HWND)notify_hwnd,(LPARAM)(DWORD)reply_copydata_message);
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE				(WM_USER + 102)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_CASE,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_case_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_CASE					(WM_USER + 103)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_WHOLE_WORD,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_whole_word_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_WHOLE_WORD				(WM_USER + 104)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_PATH,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_path_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_PATH					(WM_USER + 105)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_DIACRITICS,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_diacritics_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_DIACRITICS				(WM_USER + 106)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_REGEX,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_regex_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_MATCH_REGEX					(WM_USER + 107)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_CASE,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_case_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_CASE			(WM_USER + 108)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_WHOLE_WORD,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_whole_word_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_WHOLE_WORD		(WM_USER + 109)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_PATH,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_path_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_PATH			(WM_USER + 110)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_DIACRITICS,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_diacritics_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_DIACRITICS		(WM_USER + 111)

// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_REGEX,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_match_regex_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_FILTER_MATCH_REGEX			(WM_USER + 112)

// if the search is empty, a null query will be performed.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_HIDE_EMPTY_SEARCH_RESULTS,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_hide_empty_search_results_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_HIDE_EMPTY_SEARCH_RESULTS	(WM_USER + 113)

// clear focus and selection after executing the results.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_CLEAR_FOCUS_AND_SELECTION,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_clear_focus_and_selection_value); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_CLEAR_FOCUS_AND_SELECTION	(WM_USER + 114)

// pre sort the results 
// this is very expensive for slow sorts, use EVERYTHING_IPC_WM_QUERY_IS_FAST_SORT to determine if the sort will be fast.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_SORT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)sort_type); 
// returns nonzero if successful. returns 0 if not supported.
// sort_type can be one of EVERYTHING_IPC_SORT_* types.
// requires Everything 1.4

#define EVERYTHING_IPC_SORT_NAME_ASCENDING								1
#define EVERYTHING_IPC_SORT_NAME_DESCENDING								2
#define EVERYTHING_IPC_SORT_PATH_ASCENDING								3
#define EVERYTHING_IPC_SORT_PATH_DESCENDING								4
#define EVERYTHING_IPC_SORT_SIZE_ASCENDING								5
#define EVERYTHING_IPC_SORT_SIZE_DESCENDING								6
#define EVERYTHING_IPC_SORT_EXTENSION_ASCENDING							7
#define EVERYTHING_IPC_SORT_EXTENSION_DESCENDING						8
#define EVERYTHING_IPC_SORT_TYPE_NAME_ASCENDING							9
#define EVERYTHING_IPC_SORT_TYPE_NAME_DESCENDING						10
#define EVERYTHING_IPC_SORT_DATE_CREATED_ASCENDING						11
#define EVERYTHING_IPC_SORT_DATE_CREATED_DESCENDING						12
#define EVERYTHING_IPC_SORT_DATE_MODIFIED_ASCENDING						13
#define EVERYTHING_IPC_SORT_DATE_MODIFIED_DESCENDING					14
#define EVERYTHING_IPC_SORT_ATTRIBUTES_ASCENDING						15
#define EVERYTHING_IPC_SORT_ATTRIBUTES_DESCENDING						16
#define EVERYTHING_IPC_SORT_FILE_LIST_FILENAME_ASCENDING				17
#define EVERYTHING_IPC_SORT_FILE_LIST_FILENAME_DESCENDING				18
#define EVERYTHING_IPC_SORT_RUN_COUNT_ASCENDING							19
#define EVERYTHING_IPC_SORT_RUN_COUNT_DESCENDING						20
#define EVERYTHING_IPC_SORT_DATE_RECENTLY_CHANGED_ASCENDING				21
#define EVERYTHING_IPC_SORT_DATE_RECENTLY_CHANGED_DESCENDING			22
#define EVERYTHING_IPC_SORT_DATE_ACCESSED_ASCENDING						23
#define EVERYTHING_IPC_SORT_DATE_ACCESSED_DESCENDING					24
#define EVERYTHING_IPC_SORT_DATE_RUN_ASCENDING							25
#define EVERYTHING_IPC_SORT_DATE_RUN_DESCENDING							26
	
#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_SORT							(WM_USER + 115)

// enable selection and total size tracking.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SEARCH_TRACK_SELECTED_AND_TOTAL_SIZE,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)new_track_selected_and_total_size); 
// returns nonzero if successful. returns 0 if not supported or an invalid sort is passed.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SEARCH_TRACK_SELECTED_AND_TOTAL_SIZE	(WM_USER + 116)

// Set the search text
// COPYDATASTRUCT cds;
// cds.dwData = EVERYTHING_IPC_COPYDATA_SET_SEARCH_TEXT;
// cds.lpData = "ABC 123";
// cds.cbData = size in bytes of cds.lpData including null terminator.
// SendMessage(everything_hwnd,WM_COPYDATA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COPYDATASTRUCT *)&cds);

#define EVERYTHING_IPC_COPYDATA_QUERY_SET_SEARCH_TEXTA					3
#define EVERYTHING_IPC_COPYDATA_QUERY_SET_SEARCH_TEXTW					4

// the filter is an empty string by default.
// an empty string means no filter.
// COPYDATASTRUCT cds;
// cds.dwData = EVERYTHING_IPC_COPYDATA_SET_FILTER_TEXT;
// cds.lpData = "ext:mp3;jpg";
// cds.cbData = size in bytes of cds.lpData including null terminator.
// SendMessage(everything_hwnd,WM_COPYDATA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COPYDATASTRUCT *)&cds);

#define EVERYTHING_IPC_COPYDATA_QUERY_SET_SEARCH_FILTER_TEXTA			5
#define EVERYTHING_IPC_COPYDATA_QUERY_SET_SEARCH_FILTER_TEXTW			6

// Execute a query (set match case/path/wholeword/regex/search anytime before this call)
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SEARCH,(WPARAM)(HWND)notify_hwnd,0); 
// requires Everything 1.4
#define EVERYTHING_IPC_WM_QUERY_SEARCH									(WM_USER + 117)

// Sort the results after a query.
// EVERYTHING_IPC_NOTIFY_RESULTS_CHANGED is fired when sort is complete.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SORT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)sort_type); 
// returns nonzero if successful. returns 0 if not supported.
// sort_type can be one of EVERYTHING_IPC_SORT_* types.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SORT									(WM_USER + 118)

// Cancel an active search, sort or read extended data on selection, useful to cancel an action if the user presses ESC.
// calling EVERYTHING_IPC_WM_QUERY_SEARCH also cancels an active query.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_CANCEL,(WPARAM)(HWND)notify_hwnd,0); 
// returns nonzero if successful. returns 0 if not supported or there is no active query.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_CANCEL									(WM_USER + 119)

// Get the number of folder results.
// Does not work when sorted by date recently changed or run history.
// int folder_result_count = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_FOLDER_RESULT_COUNT,(WPARAM)(HWND)notify_hwnd,0); 
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_FOLDER_RESULT_COUNT					(WM_USER + 120)

// Get the number of file results.
// Does not work when sorted by date recently changed or run history.
// int file_result_count = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_FILE_RESULT_COUNT,(WPARAM)(HWND)notify_hwnd,0); 
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_FILE_RESULT_COUNT					(WM_USER + 121)

// Get the total number of results.
// int result_count = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_COUNT,(WPARAM)(HWND)notify_hwnd,0); 
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_COUNT						(WM_USER + 122)

// Get the name of a result.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_NAME,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_NAMEA						(WM_USER + 123)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_NAMEW						(WM_USER + 124)

// Get the path of a result.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_FULL_PATH,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_PATHA						(WM_USER + 125)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_PATHW						(WM_USER + 126)

// Get the full path and filename of a result.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_FULL_PATH_AND_NAME,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FULL_PATH_AND_NAMEA			(WM_USER + 127)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FULL_PATH_AND_NAMEW			(WM_USER + 128)

// Get the highlighted name of a result.
// Text inside *asterisks* is highlighted.
// Double asterisks escapes a single literal asterisk.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_NAME,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_NAMEA			(WM_USER + 129)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_NAMEW			(WM_USER + 130)

// Get the highlighted path of a result.
// Text inside *asterisks* is highlighted.
// Double asterisks escapes a single literal asterisk.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_PATH,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_PATHA			(WM_USER + 131)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_PATHW			(WM_USER + 132)

// Get the highlighted full path and filename of a result.
// Text inside *asterisks* is highlighted.
// Double asterisks escapes a single literal asterisk.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_FULL_PATH_AND_NAME,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_FULL_PATH_AND_NAMEA	(WM_USER + 133)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_HIGHLIGHTED_FULL_PATH_AND_NAMEW	(WM_USER + 134)

// Get the extension of a result (does not include the dot).
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_EXTENSION,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// returns nonzero if successful. returns 0 if not supported or the result is a folder.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_EXTENSIONA					(WM_USER + 135)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_EXTENSIONW					(WM_USER + 136)

// Get the file list file name of a results.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_LIST_FILE_NAME,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// returns nonzero if successful. returns 0 if not supported or the result is not in a file list.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_LIST_FILE_NAMEA			(WM_USER + 137)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_LIST_FILE_NAMEW			(WM_USER + 138)

// Get the number of times a result has been opened with "Everything".
// int run_count = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_RUN_COUNT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_RUN_COUNT					(WM_USER + 139)

// Get the date of the last time the result was run.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_DATE_RUN,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// returns nonzero if successful. returns 0 if not supported.
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a FILETIME structure. both low and high parts are 0xffffffff if the result has not been run.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_DATE_RUN						(WM_USER + 140)

// Get the date of the last time the result was changed.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_DATE_RECENTLY_CHANGED,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// returns nonzero if successful. returns 0 if not supported.
// index is the zero based result index of the filename to recieve.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a FILETIME structure. both low and high parts are 0xffffffff if the result has not been recently changed.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_DATE_RECENTLY_CHANGED		(WM_USER + 141)

// Determine if the result is a folder or a file.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_IS_FOLDER_RESULT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// returns nonzero if the result is a folder. returns 0 if not supported or the index is invalid or the result is a file.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_IS_FOLDER_RESULT						(WM_USER + 142)

// Determine if the result is a root.
// a root is a folder or file with no parent folder.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_IS_ROOT_RESULT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index of the filename to recieve.
// returns nonzero if the result is a root. returns 0 if not supported or the index is invalid or the result is not a root.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_IS_ROOT_RESULT							(WM_USER + 143)

// Set the starting offset of a find nearest string match call.
// set to -1 or 0 to start from the first item in the list.
// if the item is not found before the end of the results, the search continues from the start of the results.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_FIND_NEAREST_STRING_MATCH_START,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index.
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_FIND_NEAREST_STRING_MATCH_START		(WM_USER + 144)

// Find the nearest string match.
// COPYDATASTRUCT cds;
// cds.dwData = EVERYTHING_IPC_COPYDATA_FIND_NEAREST_STRING_MATCH;
// cds.lpData = TEXT("ABC 123");
// cds.cbData = size of cds.lpData in bytes including the null terminator.
// int index = (int)SendMessage(everything_hwnd,WM_COPYDATA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COPYDATASTRUCT *)&cds);
// returns the index of the first match, returns -1 if no match found.

#define EVERYTHING_IPC_COPYDATA_QUERY_FIND_NEAREST_STRING_MATCHA		7
#define EVERYTHING_IPC_COPYDATA_QUERY_FIND_NEAREST_STRING_MATCHW		8

// Set the first result index of the list of results for a call to request_file_info.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_REQUEST_FILE_INFO_START,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// index is the zero based result index.
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_REQUEST_FILE_INFO_START				(WM_USER + 145)

// Set the number of resultsto gather file info for in a call to request file info.
// Set this to the number of visible items in your list.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_REQUEST_FILE_INFO_COUNT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)count); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_REQUEST_FILE_INFO_COUNT				(WM_USER + 146)

// Requests thumbnails, icon, size and date information for a list of results specified in the calls EVERYTHING_IPC_WM_QUERY_SET_REQUEST_FILE_INFO_START and EVERYTHING_IPC_WM_QUERY_SET_REQUEST_FILE_INFO_COUNT.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_REQUEST_FILE_INFO,(WPARAM)(HWND)notify_hwnd,(LPARAM)(DWORD)file_info_flags); 
// file_info_flags can be zero or more of the following values:
// EVERYTHING_IPC_FILE_INFO_SIZE
// EVERYTHING_IPC_FILE_INFO_DATE_CREATED
// EVERYTHING_IPC_FILE_INFO_DATE_MODIFIED
// EVERYTHING_IPC_FILE_INFO_DATE_ACCESSED
// EVERYTHING_IPC_FILE_INFO_ATTRIBUTES
// EVERYTHING_IPC_FILE_INFO_ICON
// EVERYTHING_IPC_FILE_INFO_TYPE
// EVERYTHING_IPC_FILE_INFO_THUMBNAIL
// if the information is already indexed, the request does nothing.
// returns nonzero if successful. returns 0 if not supported.
// EVERYTHING_IPC_NOTIFY_FILE_INFO_CHANGED is fired when a request is complete.
// requires Everything 1.4

#define EVERYTHING_IPC_FILE_INFO_SIZE									0x0000001
#define EVERYTHING_IPC_FILE_INFO_DATE_CREATED							0x0000002
#define EVERYTHING_IPC_FILE_INFO_DATE_MODIFIED							0x0000004
#define EVERYTHING_IPC_FILE_INFO_DATE_ACCESSED							0x0000008
#define EVERYTHING_IPC_FILE_INFO_ATTRIBUTES								0x0000010
#define EVERYTHING_IPC_FILE_INFO_ICON									0x0000020 
#define EVERYTHING_IPC_FILE_INFO_TYPE									0x0000040 
#define EVERYTHING_IPC_FILE_INFO_THUMBNAIL								0x0000080 

#define EVERYTHING_IPC_WM_QUERY_REQUEST_FILE_INFO						(WM_USER + 147)

// Get the change flags for a result.
// can only be called in response to a EVERYTHING_IPC_NOTIFY_FILE_INFO_CHANGED notification.
// changed_flags = (DWORD)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO_CHANGE,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// returns zero or more of EVERYTHING_IPC_FILE_INFO_* flags, if a flag is set, that part should be invalidated.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO_CHANGE				(WM_USER + 148)

// Set the flags for the EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_FLAGS,(WPARAM)(HWND)notify_hwnd,(LPARAM)(DWORD)file_info_flags); 
// file_info_flags can be zero or more of EVERYTHING_IPC_FILE_INFO_* flags.
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_FLAGS			(WM_USER + 149)

// Set the background color for the EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_ICON_BACKGROUND_COLOR,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COLORREF)background_color); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_ICON_BACKGROUND_COLOR				(WM_USER + 150)

// Set the foreground (selected items) color for the EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_ICON_FOREGROUND_COLOR,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COLORREF)foreground_color); 
// foreground_color is a COLORREF.
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_ICON_FOREGROUND_COLOR				(WM_USER + 151)

// Set the how much the foreground color is blended with the icon. 0 = no blending, 255 = foreground color only, 128 = 50% icon/50% foreground color.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_ICON_FOREGROUND_ALPHA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)foreground_alpha); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_ICON_FOREGROUND_ALPHA				(WM_USER + 152)

// Multiply the icons alpha (cut items), a = (src_a * multiply_alpha) / 255.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_ICON_MULTIPLY_ALPHA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)multiply_alpha); 
// back_color is a COLORREF.
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_ICON_MULTIPLY_ALPHA				(WM_USER + 153)

// Set the thumbnail width.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_WIDE,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)wide); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_WIDE						(WM_USER + 154)

// Set the thumbnail height.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_HIGH,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)high); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_HIGH						(WM_USER + 155)

// Set the background color for the EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_BACKGROUND_COLOR,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COLORREF)back_color); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_BACKGROUND_COLOR			(WM_USER + 156)

// Set the multiply alpha (cut results) for the EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_MULTIPLY_ALPHA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COLORREF)back_color); 
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_MULTIPLY_ALPHA			(WM_USER + 157)

// Get the file info (size/dates/attributes/icon/thumbnail/type) for a result
// this will only get information that is currently cached or indexed.
// use EVERYTHING_IPC_WM_QUERY_REQUEST_FILE_INFO to request information.
// call EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_FLAGS anytime before this call to setup which values to get.
// the EVERYTHING_IPC_NOTIFY_FILE_INFO_CHANGED notification will indicate that this information has changed.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFO,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// returns non zero if successul. returns 0 if not supported.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a EVERYTHING_IPC_FILE_INFO structure.
// requires Everything 1.4

typedef struct EVERYTHING_IPC_ICON
{
	// 32bit 16 x 16 Device Independant Bitmap.
	DWORD pixels[256];
	
}EVERYTHING_IPC_ICON;

// a 256x256 thumbnail image scaled up/down to the desired size.
typedef struct EVERYTHING_IPC_THUMBNAIL
{
	// 32bit wide x high Device Independant Bitmap.
	// use EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_WIDE and EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_THUMBNAIL_HIGH to define the thumbnail dimensions.
	DWORD pixels[1];
	
}EVERYTHING_IPC_THUMBNAIL;

typedef struct EVERYTHING_IPC_FILE_INFO
{
	DWORD size_low; // if size_low and size_high are 0xffffffff the size has not been cached yet, or size information is unavailable. only valid if EVERYTHING_IPC_FILE_INFO_SIZE flag was set in EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_INFO_FLAGS.
	DWORD size_high; // if size_low and size_high are 0xffffffff the size has not been cached yet, or size information is unavailable.
	FILETIME date_modified; // if the low and high parts are both 0xffffffff the date has not been cached yet, or the date information is unavailable.
	FILETIME date_created; // if the low and high parts are both 0xffffffff the date has not been cached yet, or the date information is unavailable.
	FILETIME date_accessed; // if the low and high parts are both 0xffffffff the date has not been cached yet, or the date information is unavailable.
	DWORD attributes; // zero or more of FILE_ATTRIBUTE_* flags. the FILE_ATTRIBUTE_DIRECTORY bit is always valid, even when this data is not requested.
	DWORD icon_offset; // byte offset from the start of this structure to an EVERYTHING_IPC_ICON structure, 0 if no icon structure.
	DWORD thumbnail_offset; // byte offset from the start of this structure to an EVERYTHING_IPC_THUMBNAIL structure, 0 if no thumbnail structure.
	DWORD type_offset; // byte offset from the start of this structure to a null terminated ansi/wchar (depending on the call type) string, 0 if no type.

}EVERYTHING_IPC_FILE_INFO;

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFOA					(WM_USER + 158)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_INFOW					(WM_USER + 159)

// Read the desired file info for all the selected items.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_READ_EXTENDED_INFO_ON_SELECTION,(WPARAM)(HWND)notify_hwnd,(LPARAM)(DWORD)file_info_flags); 
// file_info_flags can be zero or more of the following values:
// EVERYTHING_IPC_FILE_INFO_SIZE
// EVERYTHING_IPC_FILE_INFO_DATE_CREATED
// EVERYTHING_IPC_FILE_INFO_DATE_MODIFIED
// EVERYTHING_IPC_FILE_INFO_DATE_ACCESSED
// EVERYTHING_IPC_FILE_INFO_ATTRIBUTES
// returns non zero if successul. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_READ_EXTENDED_INFO_ON_SELECTION			(WM_USER + 160)

// Clears all file info cache.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_REFRESH,(WPARAM)(HWND)notify_hwnd,0);
// returns non zero if successul. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_REFRESH									(WM_USER + 161)

// Determine if the result is selected.
// int is_selected = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_IS_SELECTED_RESULT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index);
// returns non zero if selected. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_IS_SELECTED_RESULT						(WM_USER + 162)

// deselect all results.
// int is_selected = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_DESELECT_ALL,(WPARAM)(HWND)notify_hwnd,0);
// returns non zero if selected. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_DESELECT_ALL							(WM_USER + 163)

// Select a result.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SELECT_RESULT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index);
// returns non zero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SELECT_RESULT							(WM_USER + 164)

// Set the first index of a range of results to select.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_SELECT_RESULT_RANGE_START,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index);
// returns non zero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_SELECT_RESULT_RANGE_START			(WM_USER + 165)

// Select a range of results.
// Call EVERYTHING_IPC_WM_QUERY_SET_SELECT_RESULT_RANGE_START to set the first index.
// set count to the number of results to select from the first index.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SELECT_RESULT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)count);
// returns non zero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SELECT_RESULT_RANGE						(WM_USER + 166)

// Select all results
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SELECT_ALL,(WPARAM)(HWND)notify_hwnd,0);
// returns non zero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SELECT_ALL								(WM_USER + 167)

// Invert the selection for all results.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_INVERT_ALL_SELECTION,(WPARAM)(HWND)notify_hwnd,0);
// returns non zero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_INVERT_ALL_SELECTION					(WM_USER + 168)

// Deselect a result.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_DESELECT_RESULT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index);
// returns non zero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_DESELECT_RESULT							(WM_USER + 169)

// Invert result selection
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_INVERT_RESULT_SELECTION,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index);
// returns non zero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_INVERT_RESULT_SELECTION					(WM_USER + 170)

// Get the number of selection results.
// int selection_count = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_SELECTION_COUNT,(WPARAM)(HWND)notify_hwnd,0);
// returns non zero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_SELECTION_COUNT						(WM_USER + 171)

// determine if the selection is a folder
// int is_folder = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_SELECTION_COUNT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)selection_index);
// returns non zero if the selection is a folder. returns 0 if not supported or the result is a file.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_IS_FOLDER_SELECTION						(WM_USER + 172)

// determine if the selection is a root (a file or folder with no parent)
// int is_root = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_IS_ROOT_SELECTION,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)selection_index);
// returns non zero if the selection is a root. returns 0 if not supported or the result is not a root.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_IS_ROOT_SELECTION						(WM_USER + 173)

// get the full path and name of a selected item
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_SELECTION_FULL_PATH_AND_NAME,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)selection_index);
// returns non zero if successful. returns 0 if not supported.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a null terminated ansi/wide string.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_SELECTION_FULL_PATH_AND_NAMEA		(WM_USER + 174)
#define EVERYTHING_IPC_WM_QUERY_GET_SELECTION_FULL_PATH_AND_NAMEW		(WM_USER + 175)

// convert the selection index to a result index.
// int result_index = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_SELECTION_RESULT_INDEX,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)selection_index);
// returns -1 if not found, returns the result index. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_SELECTION_RESULT_INDEX				(WM_USER + 176)

// Determine if the result is a ghost (cut)
// int is_ghost = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_IS_GHOST_RESULT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index);
// returns 0 if not supported or the result is not a ghost, returns nonzero if the result is a ghost.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_IS_GHOST_RESULT							(WM_USER + 177)

// Clear all ghosts
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_CLEAR_GHOSTS,(WPARAM)(HWND)notify_hwnd,0);
// returns 0 if not supported. returns nonzero if successful.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_CLEAR_GHOSTS							(WM_USER + 178)

// ghost all selected items.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GHOST_SELECTION,(WPARAM)(HWND)notify_hwnd,0);
// returns 0 if not supported. returns nonzero if successful.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GHOST_SELECTION							(WM_USER + 179)

// get the busy status text (eg: Loading... / Indexing...)
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_STATUS_TEXT,(WPARAM)(HWND)notify_hwnd,0);
// returns non zero if successful. returns 0 if not supported.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a null terminated ansi/wide string. This string is an empty string when Everything is ready.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_STATUS_TEXTA						(WM_USER + 180)
#define EVERYTHING_IPC_WM_QUERY_GET_STATUS_TEXTW						(WM_USER + 181)

// Get the total result size
// EVERYTHING_IPC_WM_QUERY_SET_SEARCH_TRACK_SELECTED_AND_TOTAL_SIZE must be set before the search is executed.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GHOST_SELECTION,(WPARAM)(HWND)notify_hwnd,0);
// returns non zero if successful. returns 0 if not supported.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a LARGE_INTEGER.
// If both the low part and high part are 0xffffffff there are no file results.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_TOTAL_RESULT_SIZE					(WM_USER + 182)

// Get the total selection size
// EVERYTHING_IPC_WM_QUERY_SET_SEARCH_TRACK_SELECTED_AND_TOTAL_SIZE must be set before the search is executed.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_TOTAL_SELECTION_SIZE,(WPARAM)(HWND)notify_hwnd,0);
// returns non zero if successful. returns 0 if not supported.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a LARGE_INTEGER.
// If both the low part and high part are 0xffffffff there is no selected files.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_TOTAL_SELECTION_SIZE				(WM_USER + 183)

// Get the current sort type
// call after a EVERYTHING_IPC_NOTIFY_RESULTS_CHANGED notification to get the up to date sort type
// since the sort type can change after a search is executed.
// int sort_type = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_SORT,(WPARAM)(HWND)notify_hwnd,0);
// returns 0 if not supported. returns one of EVERYTHING_IPC_SORT_* types.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_SORT								(WM_USER + 184)

// Set the run count for current selection.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_SORT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(DWORD)count);
// returns 0 if not supported. returns nonzero if successful.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_RUN_COUNT							(WM_USER + 185)

// Increment the run count for current selection by one.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_INCREMENT_RUN_COUNT,(WPARAM)(HWND)notify_hwnd,0);
// returns 0 if not supported. returns nonzero if successful.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_INCREMENT_RUN_COUNT						(WM_USER + 186)

// Get the result index of the result with the highest run count.
// int result_index = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_TOP_RUN_RESULT,(WPARAM)(HWND)notify_hwnd,0);
// returns 0 if not supported. returns the result index with the highest run count.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_TOP_RUN_RESULT						(WM_USER + 187)

// Get the result index of the last run result.
// int result_index = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_LAST_RUN_RESULT,(WPARAM)(HWND)notify_hwnd,0);
// returns 0 if not supported. returns the result index with the latest run date.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_LAST_RUN_RESULT						(WM_USER + 188)

// Determine if the sort type is a fast sort.
// a fast sort has presorted indexes.
// int is_fast_sort = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_IS_FAST_SORT,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)sort_type);
// sort_type MUST be one of the EVERYTHING_IPC_SORT_* types.
// returns 0 if not supported or the sort would be slow. returns nonzero if the sort will be fast.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_IS_FAST_SORT							(WM_USER + 189)

// Set the file info of interest for the EVERYTHING_IPC_COPYDATA_QUERY_FIND call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_FIND_FLAGS,(WPARAM)(HWND)notify_hwnd,(LPARAM)(DWORD)file_info_flags); 
// file_info_flags can be zero or more of the following values:
// EVERYTHING_IPC_FILE_INFO_SIZE
// EVERYTHING_IPC_FILE_INFO_DATE_CREATED
// EVERYTHING_IPC_FILE_INFO_DATE_MODIFIED
// EVERYTHING_IPC_FILE_INFO_DATE_ACCESSED
// EVERYTHING_IPC_FILE_INFO_ATTRIBUTES
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_FIND_FLAGS							(WM_USER + 190)

// Find all the files and folders in the specified location.
// Find is not affected by the current search.
// The query is used for file info cache.
// COPYDATASTRUCT cds;
// cds.dwData = EVERYTHING_IPC_COPYDATA_QUERY_FIND;
// cds.lpData = TEXT("C:\\Folder");
// cds.cbData = size of cds.lpData in bytes including the null terminator.
// int count = (int)SendMessage(everything_hwnd,WM_COPYDATA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COPYDATASTRUCT *)&cds);
// returns the number of items found in the location, returns 0 if not supported or no items found.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain one or more EVERYTHING_IPC_FIND_DATA structres.
// requires Everything 1.4

typedef struct EVERYTHING_IPC_FIND_DATA
{
	DWORD size_low; // if size_low and size_high are 0xffffffff the size information is unavailable.
	DWORD size_high; // if size_low and size_high are 0xffffffff the size information is unavailable.
	FILETIME date_modified; // if the low and high parts are both 0xffffffff the date information is unavailable.
	FILETIME date_created; // if the low and high parts are both 0xffffffff the date information is unavailable.
	FILETIME date_accessed; // if the low and high parts are both 0xffffffff the date information is unavailable.
	DWORD attributes; // zero or more of FILE_ATTRIBUTE_* flags. the FILE_ATTRIBUTE_DIRECTORY bit is always valid, even when this data is not requested.
	DWORD name_offset; // byte offset from the start of this structure to the null terminated ANSI/WCHAR (depending on call type) file name text.
	DWORD next_offset; // byte offset from the start of this structure to the next EVERYTHING_IPC_FIND_DATA structure. 0 if no next item.

}EVERYTHING_IPC_FIND_DATA;

#define EVERYTHING_IPC_COPYDATA_QUERY_FINDA								9
#define EVERYTHING_IPC_COPYDATA_QUERY_FINDW								10

// Determine if a folder exists in the local database.
// COPYDATASTRUCT cds;
// cds.dwData = EVERYTHING_IPC_COPYDATA_FOLDER_EXISTS;
// cds.lpData = TEXT("C:\\Folder");
// cds.cbData = size of cds.lpData in bytes including the null terminator.
// int index = (int)SendMessage(everything_hwnd,WM_COPYDATA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COPYDATASTRUCT *)&cds);
// returns nonzero if the folder exists, returns 0 if not supported or the folder does not exist.

#define EVERYTHING_IPC_COPYDATA_QUERY_FOLDER_EXISTSA					11
#define EVERYTHING_IPC_COPYDATA_QUERY_FOLDER_EXISTSW					12

// Determine if a file exists in the local database.
// COPYDATASTRUCT cds;
// cds.dwData = EVERYTHING_IPC_COPYDATA_FILE_EXISTS;
// cds.lpData = TEXT("C:\\Folder\\File.txt");
// cds.cbData = size of cds.lpData in bytes including the null terminator.
// int index = (int)SendMessage(everything_hwnd,WM_COPYDATA,(WPARAM)(HWND)notify_hwnd,(LPARAM)(COPYDATASTRUCT *)&cds);
// returns nonzero if the file exists, returns 0 if not supported or the file does not exist.

#define EVERYTHING_IPC_COPYDATA_QUERY_FILE_EXISTSA						13
#define EVERYTHING_IPC_COPYDATA_QUERY_FILE_EXISTSW						14

// Set the file info of interest for the EVERYTHING_IPC_COPYDATA_QUERY_GET_FILE_ATTRIBUTES call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_FILE_ATTRIBUTES_FLAGS,(WPARAM)(HWND)notify_hwnd,(LPARAM)(DWORD)file_info_flags); 
// file_info_flags can be zero or more of the following values:
// EVERYTHING_IPC_FILE_INFO_SIZE
// EVERYTHING_IPC_FILE_INFO_DATE_CREATED
// EVERYTHING_IPC_FILE_INFO_DATE_MODIFIED
// EVERYTHING_IPC_FILE_INFO_DATE_ACCESSED
// EVERYTHING_IPC_FILE_INFO_ATTRIBUTES
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_FILE_ATTRIBUTES_FLAGS	(WM_USER + 191)

// Get the file info for a file or folder by name.
// this call is not affected by the current search.
// if the information is not already cached it is gathered immedately.
// this call can be slow.
// this will return the first match of the file in the case of duplicated file names.
// call EVERYTHING_IPC_WM_QUERY_SET_GET_FILE_ATTRIBUTES_FLAGS before this call to setup which values to get.
// COPYDATASTRUCT cds;
// cds.dwData = EVERYTHING_IPC_COPYDATA_QUERY_GET_FILE_ATTRIBUTES;
// cds.lpData = TEXT("C:\\Folder\\File.txt");
// cds.cbData = size of cds.lpData in bytes including the null terminator.
// SendMessage(everything_hwnd,WM_COPYDATA,0,(LPARAM)(COPYDATASTRUCT *)&cds);
// returns non zero if successul. returns 0 if not supported.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a EVERYTHING_IPC_FILE_ATTRIBUTES structure.
// requires Everything 1.4

typedef struct EVERYTHING_IPC_FILE_ATTRIBUTES
{
	DWORD size_low; // if size_low and size_high are 0xffffffff the size information is unavailable.
	DWORD size_high; // if size_low and size_high are 0xffffffff the size information is unavailable.
	FILETIME date_modified; // if the low and high parts are both 0xffffffff the date information is unavailable.
	FILETIME date_created; // if the low and high parts are both 0xffffffff the date information is unavailable.
	FILETIME date_accessed; // if the low and high parts are both 0xffffffff the date information is unavailable.
	DWORD attributes; // zero or more of FILE_ATTRIBUTE_* flags. the FILE_ATTRIBUTE_DIRECTORY bit is always valid, even when this data is not requested.
	DWORD name_offset; // byte offset from the start of this structure to the null terminated ANSI/WCHAR file name (depending on call type) text.

}EVERYTHING_IPC_FILE_ATTRIBUTES;

#define EVERYTHING_IPC_COPYDATA_QUERY_GET_FILE_ATTRIBUTESA				15
#define EVERYTHING_IPC_COPYDATA_QUERY_GET_FILE_ATTRIBUTESW				16

// Set the file info of interest for the EVERYTHING_IPC_COPYDATA_QUERY_GET_FILE_ATTRIBUTES call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_GET_FILE_ATTRIBUTES_FLAGS,(WPARAM)(HWND)notify_hwnd,(LPARAM)(DWORD)file_info_flags); 
// file_info_flags can be zero or more of the following values:
// EVERYTHING_IPC_FILE_INFO_SIZE
// EVERYTHING_IPC_FILE_INFO_DATE_CREATED
// EVERYTHING_IPC_FILE_INFO_DATE_MODIFIED
// EVERYTHING_IPC_FILE_INFO_DATE_ACCESSED
// EVERYTHING_IPC_FILE_INFO_ATTRIBUTES
// returns nonzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_ATTRIBUTES_FLAGS	(WM_USER + 192)

// Get the find data for a result
// if the information is not already cached it is gathered immedately.
// this call can be slow, use file info requests when this information is not needed immedately.
// call EVERYTHING_IPC_WM_QUERY_SET_GET_RESULT_FILE_ATTRIBUTES_FLAGS before this call to set the file info of interest.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_ATTRIBUTES,(WPARAM)(HWND)notify_hwnd,(LPARAM)(int)result_index); 
// returns non zero if successul. returns 0 if not supported.
// Everything will reply with a WM_COPYDATA message with the dwData message part set to 
// the value specified in the call to EVERYTHING_IPC_WM_QUERY_SET_REPLY_COPYDATA_MESSAGE
// the lpData value will contain a EVERYTHING_IPC_FILE_ATTRIBUTES structure.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_ATTRIBUTESA				(WM_USER + 193)
#define EVERYTHING_IPC_WM_QUERY_GET_RESULT_FILE_ATTRIBUTESW				(WM_USER + 194)

// Create a new itemref.
// an itemref tracks a result, since a result_index may reference a different result between EVERYTHING_IPC_NOTIFY_RESULTS_CHANGED notifications.
// useful for a focus index.
// int itemref = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_CREATE_ITEMREF,(WPARAM)(HWND)notify_hwnd,0); 
// returns a new itemref handle. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_CREATE_ITEMREF							(WM_USER + 195)

// Delete a itemref.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_DELETE_ITEMREF,(WPARAM)(int)itemref,0); 
// returns a newzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_DELETE_ITEMREF							(WM_USER + 196)

// Get the result index from an itemref.
// int result_index = (int)SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_GET_ITEMREF,(WPARAM)(int)itemref,0); 
// returns a newzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_GET_ITEMREF								(WM_USER + 197)

// Set the result index for an itemref.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_SET_ITEMREF,(WPARAM)(int)itemref,(LPARAM)(int)result_index); 
// returns a newzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_SET_ITEMREF								(WM_USER + 198)

// Lock the db. When the db is locked, no updating occurs.
// this will make sure that result_index does not refer to a different item between calls.
// The db must be unlocked as soon as possible.
// there must be a matching EVERYTHING_IPC_WM_QUERY_UNLOCK for each EVERYTHING_IPC_WM_QUERY_LOCK call.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_LOCK,0,0); 
// returns a newzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_LOCK									(WM_USER + 199)

// Unlock the db.
// this will make sure that result_index does not refer to a different item between calls.
// The db must be unlocked as soon as possible.
// SendMessage(everything_hwnd,EVERYTHING_IPC_WM_QUERY_UNLOCK,0,0); 
// returns a newzero if successful. returns 0 if not supported.
// requires Everything 1.4

#define EVERYTHING_IPC_WM_QUERY_UNLOCK									(WM_USER + 200)

// end extern C
#ifdef __cplusplus
}
#endif

#endif // _EVERYTHING_H_

