#pragma once

// TODO: There are some missing/wrong entries in here.

const CMenuScreen aScreens[] = {
	// MENUPAGE_NONE = 0
	{ "", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0, },

	// MENUPAGE_STATS = 1
#ifdef MENU_MAP
	{ "FET_STA", 1, MENUPAGE_NONE, MENUPAGE_NONE, 5, 3,
#else
	{ "FET_STA", 1, MENUPAGE_NONE, MENUPAGE_NONE, 5, 2,
#endif
		MENUACTION_CHANGEMENU, "FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_NEW_GAME = 2
	{ "FET_SGA", 1, MENUPAGE_NONE, MENUPAGE_NONE, 0, 1,
		MENUACTION_CHANGEMENU, "FES_SNG", SAVESLOT_NONE, MENUPAGE_NEW_GAME_RELOAD,
		MENUACTION_POPULATESLOTS_CHANGEMENU, "GMLOAD",  SAVESLOT_NONE, MENUPAGE_CHOOSE_LOAD_SLOT,
		MENUACTION_POPULATESLOTS_CHANGEMENU, "FES_DGA", SAVESLOT_NONE, MENUPAGE_CHOOSE_DELETE_SLOT,
		MENUACTION_CHANGEMENU, "FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_BRIEFS = 3
#ifdef MENU_MAP
	{ "FET_BRE", 1, MENUPAGE_NONE, MENUPAGE_NONE, 6, 4,
#else
	{ "FET_BRE", 1, MENUPAGE_NONE, MENUPAGE_NONE, 6, 3,
#endif
		MENUACTION_CHANGEMENU, "FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_CONTROLLER_SETTINGS = 4
	{ "FET_CON", 1, MENUPAGE_OPTIONS, MENUPAGE_OPTIONS, 0, 0,
		MENUACTION_CTRLCONFIG,		"FEC_CCF", SAVESLOT_NONE, MENUPAGE_CONTROLLER_SETTINGS,
		MENUACTION_CTRLDISPLAY,		"FEC_CDP", SAVESLOT_NONE, MENUPAGE_CONTROLLER_SETTINGS,
		MENUACTION_CTRLVIBRATION,	"FEC_VIB", SAVESLOT_NONE, MENUPAGE_CONTROLLER_SETTINGS,
		MENUACTION_CHANGEMENU,		"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_SOUND_SETTINGS = 5
	{ "FET_AUD", 1, MENUPAGE_OPTIONS, MENUPAGE_OPTIONS, 1, 1,
		MENUACTION_MUSICVOLUME,		"FEA_MUS", SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS,
		MENUACTION_SFXVOLUME,		"FEA_SFX", SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS,
		MENUACTION_AUDIOHW,			"FEA_3DH", SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS,
		MENUACTION_SPEAKERCONF,		"FEA_SPK", SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS,
		MENUACTION_DYNAMICACOUSTIC,	"FET_DAM", SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS,
		MENUACTION_RADIO,			"FEA_RSS", SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS,
		MENUACTION_RESTOREDEF,		"FET_DEF", SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS,
		MENUACTION_CHANGEMENU,		"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_GRAPHICS_SETTINGS = 6
	{ "FET_DIS", 1, MENUPAGE_OPTIONS, MENUPAGE_OPTIONS, 2, 2,
		MENUACTION_BRIGHTNESS,	"FED_BRI", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_DRAWDIST,	"FEM_LOD", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_FRAMESYNC,	"FEM_VSC", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_FRAMELIMIT,	"FEM_FRM", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_TRAILS,		"FED_TRA", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_SUBTITLES,	"FED_SUB", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_WIDESCREEN,	"FED_WIS", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_SCREENRES,	"FED_RES", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_RESTOREDEF,	"FET_DEF", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_CHANGEMENU,	"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_LANGUAGE_SETTINGS = 7
	{ "FET_LAN", 1, MENUPAGE_OPTIONS, MENUPAGE_OPTIONS, 3, 3,
		MENUACTION_LANG_ENG,	"FEL_ENG", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_LANG_FRE,	"FEL_FRE", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_LANG_GER,	"FEL_GER", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_LANG_ITA,	"FEL_ITA", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_LANG_SPA,    "FEL_SPA", SAVESLOT_NONE, MENUPAGE_NONE,
#ifdef MORE_LANGUAGES
		MENUACTION_LANG_PL,	    "FEL_POL", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_LANG_RUS,    "FEL_RUS", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_LANG_JAP,	"FEL_JAP", SAVESLOT_NONE, MENUPAGE_NONE,
#endif
		MENUACTION_CHANGEMENU,	"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_CHOOSE_LOAD_SLOT = 8
	{ "FET_LG", 1, MENUPAGE_NEW_GAME, MENUPAGE_NEW_GAME, 1, 1,
		MENUACTION_CHANGEMENU,	"FESZ_CA", SAVESLOT_NONE,	MENUPAGE_NEW_GAME,
		MENUACTION_CHECKSAVE,	"FEM_SL1", SAVESLOT_1,		MENUPAGE_LOAD_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL2", SAVESLOT_2,		MENUPAGE_LOAD_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL3", SAVESLOT_3,		MENUPAGE_LOAD_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL4", SAVESLOT_4,		MENUPAGE_LOAD_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL5", SAVESLOT_5,		MENUPAGE_LOAD_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL6", SAVESLOT_6,		MENUPAGE_LOAD_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL7", SAVESLOT_7,		MENUPAGE_LOAD_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL8", SAVESLOT_8,		MENUPAGE_LOAD_SLOT_CONFIRM,
	},

	// MENUPAGE_CHOOSE_DELETE_SLOT = 9
	{ "FET_DG", 1, MENUPAGE_NEW_GAME, MENUPAGE_NEW_GAME, 2, 2,
		MENUACTION_CHANGEMENU,	"FESZ_CA",	SAVESLOT_NONE,	MENUPAGE_NEW_GAME,
		MENUACTION_CHECKSAVE,	"FEM_SL1",	SAVESLOT_1,		MENUPAGE_DELETE_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL2",	SAVESLOT_2,		MENUPAGE_DELETE_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL3",	SAVESLOT_3,		MENUPAGE_DELETE_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL4",	SAVESLOT_4,		MENUPAGE_DELETE_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL5",	SAVESLOT_5,		MENUPAGE_DELETE_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL6",	SAVESLOT_6,		MENUPAGE_DELETE_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL7",	SAVESLOT_7,		MENUPAGE_DELETE_SLOT_CONFIRM,
		MENUACTION_CHECKSAVE,	"FEM_SL8",	SAVESLOT_8,		MENUPAGE_DELETE_SLOT_CONFIRM,
	},

	// MENUPAGE_NEW_GAME_RELOAD = 10
	{ "FET_NG", 1, MENUPAGE_NEW_GAME, MENUPAGE_NEW_GAME, 0, 0,
		MENUACTION_LABEL,		"FESZ_QR",	SAVESLOT_NONE,	MENUPAGE_NONE,
		MENUACTION_CHANGEMENU,	"FEM_NO",	SAVESLOT_NONE,	MENUPAGE_NEW_GAME,
		MENUACTION_NEWGAME,		"FEM_YES",	SAVESLOT_NONE,	MENUPAGE_NEW_GAME_RELOAD,
	},

	// MENUPAGE_LOAD_SLOT_CONFIRM = 11
	{ "FET_LG", 1, MENUPAGE_CHOOSE_LOAD_SLOT, MENUPAGE_CHOOSE_LOAD_SLOT, 0, 0,
		 MENUACTION_LABEL,		"FESZ_QL",	SAVESLOT_NONE,	MENUPAGE_NONE,
		 MENUACTION_CHANGEMENU,	"FEM_NO",	SAVESLOT_NONE,	MENUPAGE_CHOOSE_LOAD_SLOT,
		 MENUACTION_CHANGEMENU,	"FEM_YES",	SAVESLOT_NONE,	MENUPAGE_LOADING_IN_PROGRESS,
	},

	// MENUPAGE_DELETE_SLOT_CONFIRM = 12
	{ "FET_DG", 1, MENUPAGE_CHOOSE_DELETE_SLOT, MENUPAGE_CHOOSE_DELETE_SLOT, 0, 0,
		 MENUACTION_LABEL,		"FESZ_QD",	SAVESLOT_NONE,  MENUPAGE_NONE,
		 MENUACTION_CHANGEMENU,	"FEM_NO",	SAVESLOT_NONE,  MENUPAGE_CHOOSE_DELETE_SLOT,
		 MENUACTION_CHANGEMENU,	"FEM_YES",	SAVESLOT_NONE,	MENUPAGE_DELETING,
	},

	// MENUPAGE_NO_MEMORY_CARD = 13
	{ "FES_NOC", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		// hud adjustment page in mobile
	},

	// MENUPAGE_LOADING_IN_PROGRESS = 14
	{ "FET_LG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_LABEL, "FED_LDW", SAVESLOT_NONE, MENUPAGE_LOAD_SLOT_CONFIRM,
	},

	// MENUPAGE_DELETING_IN_PROGRESS = 15
	{ "FET_DG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_LABEL, "FEDL_WR", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_PS2_LOAD_FAILED = 16
	{ "FET_LG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_LABEL, "FES_LOE", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_DELETE_FAILED = 17
	{ "FET_DG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_LABEL, "FES_DEE", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_CHANGEMENU, "FEC_OKK", SAVESLOT_NONE, MENUPAGE_CHOOSE_DELETE_SLOT,
	},

	// MENUPAGE_DEBUG_MENU = 18
	{ "FED_DBG", 1, MENUPAGE_NONE, MENUPAGE_NONE, 4, 0,
		MENUACTION_RELOADIDE,	"FED_RID", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_RELOADIPL,	"FED_RIP", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_SETDBGFLAG,	"FED_DFL", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_SWITCHBIGWHITEDEBUGLIGHT,	"FED_DLS", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_PEDROADGROUPS,	"FED_SPR", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_CARROADGROUPS,	"FED_SCR", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_COLLISIONPOLYS,	"FED_SCP", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_PARSEHEAP,	"FED_PAH", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_SHOWCULL,	"FED_SCZ", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_DEBUGSTREAM,	"FED_DSR", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_CHANGEMENU,	"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_MEMORY_CARD_DEBUG = 19
	{ "FEM_MCM", 1, MENUPAGE_NONE, MENUPAGE_NONE, 7, 0,
		MENUACTION_REGMEMCARD1,	"FEM_RMC", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_TESTFORMATMEMCARD1,	"FEM_TFM", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_TESTUNFORMATMEMCARD1,	"FEM_TUM", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_CREATEROOTDIR,	"FEM_CRD", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_CREATELOADICONS,	"FEM_CLI", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_FILLWITHGUFF,	"FEM_FFF", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_SAVEONLYTHEGAME,	"FEM_SOG", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_SAVEGAME,	"FEM_STG", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_SAVEGAMEUNDERGTA,	"FEM_STS", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_CREATECOPYPROTECTED,	"FEM_CPD", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_MEMORY_CARD_TEST = 20
	{ "FEM_MC2", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_MULTIPLAYER_MAIN = 21
	{ "FET_MP", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_PS2_SAVE_FAILED = 22
	{ "MCDNSP", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_MEMCARDSAVECONFIRM, "JAILB_U", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_PS2_SAVE_FAILED_2 = 23
	{ "MCGNSP", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_MEMCARDSAVECONFIRM, "JAILB_U", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// Unused in PC but anyway
	// MENUPAGE_SAVE = 24
#ifdef PS2_SAVE_DIALOG
	{ "FET_SG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_CHANGEMENU,			"FESZ_SA",	SAVESLOT_NONE, MENUPAGE_CHOOSE_SAVE_SLOT,
		MENUACTION_RESUME_FROM_SAVEZONE,	"FESZ_CA",	SAVESLOT_NONE, MENUPAGE_NONE,
	},
#else
	{ "FET_SG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_LABEL,				"FES_SCG",	SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_POPULATESLOTS_CHANGEMENU,			"GMSAVE",	SAVESLOT_NONE, MENUPAGE_CHOOSE_SAVE_SLOT,
		MENUACTION_RESUME_FROM_SAVEZONE,	"FESZ_CA",	SAVESLOT_NONE, MENUPAGE_NONE,
	},
#endif

	// MENUPAGE_NO_MEMORY_CARD_2 = 25
	{ "FES_NOC", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_CHANGEMENU,	"FESZ_CA",	SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_CHOOSE_SAVE_SLOT = 26
	{ "FET_SG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		MENUACTION_RESUME_FROM_SAVEZONE,	"FESZ_CA", SAVESLOT_NONE,	MENUPAGE_NONE,
		MENUACTION_CHANGEMENU,			"FEM_SL1", SAVESLOT_1,		MENUPAGE_SAVE_OVERWRITE_CONFIRM,
		MENUACTION_CHANGEMENU,			"FEM_SL2", SAVESLOT_2,		MENUPAGE_SAVE_OVERWRITE_CONFIRM,
		MENUACTION_CHANGEMENU,			"FEM_SL3", SAVESLOT_3,		MENUPAGE_SAVE_OVERWRITE_CONFIRM,
		MENUACTION_CHANGEMENU,			"FEM_SL4", SAVESLOT_4,		MENUPAGE_SAVE_OVERWRITE_CONFIRM,
		MENUACTION_CHANGEMENU,			"FEM_SL5", SAVESLOT_5,		MENUPAGE_SAVE_OVERWRITE_CONFIRM,
		MENUACTION_CHANGEMENU,			"FEM_SL6", SAVESLOT_6,		MENUPAGE_SAVE_OVERWRITE_CONFIRM,
		MENUACTION_CHANGEMENU,			"FEM_SL7", SAVESLOT_7,		MENUPAGE_SAVE_OVERWRITE_CONFIRM,
		MENUACTION_CHANGEMENU,			"FEM_SL8", SAVESLOT_8,		MENUPAGE_SAVE_OVERWRITE_CONFIRM,
	},

	// MENUPAGE_SAVE_OVERWRITE_CONFIRM = 27
	{ "FET_SG", 1, MENUPAGE_CHOOSE_SAVE_SLOT, MENUPAGE_CHOOSE_SAVE_SLOT, 0, 0,
		MENUACTION_LABEL,		"FESZ_QO", SAVESLOT_NONE, MENUPAGE_NONE,
		MENUACTION_CHANGEMENU,	"FEM_YES", SAVESLOT_NONE, MENUPAGE_SAVING_IN_PROGRESS,
		MENUACTION_CHANGEMENU,	"FEM_NO",  SAVESLOT_NONE, MENUPAGE_CHOOSE_SAVE_SLOT,
	},

	// MENUPAGE_MULTIPLAYER_MAP = 28
	{ "FET_MAP", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_MULTIPLAYER_CONNECTION = 29
	{ "FET_CON", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_MULTIPLAYER_FIND_GAME = 30
	{ "FET_FG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_MULTIPLAYER_MODE = 31
	{ "FET_GT", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_MULTIPLAYER_CREATE = 32
	{ "FET_HG", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_MULTIPLAYER_START = 33
	{ "FEN_STA", 2, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_SKIN_SELECT_OLD = 34
	{ "FET_PS", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

	},

	// MENUPAGE_CONTROLLER_PC = 35
	{ "FET_CTL", 1, MENUPAGE_OPTIONS, MENUPAGE_OPTIONS, 0, 0,
		MENUACTION_CTRLMETHOD,	"FET_CME", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC,
		MENUACTION_CHANGEMENU,	"FET_RDK", SAVESLOT_NONE, MENUPAGE_KEYBOARD_CONTROLS,
		MENUACTION_CHANGEMENU,	"FET_AMS", SAVESLOT_NONE, MENUPAGE_MOUSE_CONTROLS,
		MENUACTION_RESTOREDEF,	"FET_DEF", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC,
		MENUACTION_CHANGEMENU,	"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_CONTROLLER_PC_OLD1 = 36
	{ "FET_CTL", 1, MENUPAGE_CONTROLLER_PC, MENUPAGE_CONTROLLER_PC, 0, 0,
		MENUACTION_GETKEY,	"FEC_PLB", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_GETKEY,	"FEC_CWL", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_GETKEY,	"FEC_CWR", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_GETKEY,	"FEC_LKT", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_GETKEY,	"FEC_PJP", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_GETKEY,	"FEC_PSP", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_GETKEY,	"FEC_TLF", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_GETKEY,	"FEC_TRG", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_GETKEY,	"FEC_CCM", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1,
		MENUACTION_CHANGEMENU,	"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
	},

	// MENUPAGE_CONTROLLER_PC_OLD2 = 37
	{ "FET_CTL", 1, MENUPAGE_CONTROLLER_PC, MENUPAGE_CONTROLLER_PC, 1, 1,

	},

	// MENUPAGE_CONTROLLER_PC_OLD3 = 38
   { "FET_CTL", 1, MENUPAGE_CONTROLLER_PC, MENUPAGE_CONTROLLER_PC, 2, 2,
		MENUACTION_GETKEY,	"FEC_LUP", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD3,
		MENUACTION_GETKEY,	"FEC_LDN", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD3,
		MENUACTION_GETKEY,	"FEC_SMS", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD3,
		MENUACTION_SHOWHEADBOB,	"FEC_GSL", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD3,
		MENUACTION_CHANGEMENU,	"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
   },

   // MENUPAGE_CONTROLLER_PC_OLD4 = 39
   { "FET_CTL", 1, MENUPAGE_CONTROLLER_PC, MENUPAGE_CONTROLLER_PC, 3, 3,

   },

   // MENUPAGE_CONTROLLER_DEBUG = 40
   { "FEC_DBG", 1, MENUPAGE_CONTROLLER_PC, MENUPAGE_CONTROLLER_PC, 3, 3,
		MENUACTION_GETKEY,	"FEC_TGD",	SAVESLOT_NONE, MENUPAGE_CONTROLLER_DEBUG,
		MENUACTION_GETKEY,	"FEC_TDO",	SAVESLOT_NONE, MENUPAGE_CONTROLLER_DEBUG,
		MENUACTION_GETKEY,	"FEC_TSS",	SAVESLOT_NONE, MENUPAGE_CONTROLLER_DEBUG,
		MENUACTION_GETKEY,	"FEC_SMS",	SAVESLOT_NONE, MENUPAGE_CONTROLLER_DEBUG,
		MENUACTION_CHANGEMENU,	"FEDS_TB",	SAVESLOT_NONE, MENUPAGE_NONE,
   },

   // MENUPAGE_OPTIONS = 41
#ifdef MENU_MAP
   { "FET_OPT", 1, MENUPAGE_NONE, MENUPAGE_NONE, 1, 5,
#else
   { "FET_OPT", 1, MENUPAGE_NONE, MENUPAGE_NONE, 1, 4,
#endif
		MENUACTION_CHANGEMENU,		"FET_CTL", SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC,
		MENUACTION_LOADRADIO,		"FET_AUD", SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS,
		MENUACTION_CHANGEMENU,		"FET_DIS", SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS,
		MENUACTION_CHANGEMENU,		"FET_LAN", SAVESLOT_NONE, MENUPAGE_LANGUAGE_SETTINGS,
		MENUACTION_PLAYERSETUP,		"FET_PSU", SAVESLOT_NONE, MENUPAGE_SKIN_SELECT,
		MENUACTION_CHANGEMENU,		"FEDS_TB", SAVESLOT_NONE, MENUPAGE_NONE,
   },

   // MENUPAGE_EXIT = 42
#ifdef MENU_MAP
   { "FET_QG", 1, MENUPAGE_NONE, MENUPAGE_NONE, 2, 6,
#else
   { "FET_QG", 1, MENUPAGE_NONE, MENUPAGE_NONE, 2, 5,
#endif
	   MENUACTION_LABEL,		"FEQ_SRE",	SAVESLOT_NONE, MENUPAGE_NONE,
	   MENUACTION_CHANGEMENU,	"FEM_NO",	SAVESLOT_NONE, MENUPAGE_NONE,
	   MENUACTION_CANCELGAME,	"FEM_YES",	SAVESLOT_NONE, MENUPAGE_NONE,
   },

   // MENUPAGE_SAVING_IN_PROGRESS = 43
   { "", 1, MENUPAGE_CHOOSE_SAVE_SLOT, MENUPAGE_CHOOSE_SAVE_SLOT, 0, 0,
	   MENUACTION_LABEL,	"FES_WAR",	SAVESLOT_NONE, MENUPAGE_NONE,
   },

   // MENUPAGE_SAVE_SUCCESSFUL = 44
   { "FET_SG", 1, MENUPAGE_CHOOSE_SAVE_SLOT, MENUPAGE_CHOOSE_SAVE_SLOT, 0, 0,
	   MENUACTION_LABEL,				"FES_SSC",	SAVESLOT_LABEL,	MENUPAGE_NONE,
	   MENUACTION_RESUME_FROM_SAVEZONE,	"FEC_OKK",	SAVESLOT_NONE,	MENUPAGE_CHOOSE_SAVE_SLOT,
   },

   // MENUPAGE_DELETING = 45
   { "FET_DG", 1, MENUPAGE_CHOOSE_DELETE_SLOT, MENUPAGE_CHOOSE_DELETE_SLOT, 0, 0,
	   MENUACTION_LABEL,	"FED_DLW",	SAVESLOT_NONE, MENUPAGE_NONE,
   },

   // MENUPAGE_DELETE_SUCCESS = 46
   { "FET_DG", 1, MENUPAGE_CHOOSE_DELETE_SLOT, MENUPAGE_CHOOSE_DELETE_SLOT, 0, 0,
		MENUACTION_LABEL,		"DEL_FNM", SAVESLOT_NONE,	MENUPAGE_NONE,
		MENUACTION_CHANGEMENU,	"FEC_OKK", SAVESLOT_NONE,	MENUPAGE_CHOOSE_DELETE_SLOT,
   },

   // MENUPAGE_SAVE_FAILED = 47
   { "FET_SG", 1, MENUPAGE_CHOOSE_SAVE_SLOT, MENUPAGE_CHOOSE_SAVE_SLOT, 0, 0,
		MENUACTION_LABEL,		"FEC_SVU",	SAVESLOT_NONE,	MENUPAGE_NONE,
		MENUACTION_CHANGEMENU,	"FEC_OKK",	SAVESLOT_NONE,	MENUPAGE_CHOOSE_SAVE_SLOT,
   },

   // MENUPAGE_LOAD_FAILED = 48
   { "FET_SG", 1, MENUPAGE_CHOOSE_SAVE_SLOT, MENUPAGE_CHOOSE_SAVE_SLOT, 0, 0,
		MENUACTION_LABEL,	"FEC_SVU",	SAVESLOT_NONE,	MENUPAGE_NONE,
   },

   // MENUPAGE_LOAD_FAILED_2 = 49
   { "FET_LG", 1, MENUPAGE_CHOOSE_SAVE_SLOT, MENUPAGE_CHOOSE_SAVE_SLOT, 0, 0,
		MENUACTION_LABEL,		"FEC_LUN",	SAVESLOT_NONE,  MENUPAGE_NONE,
		MENUACTION_CHANGEMENU,	"FEDS_TB",	SAVESLOT_NONE,  MENUPAGE_CHOOSE_LOAD_SLOT,
   },

   // MENUPAGE_FILTER_GAME = 50
   { "FIL_FLT", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,

   },

   // MENUPAGE_START_MENU = 51
   { "FEM_MM", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
		 MENUACTION_CHANGEMENU,	"FEN_STA",	SAVESLOT_NONE,	MENUPAGE_NEW_GAME,
		 MENUACTION_CHANGEMENU,	"FET_OPT",	SAVESLOT_NONE,	MENUPAGE_OPTIONS,
		 MENUACTION_CHANGEMENU,	"FEM_QT",	SAVESLOT_NONE,	MENUPAGE_EXIT,
   },

   // MENUPAGE_PAUSE_MENU = 52
   { "FET_PAU", 1, MENUPAGE_DISABLED, MENUPAGE_DISABLED, 0, 0,
	   MENUACTION_RESUME,		"FEM_RES",	SAVESLOT_NONE, MENUPAGE_NONE,
	   MENUACTION_CHANGEMENU,	"FEN_STA",	SAVESLOT_NONE, MENUPAGE_NEW_GAME,
#ifdef MENU_MAP
	   MENUACTION_CHANGEMENU,	"FEG_MAP",	SAVESLOT_NONE, MENUPAGE_MAP,
#endif
	   MENUACTION_CHANGEMENU,	"FEP_STA",	SAVESLOT_NONE, MENUPAGE_STATS,
	   MENUACTION_CHANGEMENU,	"FEP_BRI",	SAVESLOT_NONE, MENUPAGE_BRIEFS,
	   MENUACTION_CHANGEMENU,	"FET_OPT",	SAVESLOT_NONE, MENUPAGE_OPTIONS,
	   MENUACTION_CHANGEMENU,	"FEM_QT",	SAVESLOT_NONE, MENUPAGE_EXIT,
   },

   // MENUPAGE_CHOOSE_MODE = 53
   { "FEN_STA", 1, MENUPAGE_NONE, MENUPAGE_NONE, 0, 1,
	   MENUACTION_CHANGEMENU,	"FET_SP",	SAVESLOT_NONE, MENUPAGE_NEW_GAME,
	   MENUACTION_INITMP,		"FET_MP",	SAVESLOT_NONE, MENUPAGE_MULTIPLAYER_MAIN,
	   MENUACTION_CHANGEMENU,	"FEDS_TB",	SAVESLOT_NONE, MENUPAGE_NONE,
   },

   // MENUPAGE_SKIN_SELECT = 54
   { "FET_PSU", 1, MENUPAGE_OPTIONS, MENUPAGE_OPTIONS, 4, 4,
		MENUACTION_CHANGEMENU,	"FEDS_TB",	SAVESLOT_NONE, MENUPAGE_MULTIPLAYER_MAIN,
   },

   // MENUPAGE_KEYBOARD_CONTROLS = 55
   { "FET_STI", 1, MENUPAGE_CONTROLLER_PC, MENUPAGE_CONTROLLER_PC, 1, 1,
		MENUACTION_CHANGEMENU,	"FEDS_TB",	SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC,
   },

   // MENUPAGE_MOUSE_CONTROLS = 56
   { "FET_MTI", 1, MENUPAGE_CONTROLLER_PC, MENUPAGE_CONTROLLER_PC, 2, 2,
	   MENUACTION_MOUSESENS,	"FEC_MSH",	SAVESLOT_NONE, MENUPAGE_MOUSE_CONTROLS,
	   MENUACTION_INVVERT,		"FEC_IVV",	SAVESLOT_NONE, MENUPAGE_MOUSE_CONTROLS,
	   MENUACTION_MOUSESTEER,	"FET_MST",	SAVESLOT_NONE, MENUPAGE_MOUSE_CONTROLS,
	   MENUACTION_CHANGEMENU,	"FEDS_TB",	SAVESLOT_NONE, MENUPAGE_NONE,
   },

   // MENUPAGE_57 = 57
   { "", 0, MENUPAGE_NONE, MENUPAGE_NONE, 0, 0,
	   // mission failed, wanna restart page in mobile
   },

   // MENUPAGE_58 = 58
   { "", 0, MENUPAGE_NONE, MENUPAGE_NONE, 0, 0,

   },

#ifdef MENU_MAP
   // MENUPAGE_MAP = 59
   { "FEG_MAP", 1, MENUPAGE_NONE, MENUPAGE_NONE, 5, 2,
   },
#endif
};
