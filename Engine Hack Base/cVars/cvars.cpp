#include "cvars.h"
#include "../locals.h"
#include "../ClientDll/ClientDll.h"
#include "../WayBot/ways.h"
#include "../Console/console.h"
#include "../Drawing/Drawing.h"
#include "../cFont/TableFont.h"

CVars cvar;
CFunc cfunc;

bool strafe = false;
bool gstrafe = false;
bool jumpbug = false;
bool fastrun = false;
bool freelook = false;
bool norefresh = false;

bool CFunc::forwardm = false;
bool CFunc::moveleft = false;
bool CFunc::moveright = false;
bool CFunc::back = false;
bool CFunc::duck = false;
bool CFunc::jump = false;

void CFunc::p_for() { CFunc::forwardm = true; }
void CFunc::m_for() { CFunc::forwardm = false; }

void CFunc::p_ml() { CFunc::moveleft = true; }
void CFunc::m_ml() { CFunc::moveleft = false; }

void CFunc::p_mr() { CFunc::moveright = true; }
void CFunc::m_mr() { CFunc::moveright = false; }

void CFunc::p_bk() { CFunc::back = true; }
void CFunc::m_bk() { CFunc::back = false; }

void CFunc::p_dk() { CFunc::duck = true; }
void CFunc::m_dk() { CFunc::duck = false; }

void CFunc::p_jp() { CFunc::jump = true; }
void CFunc::m_jp() { CFunc::jump = false; }

bool CheckCvarC(char* cVarName)
{
	cvar_s *cTemp = nullptr;
	cTemp = g_Engine.pfnGetCvarPointer(cVarName);
	if ((DWORD*)cTemp != nullptr)
		return true;
	else
		return false;
}
void CFunc::fps_on()
{
	if(CheckCvarC("fps_override"))
		g_Engine.pfnClientCmd("fps_override 1");

	g_Engine.pfnClientCmd(PrefHack("exec +fps.cfg",prefix,""));
	g_Local.bFPS = true;
}

void CFunc::fps_off()
{
	if(CheckCvarC("fps_override"))
		g_Engine.pfnClientCmd("fps_override 0");

	g_Engine.pfnClientCmd(PrefHack("exec -fps.cfg",prefix,""));
	g_Local.bFPS = false;
}

static bool fix_str = false;

void strafe_on()
{
	if ( cvar.strafe_helper->value == 1 )
	{
		fix_str = true;
		cvar.strafe_helper->value = 0;
	}
	if(cvar.strafe_fps->value)
	{
		cfunc.fps_on();
	}
	strafe = true;
}

void strafe_off()
{
	if ( cvar.strafe_helper->value == 0 && fix_str == true )
	{
		fix_str = false;
		cvar.strafe_helper->value = 1;
	}
	if(cvar.strafe_fps->value)
	{
		cfunc.fps_off();
	}
	strafe = false;
}

void gstrafe_on()
{
	gstrafe = true;
}

void gstrafe_off()
{
	gstrafe = false;
}

void fastrun_on()
{
	fastrun = true;
}

void fastrun_off()
{
	fastrun = false;
}

void FastRunSwitch() {
	char state[2];
	strncpy( state, g_Engine.Cmd_Argv(1), 1 );
	state[1] = '\0';
	if(state[0] == '0') { fastrun = false; }
	else	if(state[0] == '1') { fastrun = true; }
	else { fastrun = !fastrun; }
}

void FreeLookSwitch() {
	char state[2];
	strncpy( state, g_Engine.Cmd_Argv(1), 1 );
	state[1] = '\0';
	if(state[0] == '0') { freelook = false; }
	else	if(state[0] == '1') { freelook = true; }
	else { freelook = !freelook; }
}

void jumpbug_on()
{
	jumpbug = true;
}

void jumpbug_off()
{
	jumpbug = false;
}

void exec() 
{
	char patch[256];
	sprintf(patch,"settings/%s",g_Engine.Cmd_Argv(1));
	RunScript(patch); 
}

void playsnnd()
{
	if(g_Engine.Cmd_Argc()>=1)
	{
		FILE *sfile = fopen(szDirFile(g_Engine.Cmd_Argv(1)).c_str(), "r");
		if(sfile != NULL) {
			char script[1024];
			while(fgets(script, sizeof(script), sfile) != NULL)
			{
				g_Engine.pfnPlayMp3(g_Engine.Cmd_Argv(1),0);
			}
			fclose(sfile);
		} else g_Engine.Con_Printf("%s is invalid sound file!\n", g_Engine.Cmd_Argv(1));
	}
}

void RandomValue()
{
	if(g_Engine.Cmd_Argc()<4)
	{
		g_Engine.Con_Printf("random_value <cvar name> <min value> <max value>\n");
	}
	else
	{
		float r=g_Engine.pfnRandomFloat(atof(g_Engine.Cmd_Argv(2)),atof(g_Engine.Cmd_Argv(3)));
		g_Engine.Cvar_SetValue(g_Engine.Cmd_Argv(1),r);
	}
}

void loopx()
{
	g_Engine.pfnClientCmd(g_Engine.Cmd_Argv(1));
}

void saygreen()
{
	// start code green chat by _or_75
	char saygreen[256];
	int j = 0;
	j = sprintf(saygreen,"%s","say \"");
	j += sprintf(saygreen+j,"%c",0x01);
	j += sprintf(saygreen+j,"%c",0x04);
	j += sprintf(saygreen+j,"%s",g_Engine.Cmd_Argv(1));
	j += sprintf(saygreen+j,"%s","\"");
	g_Engine.pfnClientCmd(saygreen);
	// end code green chat by _or_75
}

void txtHUD()
{
	if ( g_Engine.Cmd_Argc() <= 1 )
	{
		g_Engine.Con_Printf("txt <message> ...\n");
	}
	else
	{
		if ( g_nStatus.connected ) 
		{
			strcpy(txtHUDBuffMsg,"\0");
			int argc = g_Engine.Cmd_Argc()-1;
			if ( argc == 1 )
			{
				strcpy(txtHUDBuffMsg,g_Engine.Cmd_Argv(1));
			}
			else
			{
				for(int i = 1;i <= argc;i++)
				{
					strcat(txtHUDBuffMsg," ");
					strcat(txtHUDBuffMsg,g_Engine.Cmd_Argv(i));
				}
			}
			txtHUDM = true;
		}
	}
}

char Dic[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZzyxwvutsrqponmlkjihgfedcba";
char Input[] = "CSX-000000";
unsigned int treasure;
int DicLen,InputLen;
unsigned int hashing(char *message)
{
	int i = 0;
	unsigned int hash = 0x4E67C6A7;
	int current_char = message[i++];
	while ( current_char )
	{
		hash = hash ^ ((hash >> 2) + hash * 32 + current_char);
		current_char = message[i++];
	}
	return hash;
}
bool ScanLast3(unsigned int PrevHash)
{
	unsigned int h1,h2,h3,hh;
	for (int i1=0; i1 < DicLen; i1++) {
		h1 = PrevHash ^ ((PrevHash >> 2) + (PrevHash << 5) + (unsigned char)Dic[i1]);
		hh = h1 ^ ((h1 >> 2) + (h1 << 5));
		hh = hh ^ ((hh >> 2) + (hh << 5));
		if ((hh ^ treasure) >> (8+5+3))
			continue;
		for (int i2=0; i2 < DicLen; i2++) {
			h2 = h1 ^ ((h1 >> 2) + (h1 << 5) + (unsigned char)Dic[i2]);
			hh = h2 ^ ((h2 >> 2) + (h2 << 5));
			if ((hh ^ treasure) >> (8+3))
				continue;
			for (int i3=0; i3 < DicLen; i3++) {
				h3 = h2 ^ ((h2 >> 2) + (h2 << 5) + (unsigned char)Dic[i3]);
				if (h3 == treasure) {
					Input[InputLen-3] = Dic[i1];
					Input[InputLen-2] = Dic[i2];
					Input[InputLen-1] = Dic[i3];
					return 1;
				}
			}
		}
	}
	return 0;
}
bool ScanNext(int Index, unsigned int PrevHash){
	bool res;
	for (int i=0; i < DicLen; i++) {
		unsigned int h = PrevHash ^ ((PrevHash >> 2) + (PrevHash << 5) + (unsigned char)Dic[i]);
		if (Index+1 < InputLen-3)
			res = ScanNext(Index+1,h);
		else
			res = ScanLast3(h);
		if (res) {
			Input[Index] = Dic[i];
			return 1;
		}
	}
	return 0;
}
bool bruteforce()
{
	DicLen = strlen(Dic);
	InputLen = strlen(Input);
	int i = InputLen-7;
	if (i < 0) {i=0;}
	Input[i] = '\0';
	unsigned int h = hashing(Input);
	return ScanNext(i,h);
}
void PatchMemSteamId()
{
	DWORD PatchAddr = (DWORD)GetModuleHandle("steamclient.dll") + 0x5B4D4;
	*(int*)PatchAddr = 0;
}
void HackSteamID()
{
	if ( g_Engine.Cmd_Argc() == 2 ) {
		treasure = atoi(g_Engine.Cmd_Argv(1));
		if ( bruteforce() == 1 )
		{
			DWORD sMask =  (DWORD)GetModuleHandleA("steamclient.dll") + 0x5A448;
			PatchMemSteamId();
			*(string*)sMask = (string)Input;
			g_Engine.Con_Printf("SteamID Set: %u\n", hashing(Input));
		}
	}
	else
	{
		g_Engine.Con_Printf("Check the syntax\n");
	}
}
void DelAlias()
{
	if ( g_Engine.Cmd_Argc() != 2 )
	{
		g_Engine.Con_Printf("remove <alias name>");
	}
	else
	{
		char dela[256];
		sprintf(dela,"alias %s \"\"",g_Engine.Cmd_Argv(1));
		g_Engine.pfnClientCmd(dela);
	}
}

void CFunc::Init()
{
	g_Engine.pfnAddCommand("remove",DelAlias);
	g_Engine.pfnAddCommand("txt",txtHUD);
	g_Engine.pfnAddCommand(PrefHack("loopx",prefix,""),loopx);
	g_Engine.pfnAddCommand("random_value", RandomValue);
	g_Engine.pfnAddCommand(PrefHack("say_green",prefix,""),saygreen);
	
	cvar.wallhack_xqz = g_Engine.pfnRegisterVariable(PrefHack("wallhack_xqz",prefix,""),"0",0);
	cvar.whitewalls = g_Engine.pfnRegisterVariable(PrefHack("whitewalls",prefix,""),"0",0);

	cvar.knifebot = g_Engine.pfnRegisterVariable(PrefHack("knifebot",prefix,""),"1",0);
	cvar.knifebot_aim = g_Engine.pfnRegisterVariable(PrefHack("knifebot_aim",prefix,""),"1",0);
	cvar.knifebot_aim_fov = g_Engine.pfnRegisterVariable(PrefHack("knifebot_aim_fov",prefix,""),"45",0);
	cvar.knifebot_aim_wp = g_Engine.pfnRegisterVariable(PrefHack("knifebot_aim_wp",prefix,""),"0",0);
	cvar.knifebot_team = g_Engine.pfnRegisterVariable(PrefHack("knifebot_team",prefix,""),"0",0);
	cvar.knifebot_attack = g_Engine.pfnRegisterVariable(PrefHack("knifebot_attack",prefix,""),"2",0); 
	cvar.knifebot_dist_attack = g_Engine.pfnRegisterVariable(PrefHack("knifebot_dist_attack",prefix,""),"72",0);
	cvar.knifebot_dist_attack2 = g_Engine.pfnRegisterVariable(PrefHack("knifebot_dist_attack2",prefix,""),"64",0);
	cvar.knifebot_fps = g_Engine.pfnRegisterVariable(PrefHack("knifebot_fps",prefix,""),"0",0);
	cvar.knifebot_fps_time = g_Engine.pfnRegisterVariable(PrefHack("knifebot_fps_time",prefix,""),"1",0);

	cvar.lj_stats_log = g_Engine.pfnRegisterVariable(PrefHack("lj_stats_log",prefix,""),"0",0);

	cvar.fullbright = g_Engine.pfnRegisterVariable(PrefHack("fullbright",prefix,""),"0",0);

	cvar.noflash = g_Engine.pfnRegisterVariable(PrefHack("noflash",prefix,""),"130",0);

	cvar.nospread = g_Engine.pfnRegisterVariable(PrefHack("nospread",prefix,""),"0",0);

	cvar.norecoil = g_Engine.pfnRegisterVariable(PrefHack("norecoil",prefix,""),"0",0);

	cvar.autopistol = g_Engine.pfnRegisterVariable(PrefHack("autopistol",prefix,""),"0",0);
	cvar.autopistol_wpn_fix = g_Engine.pfnRegisterVariable(PrefHack("autopistol_wpn_fix",prefix,""),"1",0);

	cvar.cam = g_Engine.pfnRegisterVariable(PrefHack("cam",prefix,""),"0",0);
	cvar.cam_up = g_Engine.pfnRegisterVariable(PrefHack("cam_up",prefix,""),"0",0);
	cvar.cam_back = g_Engine.pfnRegisterVariable(PrefHack("camp_back",prefix,""),"100",0);
	cvar.cam_side = g_Engine.pfnRegisterVariable(PrefHack("camp_side",prefix,""),"0",0);

	cvar.SpinHack = g_Engine.pfnRegisterVariable(PrefHack("spinhack",prefix,""),"0",0);
	cvar.SpinHackSpeed = g_Engine.pfnRegisterVariable(PrefHack("spinhackspeed",prefix,""),"7",0);

	cvar.aim = g_Engine.pfnRegisterVariable(PrefHack("aim",prefix,""),"0",0);
	cvar.aim_team = g_Engine.pfnRegisterVariable(PrefHack("aim_team",prefix,""),"0",0);
	cvar.aim_mode = g_Engine.pfnRegisterVariable(PrefHack("aim_mode",prefix,""),"1",0);
	cvar.aim_fov = g_Engine.pfnRegisterVariable(PrefHack("aim_fov",prefix,""),"360",0);
	cvar.aim_help = g_Engine.pfnRegisterVariable(PrefHack("aim_help",prefix,""),"1",0);
	cvar.aim_auto = g_Engine.pfnRegisterVariable(PrefHack("aim_auto",prefix,""),"0",0);
	cvar.aim_autowall = g_Engine.pfnRegisterVariable(PrefHack("aim_autowall",prefix,""),"0",0);

	cvar.aim_height = g_Engine.pfnRegisterVariable(PrefHack("aim_height",prefix,""),"21",0);
	cvar.aim_height_hitbox = g_Engine.pfnRegisterVariable(PrefHack("aim_height_hitbox",prefix,""),"11",0);
	cvar.aim_height_bone = g_Engine.pfnRegisterVariable(PrefHack("aim_height_bone",prefix,""),"9",0);

	cvar.aim_smooth = g_Engine.pfnRegisterVariable(PrefHack("aim_smooth",prefix,""),"0",0);
	cvar.aim_prediction = g_Engine.pfnRegisterVariable(PrefHack("aim_prediction",prefix,""),"1",0);
	cvar.aim_prediction_offset = g_Engine.pfnRegisterVariable(PrefHack("aim_prediction_offset",prefix,""),"0",0);
	cvar.aim_auto_attack = g_Engine.pfnRegisterVariable(PrefHack("aim_auto_attack",prefix,""),"1",0);

	cvar.triggerbot = g_Engine.pfnRegisterVariable(PrefHack("triggerbot",prefix,""),"0",0);
	cvar.triggerbot_spot = g_Engine.pfnRegisterVariable(PrefHack("triggerbot_spot",prefix,""),"10",0);
	cvar.triggerbot_fov = g_Engine.pfnRegisterVariable(PrefHack("triggerbot_fov",prefix,""),"35",0);

	cvar.addvec_j = g_Engine.pfnRegisterVariable("addvec_j","2 5 13",0);
	cvar.addvec_s = g_Engine.pfnRegisterVariable("addvec_s","5 2 20",0);
	cvar.addvec_d = g_Engine.pfnRegisterVariable("addvec_d","5 2 25",0);

	cvar.aim_silent = g_Engine.pfnRegisterVariable(PrefHack("aim_silent",prefix,""),"0",0);

	cvar.show = g_Engine.pfnRegisterVariable(PrefHack("show",prefix,""),"1",0);
	cvar.show_fps = g_Engine.pfnRegisterVariable(PrefHack("show_fps",prefix,""),"1",0);
	cvar.show_time = g_Engine.pfnRegisterVariable(PrefHack("show_time",prefix,""),"1",0);
	cvar.show_keys = g_Engine.pfnRegisterVariable(PrefHack("show_keys",prefix,""),"1",0);
	cvar.show_color = g_Engine.pfnRegisterVariable(PrefHack("show_color",prefix,""),"0 255 0",0);

	cvar.show_info = g_Engine.pfnRegisterVariable(PrefHack("show_info",prefix,""),"1",0);
	cvar.show_info_main = g_Engine.pfnRegisterVariable(PrefHack("show_info_main",prefix,""),"1",0);
	cvar.show_info_strafe = g_Engine.pfnRegisterVariable(PrefHack("show_info_strafe",prefix,""),"1",0);
	cvar.show_info_helper = g_Engine.pfnRegisterVariable(PrefHack("show_info_helper",prefix,""),"1",0);
	cvar.show_info_waybot = g_Engine.pfnRegisterVariable(PrefHack("show_info_waybot",prefix,""),"1",0);

	cvar.jb_auto = g_Engine.pfnRegisterVariable(PrefHack("jumpbug_auto",prefix,""),"0",0);
	
	cvar.show_kz = g_Engine.pfnRegisterVariable(PrefHack("show_kz",prefix,""),"1",0);
	cvar.show_kz_edge = g_Engine.pfnRegisterVariable(PrefHack("show_kz_edge",prefix,""),"1",0);
	cvar.show_kz_speed = g_Engine.pfnRegisterVariable(PrefHack("show_kz_speed",prefix,""),"1",0);
	cvar.show_kz_height = g_Engine.pfnRegisterVariable(PrefHack("show_kz_height",prefix,""),"0",0);
	cvar.show_kz_fallspeed = g_Engine.pfnRegisterVariable(PrefHack("show_kz_fallspeed",prefix,""),"0",0);
	cvar.show_kz_damage = g_Engine.pfnRegisterVariable(PrefHack("show_kz_damage",prefix,""),"1",0);
	cvar.show_kz_ljstats = g_Engine.pfnRegisterVariable(PrefHack("show_kz_ljstats",prefix,""),"1",0);
	cvar.show_kz_color = g_Engine.pfnRegisterVariable(PrefHack("show_kz_color",prefix,""),"255 255 255",0);

	cvar.show_glow_model = g_Engine.pfnRegisterVariable(PrefHack("show_glow_model",prefix,""),"0",0);
	cvar.show_glow_model_color = g_Engine.pfnRegisterVariable(PrefHack("show_glow_model_color",prefix,""),"255 64 64",0);
	cvar.show_glow_model_ticknes = g_Engine.pfnRegisterVariable(PrefHack("show_glow_model_ticknes",prefix,""),"1",0);
	cvar.show_glow_model_team = g_Engine.pfnRegisterVariable(PrefHack("show_glow_model_team",prefix,""),"1",0);

	cvar.show_glow_weapon = g_Engine.pfnRegisterVariable(PrefHack("show_glow_weapon",prefix,""),"0",0);
	cvar.show_glow_weapon_color = g_Engine.pfnRegisterVariable(PrefHack("show_glow_weapon_color",prefix,""),"0 255 0",0);
	cvar.show_glow_weapon_tickness = g_Engine.pfnRegisterVariable(PrefHack("show_glow_weapon_tickness",prefix,""),"1",0);

	cvar.esp = g_Engine.pfnRegisterVariable(PrefHack("esp",prefix,""),"1",0);
	cvar.esp_name = g_Engine.pfnRegisterVariable(PrefHack("esp_name",prefix,""),"32",0);
	cvar.esp_weapon = g_Engine.pfnRegisterVariable(PrefHack("esp_weapon",prefix,""),"0",0);
	cvar.esp_sequence = g_Engine.pfnRegisterVariable(PrefHack("esp_sequence",prefix,""),"0",0);
	cvar.esp_box = g_Engine.pfnRegisterVariable(PrefHack("esp_box",prefix,""),"0",0);
	cvar.esp_box_size = g_Engine.pfnRegisterVariable(PrefHack("esp_box_size",prefix,""),"1",0);
	cvar.esp_3dbox = g_Engine.pfnRegisterVariable(PrefHack("esp_3dbox",prefix,""),"0",0);
	cvar.esp_distance = g_Engine.pfnRegisterVariable(PrefHack("esp_distance",prefix,""),"0",0);
	cvar.esp_gun_shot = g_Engine.pfnRegisterVariable(PrefHack("esp_gun_shot",prefix,""),"0",0);

	cvar.esp_far = g_Engine.pfnRegisterVariable(PrefHack("esp_far",prefix,""),"0",0);
	cvar.esp_far_smooth = g_Engine.pfnRegisterVariable(PrefHack("esp_far_smooth",prefix,""),"1",0);

	cvar.esp_sound  = g_Engine.pfnRegisterVariable(PrefHack("esp_sound",prefix,""),"0",0);
	cvar.esp_sound_team  = g_Engine.pfnRegisterVariable(PrefHack("esp_sound_team",prefix,""),"1",0);
	cvar.esp_sound_color = g_Engine.pfnRegisterVariable(PrefHack("esp_sound_color",prefix,""),"0 255 0",0);
	cvar.esp_sound_text = g_Engine.pfnRegisterVariable(PrefHack("esp_sound_text",prefix,""),"0",0);

	cvar.gl_font = g_Engine.pfnRegisterVariable(PrefHack("opengl_font",prefix,""),"1",0);

	cvar.speed = g_Engine.pfnRegisterVariable(PrefHack("speed",prefix,""),"1",0);
	cvar.speed_net = g_Engine.pfnRegisterVariable(PrefHack("speed_net",prefix,""),"0",0);

	cvar.waybot_speed_fps = g_Engine.pfnRegisterVariable(PrefHack("waybot_speed_fps",prefix,""),"1",0); 
	cvar.waybot_speed_fps_file = g_Engine.pfnRegisterVariable(PrefHack("waybot_speed_fps_file",prefix,""),"fps_speed.cfg",0);

	cvar.waybot_speed = g_Engine.pfnRegisterVariable(PrefHack("waybot_speed",prefix,""),"20",0);
	cvar.waybot_color = g_Engine.pfnRegisterVariable(PrefHack("waybot_color",prefix,""),"0 255 0",0);

	g_Engine.pfnAddCommand(PrefHack("waybot",prefix,"+"), Way.Resume);
	g_Engine.pfnAddCommand(PrefHack("waybot",prefix,"-"), Way.Stop);
	g_Engine.pfnAddCommand(PrefHack("waybot_add",prefix,""), Way.CreatePoint);
	g_Engine.pfnAddCommand(PrefHack("waybot_clear",prefix,""), Way.ClearPoints);
	g_Engine.pfnAddCommand(PrefHack("waybot_clear_last",prefix,""), Way.ClearLastPoint);
	g_Engine.pfnAddCommand(PrefHack("waybot_edit",prefix,""), Way.EditPoint);
	g_Engine.pfnAddCommand(PrefHack("waybot_load",prefix,""), Way.Load);
	g_Engine.pfnAddCommand(PrefHack("waybot_save",prefix,""), Way.Save);

	cvar.bhop = g_Engine.pfnRegisterVariable(PrefHack("bhop",prefix,""),"1",0);
	cvar.bhop_nsd = g_Engine.pfnRegisterVariable(PrefHack("bhop_nsd",prefix,""),"1",0);
	cvar.bhop_cnt = g_Engine.pfnRegisterVariable(PrefHack("bhop_cnt",prefix,""),"4",0);
	cvar.bhop_cnt_rand = g_Engine.pfnRegisterVariable(PrefHack("bhop_cnt_rand",prefix,""),"1",0);
	cvar.bhop_cnt_rand_min = g_Engine.pfnRegisterVariable(PrefHack("bhop_cnt_rand_min",prefix,""),"0",0);
	cvar.bhop_cnt_rand_max = g_Engine.pfnRegisterVariable(PrefHack("bhop_cnt_rand_max",prefix,""),"12",0);
	cvar.bhop_autoduck = g_Engine.pfnRegisterVariable(PrefHack("bhop_autoduck",prefix,""),"0",0);

	cvar.hide_player = g_Engine.pfnRegisterVariable(PrefHack("hide_player",prefix,""),"0",0);

	cvar.fps_helper = g_Engine.pfnRegisterVariable(PrefHack("fps_helper",prefix,""),"0",0);
	cvar.fps_slowto = g_Engine.pfnRegisterVariable(PrefHack("fps_slowto",prefix,""),"0",0);

	cvar.freelook_speed = g_Engine.pfnRegisterVariable(PrefHack("freelook_speed",prefix,""),"700",0);

	cvar.gstrafe_bhop = g_Engine.pfnRegisterVariable(PrefHack("groundstrafe_bhop",prefix,""),"1",0);
	cvar.gstrafe_standup = g_Engine.pfnRegisterVariable(PrefHack("groundstrafe_standup",prefix,""),"0",0);
	cvar.gstrafe_noslowdown = g_Engine.pfnRegisterVariable(PrefHack("groundstrafe_noslowdown",prefix,""),"1",0);

	cvar.fixedpitch = g_Engine.pfnRegisterVariable(PrefHack("fixedpitch",prefix,""),"0",0);
	cvar.fixedyaw = g_Engine.pfnRegisterVariable(PrefHack("fixedyaw",prefix,""),"0",0);

	cvar.strafe_aa = g_Engine.pfnRegisterVariable(PrefHack("strafe_aa",prefix,""),"100",0);
	cvar.strafe_fps = g_Engine.pfnRegisterVariable(PrefHack("strafe_fps",prefix,""),"1",0);
	cvar.strafe_dir = g_Engine.pfnRegisterVariable(PrefHack("strafe_dir",prefix,""),"1",0);
	cvar.strafe_dir_auto = g_Engine.pfnRegisterVariable(PrefHack("strafe_dir_auto",prefix,""),"0",0);
	cvar.strafe_temp = g_Engine.pfnRegisterVariable(PrefHack("strafe_temp",prefix,""),"0.746",0);
	cvar.strafe_invisible = g_Engine.pfnRegisterVariable(PrefHack("strafe_invisible",prefix,""),"1",0);

	cvar.strafe_speed = g_Engine.pfnRegisterVariable(PrefHack("strafe_speed",prefix,""),"69",0);
	cvar.strafe_crazy = g_Engine.pfnRegisterVariable(PrefHack("strafe_crazy",prefix,""),"0",0);
	cvar.strafe_helper = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper",prefix,""),"0",0);
	cvar.strafe_helper_boost = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_boost",prefix,""),"0",0);
	cvar.strafe_helper_add_strafe = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_add_strafe",prefix,""),"1",0);
	cvar.strafe_helper_max_strafe = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_max_strafe",prefix,""),"6",0);
	cvar.strafe_helper_max_strafe_rand = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_max_strafe_rand",prefix,""),"0",0);
	cvar.strafe_helper_max_strafe_rand_min = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_max_strafe_rand_min",prefix,""),"3",0);
	cvar.strafe_helper_max_strafe_rand_max = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_max_strafe_rand_max",prefix,""),"6",0);
	cvar.strafe_helper_move = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_move",prefix,""),"400",0);
	cvar.strafe_helper_move_rand = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_move_rand",prefix,""),"1",0);
	cvar.strafe_helper_move_rand_min = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_move_rand_min",prefix,""),"50",0);
	cvar.strafe_helper_move_rand_max = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_move_rand_max",prefix,""),"400",0);
	cvar.strafe_helper_main = g_Engine.pfnRegisterVariable(PrefHack("strafe_helper_main",prefix,""),"30",0);
	
	cvar.debug_sound = g_Engine.pfnRegisterVariable(PrefHack("debug_sound",prefix,""),"0",0);

	g_Engine.pfnAddCommand(PrefHack("exec",prefix,""), exec); 

	g_Engine.pfnAddCommand(PrefHack("play_sound",prefix,""), playsnnd);

	g_Engine.pfnAddCommand(PrefHack("fps",prefix,"+"), CFunc::fps_on); 
	g_Engine.pfnAddCommand(PrefHack("fps",prefix,"-"), CFunc::fps_off);

	g_Engine.pfnAddCommand(PrefHack("strafe",prefix,"+"), strafe_on); 
	g_Engine.pfnAddCommand(PrefHack("strafe",prefix,"-"), strafe_off);

	g_Engine.pfnAddCommand(PrefHack("fastrun",prefix,"+"), fastrun_on);
	g_Engine.pfnAddCommand(PrefHack("fastrun",prefix,"-"), fastrun_off);
	g_Engine.pfnAddCommand(PrefHack("fastrun",prefix,""),FastRunSwitch);

	g_Engine.pfnAddCommand(PrefHack("freelook",prefix,""),FreeLookSwitch);

	g_Engine.pfnAddCommand(PrefHack("groundstrafe",prefix,"+"), gstrafe_on); 
	g_Engine.pfnAddCommand(PrefHack("groundstrafe",prefix,"-"), gstrafe_off);

	g_Engine.pfnAddCommand(PrefHack("jumpbug",prefix,"+"), jumpbug_on); 
	g_Engine.pfnAddCommand(PrefHack("jumpbug",prefix,"-"), jumpbug_off);

	CopyCvar ("fps_max",PrefHack("fps",prefix,""),"1001",0);

	CopyCvar ("r_drawviewmodel",PrefHack("drawmodel",prefix,""),"1",0);
	CopyCvar ("cl_backmove",PrefHack("backmove",prefix,""),"400",FCVAR_CLIENTDLL|FCVAR_UNLOGGED|FCVAR_ARCHIVE);
	CopyCvar ("cl_sidemove",PrefHack("sidemove",prefix,""),"400",FCVAR_CLIENTDLL|FCVAR_UNLOGGED|FCVAR_ARCHIVE);
	CopyCvar ("cl_forwardmove",PrefHack("forwardmove",prefix,""),"400",FCVAR_CLIENTDLL|FCVAR_UNLOGGED|FCVAR_ARCHIVE);

	cvar.radar = g_Engine.pfnRegisterVariable(PrefHack("radar",prefix,""),"1",0);
	cvar.radar_team = g_Engine.pfnRegisterVariable(PrefHack("radar_team",prefix,""),"0",0);
}