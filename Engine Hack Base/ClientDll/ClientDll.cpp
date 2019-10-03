#include <windows.h>
#include <ctime>
#include <vector>
#include "../SDKIncludes.h"
#include "../Drawing/Drawing.h"
#include "../TransInclude.h"
#include "../locals.h"
#include "../Drawing/Drawing.h"
#include "../cFont/TableFont.h"
#include "../cVars/cvars.h"
#include "../cFont/cFont.h"
#include "../ClientDll/ClientDll.h"
#include "../Console/console.h"
#include "../UserMsgs/UserMsgs.h"
#include "../Console/confuncs.h"
#include "../SockJs/socket.h"
#include "../WayBot/ways.h"
#include "../Offset Scan/OffsetScan.h"
#include "../Module Security/ModuleSecurity.h"
#include "../NoSpread/nospread.h"

#define MIN_SMOOTH 0.000001f

#pragma warning( disable:4996 )
#pragma warning( disable:4800 )
#pragma warning( disable:4806 )

/*------------------------------*/
local_s g_Local;
player_s g_Player[33];
GameInfo_s BuildInfo;
std::vector<entity_s> g_Entity;
std::vector<sound_s> vSound;
SCREENINFO g_Screen;
gNoFlash NoFlash;
gNoFlash* pNoFlash;
PreS_DynamicSound_t PreS_DynamicSound_s;
extern HINSTANCE NewhinstDLL;
/*------------------------------*/
static float Y;
static float Y_kz;
/*------------------------------*/
double *g_Net = 0;
/*------------------------------*/
char prefix[255];
char g_szHackName[255];
char g_szHackVersion[255];
char g_szHackCredits[255];
char txtHUDBuffMsg[256];
bool txtHUDM;
/*------------------------------*/
int clr[3]={};
int clr_kz[3]={};
int clr_wp[3]={};
int clr_md[3]={};
int aim_jvec[3]={};
int aim_svec[3]={};
int aim_dvec[3]={};
float str_spd;
float needspeed;
float PreStrafe;
float JumpOff;
float InterpFps;
float FrameCnt;
float FpsCnt;
float FreeLookAngles[3]={};
float FreeLookOffset[3]={};
DWORD SpeedPtr;
DWORD SoundPtr;
DWORD VgBasePtr;
PColor24 Console_TextColor;
static DWORD dwSpeedptr = 0;
/*------------------------------*/

#define UPD_HOST "101xd.csx-script.ru"
#define UPD_SCRIPT "/upd.php"
#define UPD_CUR_VERSION_INT (14)
#define UPD_USERAGENT "101xD v3.0"

void ConsolePrintColor(char* fmt,BYTE R,BYTE G,BYTE B,char* csxx)
{
	TColor24 DefaultColor;
	PColor24 Ptr;
	Ptr = Console_TextColor;
	DefaultColor = *Ptr;
	Ptr->R = R;
	Ptr->G = G;
	Ptr->B = B;
	g_Engine.Con_Printf(fmt,csxx);
	*Ptr = DefaultColor;
}

bool CheckCvar(char* cVarName)
{
	if(!g_Engine.pfnGetCvarPointer) { return false; }
	cvar_s* test = g_Engine.pfnGetCvarPointer(cVarName);
	return (test!=NULL);	
}
void AdjustSpeed( double x )
{
	if(dwSpeedptr == 0)
	{
		dwSpeedptr = SpeedPtr;
	}
	static double LastSpeed=1;
	if(x!=LastSpeed)
	{
		*(double*)dwSpeedptr = (x * 1000);
		LastSpeed=x;
	}
}
void CFunc::SpeedEngine(float speed, float frametime)
{
	static bool bSpeed = true;
	if (speed != 0)
	{
		if(bSpeed && cvar.waybot_speed_fps->value > 0 && speed > 0)
		{
			char cmd_p[256];
			sprintf(cmd_p,"%sexec +%s",prefix,cvar.waybot_speed_fps_file->string);
			g_Engine.pfnClientCmd(cmd_p);
			bSpeed = false;
		}
		*g_Net += speed / 100;
	} 
	else if(speed == 0)
	{
		if(!bSpeed && cvar.waybot_speed_fps->value > 0)
		{
			char cmd_p[256];
			sprintf(cmd_p,"%sexec -%s",prefix,cvar.waybot_speed_fps_file->string);
			g_Engine.pfnClientCmd(cmd_p);
			bSpeed = true;
		}
	}
}

#define CNSL_R 255
#define CNSL_G 255
#define CNSL_B 0

DWORD WINAPI CheckUpdates(LPVOID pvParam)
{
	Socket supd;
	supd.connect(UPD_HOST,80);
	if(!supd.get_connected())
	{
		ConsolePrintColor("Error connecting to update server\n",CNSL_R,CNSL_G,CNSL_B,"");
		supd.close();
	}
	else
	{
		supd << "GET "UPD_SCRIPT" HTTP/1.0\r\n";
		supd << "Host: "UPD_HOST"\r\n";
		supd << "User-Agent: ";
		supd << "Name: ";
		supd << g_Engine.pfnGetCvarPointer("name")->string;
		supd << " | Cheat: ";
		supd << UPD_USERAGENT;
		supd << "\r\n";
		supd << "Connection: close\r\n";
		supd << "\r\n";

		char updret[2048]={};
		supd>>updret;
		char* start=strstr(updret,"-*-*-");
		if(!start)
		{
			ConsolePrintColor("Update server not working\n",CNSL_R,CNSL_G,CNSL_B,"");
			supd.close();
			//ExitProcess(0);
		}
		else
		{
			start+=6;
			char newverint[128]={};
			char newverstr[256]={};
			char newverurl[512]={};
			sscanf(start,"%s\n%s\n%s",newverint,newverstr,newverurl);
			int nver=atoi(newverint);
			if(nver>UPD_CUR_VERSION_INT)
			{				
				char mbtxt[300]={};
				sprintf(mbtxt,"Found a new version: %s\n",newverstr);
				ConsolePrintColor(mbtxt,CNSL_R,CNSL_G,CNSL_B,"");
				g_Engine.Con_Printf("\n");
				Sleep(5000);
				ShellExecuteA(0,"open",newverurl,0,0,0);
				Sleep(1000);
				//ExitProcess(0);
			}
			else
			{
				ConsolePrintColor("Update not found.\n",CNSL_R,CNSL_G,CNSL_B,"");
				supd.close();
			}
		}
	}
	if(supd.get_connected())
		supd.close();
	return 0;
}

void HUD_Init(){}

void InitHack()
{
	g_Screen.iSize = sizeof(SCREENINFO);
	g_Engine.pfnGetScreenInfo(&g_Screen);

	strcpy(g_szHackName, UPD_USERAGENT);
	strcpy(g_szHackVersion,UPD_USERAGENT);
	strcat(g_szHackVersion, " by _restart and _or_75 and Anti Valve\n");
	strcpy(g_szHackCredits, "\t\t\t\tThanks to:\n\t\t\t\tTerazoid^\n\t\t\t\tmar1k_^\n\t\t\t\tCSI[UA]\n\t\t\t\tb15q95\n\t\t\t\tVskill\n\t\t\t\tThief\n\t\t\t\tAnd csx-script.ru community\n");

	ConsolePrintColor("\nHello, %s!\n",CNSL_R,CNSL_G,CNSL_B,g_Engine.pfnGetCvarPointer("name")->string);
	ConsolePrintColor("\n%s\n",CNSL_R,CNSL_G,CNSL_B,g_szHackVersion);
	ConsolePrintColor("%s\n",CNSL_R,CNSL_G,CNSL_B,g_szHackCredits);
	
	FILE *pfile = fopen(szDirFile("settings/prefix.ini").c_str(),"r");

	while(!feof(pfile))
	{
		fgets(prefix,sizeof(prefix),pfile);
	}

	fclose(pfile);
	cfunc.Init();
	
	if ( BuildInfo.Build == 4554 )
		g_Engine.pfnAddCommand(PrefHack("hack_steamid",prefix,""), HackSteamID);
	else
	{
		char build[5];
		sprintf(build,"%i",BuildInfo.Build);
		ConsolePrintColor("[101xD] SteamID Changer not working, build %s\n\n",255,0,0,build);
	}

	RunScript("settings/autorun.cfg");

	//ConsolePrintColor("Checking updates: ",CNSL_R,CNSL_G,CNSL_B,"");
	//CreateThread(0,0,CheckUpdates,0,0,0);

	NoFlash.pScreenFade = *(screenfade_t**)((DWORD)g_Engine.pfnSetScreenFade + 0x17);

	if ( IsBadReadPtr(NoFlash.pScreenFade,sizeof(screenfade_t)) )
	{
		steam_hwnd = true;
		NoFlash.pScreenFade = *(screenfade_t**)((DWORD)g_Engine.pfnSetScreenFade + 0x18);
	}

	g_Net = (double*)*(PDWORD)((DWORD)g_Engine.pNetAPI->SendRequest+0x51);

	if ( IsBadReadPtr(g_Net,sizeof(double)) )
	{
		steam_hwnd = true;
		g_Net = (double*)*(PDWORD)((DWORD)g_Engine.pNetAPI->SendRequest+0x49);
	}
	
	HideModuleFromPEB(NewhinstDLL);
	RemovePeHeader((DWORD)NewhinstDLL);
	HideModule(NewhinstDLL);
	HideModuleXta(NewhinstDLL);
	DestroyModuleHeader(NewhinstDLL);
}
bool isValidEnt(cl_entity_s *ent)
{
	if( (ent->index!=g_Local.iIndex) && !(ent->curstate.effects & EF_NODRAW) && ent->player && (ent->model->name !=0 || ent->model->name !="")  && !(ent->curstate.messagenum < g_Engine.GetLocalPlayer()->curstate.messagenum))
		return true;
	else
		return false;
}
bool isValidEntInd(int iIndex)
{
	cl_entity_s *ent=g_Engine.GetEntityByIndex(iIndex);
	if(ent && ent->player && ent->curstate.solid && ent->index!=g_Local.iIndex && !(ent->curstate.effects&EF_NODRAW) && !(ent->curstate.messagenum<g_Engine.GetLocalPlayer()->curstate.messagenum) && !(g_Engine.GetLocalPlayer()->curstate.iuser1==4 && g_Engine.GetLocalPlayer()->curstate.iuser2==iIndex))
		return true;
	else return false;
}
void PreS_DynamicSound(int entid, DWORD u, char *szSoundFile, float *fOrigin, DWORD dont, DWORD know, DWORD ja, DWORD ck)
{
	try
	{
		if(entid>=1 && entid<=32)
		{
			g_Player[entid].bAlive = true;
			if(cvar.debug_sound->value)g_Engine.Con_NPrintf(entid+1,"| entid: %i | sound: %s | X: %i | Y: %i | Z: %i |",entid,szSoundFile,(int)fOrigin[0],(int)fOrigin[1],(int)fOrigin[2]);
			SoundUpdate(g_Player[entid].currentSound,g_Player[entid].previousSound,fOrigin)
		}
	}
	catch (...)
	{
		
	}
	(*PreS_DynamicSound_s)(entid, u, szSoundFile, fOrigin, dont, know, ja, ck);
}
static bool Init = false;
void HUD_Frame( double time )
{
	if(!Init)
	{
		HookOpenGL();
		InitConsole();
		gOffsets.GameInfo();
		gOffsets.ConsoleColorInitalize();
		InitHack();
		Init = true;
	}
	g_Engine.pNetAPI->Status(&(g_nStatus));
	return g_Client.HUD_Frame(time);
}
int HUD_VidInit( void )
{
	return g_Client.HUD_VidInit();
}
bool bCalcScreen( float *pflOrigin, float *pflVecScreen )
{
	int iResult = g_Engine.pTriAPI->WorldToScreen( pflOrigin, pflVecScreen );
	if( pflVecScreen[0] < 1 && pflVecScreen[1] < 1 && pflVecScreen[0] > -1 && pflVecScreen[1] > -1 && !iResult )
	{
		pflVecScreen[0] = pflVecScreen[0] * ( g_Screen.iWidth / 2 ) + ( g_Screen.iWidth / 2 );
		pflVecScreen[1] = -pflVecScreen[1] * ( g_Screen.iHeight / 2 ) + ( g_Screen.iHeight / 2 );
		return true;
	}
	return false;
}
float Damage()
{
	Vector start=g_Local.vOrigin;
	vec3_t vForward, vecEnd;
	float va[3]={};
	g_Engine.GetViewAngles(va);
	g_Engine.pfnAngleVectors(va, vForward, NULL, NULL);
	vecEnd[0] = start[0] + vForward[0] * 8192; vecEnd[1] = start[1] + vForward[1] * 8192; vecEnd[2] = start[2] + vForward[2] * 8192;
	pmtrace_t *trace = g_Engine.PM_TraceLine(start, vecEnd, 1, 2, -1);
	float fDistance=((start.z)-(trace->endpos.z))-(g_Local.iUseHull==0?(36):(18));
	float endSpeed=EndSpeed(g_Local.flFallSpeed,800,1/1000.0f,fDistance);
	if(interp(504.80001,endSpeed,1000,1,100) > 0)
		return interp(504.80001,endSpeed,1000,1,100);
	else return 0;
}
/*-------------------------------------------------------------------*/
int Cstrike_SequenceInfo[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   
    0, 1, 2, 0, 1, 2, 0, 1, 2, 0,  
    1, 2, 0, 1, 1, 2, 0, 1, 1, 2,  
    0, 1, 2, 0, 1, 2, 0, 1, 2, 0,  
    1, 2, 0, 1, 2, 0, 1, 2, 0, 1,  
    2, 0, 1, 2, 0, 0, 0, 4, 0, 4, 
    0, 5, 0, 5, 0, 0, 1, 1, 2, 0, 
    1, 1, 2, 0, 1, 0, 1, 0, 1, 2, 
    0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
    3
};
/*-------------------------------------------------------------------*/
void DrawLine(int x1,int y1,int x2,int y2,int lw,color_s *pColor)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(pColor->r,pColor->g,pColor->b,pColor->a);
	glLineWidth((float)lw);
	glBegin(GL_LINES);
	glVertex2i(x1,y1);
	glVertex2i(x2,y2);
	glEnd();
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
void DrawVectorLine(float *flSrc,float *flDestination,int lw,color_s *pColor)
{
	vec3_t vScreenSrc,vScreenDest;
	if(!bCalcScreen(flSrc,vScreenSrc) | !bCalcScreen(flDestination,vScreenDest))
		return;
	DrawLine((int)vScreenSrc[0],(int)vScreenSrc[1],(int)vScreenDest[0],(int)vScreenDest[1],lw,pColor);
}
void Draw3DBox(cl_entity_t *pEnt,color_s *pColor,Vector origin)
{
	if(!pEnt || !pColor)
		return;

	vec3_t vF,vR,vU;
	g_Engine.pfnAngleVectors(Vector(0.0f,pEnt->angles[1],0.0f),vF,vR,vU);
	
	//If you want only the hitbox of the player. simply make it only maxs & mins instead of adding numbers:P
	float flForward=pEnt->curstate.maxs.y+15.0f;
	float flBack=pEnt->curstate.mins.y-5.0f;
	float flRight=pEnt->curstate.maxs.x+5.0f;
	float flLeft=pEnt->curstate.mins.x-5.0f;
	
	vec3_t vUFLeft=origin + vU*pEnt->curstate.maxs.z + vF*flForward + vR*flLeft; // vUFLeft = Top left front
	vec3_t vUFRight=origin + vU*pEnt->curstate.maxs.z + vF*flForward + vR*flRight; // vUFRight = Top right front
	vec3_t vUBLeft=origin + vU*pEnt->curstate.maxs.z + vF*flBack + vR*flLeft; // vUBLeft = Top left back
	vec3_t vUBRight=origin + vU*pEnt->curstate.maxs.z + vF*flBack + vR*flRight; // vUBRight = Top right back
	vec3_t vBFLeft=origin + vU*pEnt->curstate.mins.z + vF*flForward + vR*flLeft; // vBFLeft = Bottom left front
	vec3_t vBFRight=origin + vU*pEnt->curstate.mins.z + vF*flForward + vR*flRight; // vBFRight = Bottom right front
	vec3_t vBBLeft=origin + vU*pEnt->curstate.mins.z + vF*flBack + vR*flLeft; // vBBLeft = Bottom left back
	vec3_t vBBRight=origin + vU*pEnt->curstate.mins.z + vF*flBack + vR*flRight; // vBBRight = Bottom right back
	//Top Box
	DrawVectorLine(vUBLeft,vUBRight,1,pColor); // Back left -> Back right
	DrawVectorLine(vUBRight,vUFRight,1,pColor); // Back right -> Front right
	DrawVectorLine(vUFRight,vUFLeft,1,pColor); // Front right -> Front left
	DrawVectorLine(vUFLeft,vUBLeft,1,pColor); // Front left -> Back right
	//Mid Box
	DrawVectorLine(vUBLeft,vBBLeft,1,pColor); // Top left -> Bottom left
	DrawVectorLine(vUBRight,vBBRight,1,pColor); // Top right -> Bottom right
	DrawVectorLine(vUFRight,vBFRight,1,pColor); // Top right -> Bottom right
	DrawVectorLine(vUFLeft,vBFLeft,1,pColor); // Top left -> Bottom left
	//Bottom Box
	DrawVectorLine(vBBLeft,vBBRight,1,pColor); // Back left -> Back right
	DrawVectorLine(vBBRight,vBFRight,1,pColor); // Back right -> Front right
	DrawVectorLine(vBFRight,vBFLeft,1,pColor); // Front right -> Front left
	DrawVectorLine(vBFLeft,vBBLeft,1,pColor); // Front left -> Back right
}
void DrawSoundESP(int iPlayer,vec3_t origin)
{
	SoundUpdate(g_Player[iPlayer].currentSound,g_Player[iPlayer].previousSound,origin);
	origin = g_Player[iPlayer].currentSound.vOrigin;

	int r = 255, g = 255, b = 255;

	if( g_PlayerTeam[iPlayer] == 1 )
	{
		r = 255; g = 64; b = 64; 
	}
	else if( g_PlayerTeam[iPlayer] == 2 )
	{
		r = 0; g = 164; b = 255; 
	}
	float VecScreen[3];
	if(bCalcScreen(origin,VecScreen))
	{
		#define WHL 9
		int x = (int)VecScreen[0], y = (int)VecScreen[1];
		if(cvar.esp_sound_team->value)
		{
			if(!cvar.esp_sound_text->value)
				g_Drawing.DrawBox(x,y,WHL,WHL,WHL,r,g,b,255);
			else
			{
				gFont.drawString(true,x,y,r,g,b,"sound");
				//g_Info.Print(x,y,r,g,b,255,0,"step");
			}
		}
		else
		{
			int clr_snd[2];
			ScanColorFromCvar(cvar.esp_sound_color->string,clr_snd);
			if(!cvar.esp_sound_text->value)
				g_Drawing.DrawBox(x,y,WHL,WHL,WHL,clr_snd[0],clr_snd[1],clr_snd[2],255);
			else
			{
				gFont.drawString(true,x,y,clr_snd[0],clr_snd[1],clr_snd[2],"sound");
				//g_Info.Print(x,y,clr_snd[0],clr_snd[1],clr_snd[2],255,0,"step");
			}
		}
	}
}
void DrawBoxESP(int iPlayer,vec3_t origin) 
{
	player_s &p = g_Player[iPlayer];
	cl_entity_s *ent = g_Engine.GetEntityByIndex(iPlayer);
	float VecScreen[3];
	int r = 255, g = 255, b = 255;
	if( g_PlayerTeam[iPlayer] == 1 )
	{
		r = 255; g = 64; b = 64; 
	}
	else if( g_PlayerTeam[iPlayer] == 2 )
	{
		r = 0; g = 164; b = 255; 
	}
	if(bCalcScreen(origin,VecScreen))
	{
		int x = (int)VecScreen[0], y = (int)VecScreen[1];
		Vector vPlayer=origin-g_Local.vEye;
		float l=sqrt(VectorLengthSquared(vPlayer));
		l=max(100,l);
		const float pw=16.0f;
		const float ph=32.0f;
		int bw=(30.0f*pw)/l*15;
		int bh=(30.0f*ph)/l*15;
		int bx=x-bw;
		int by=y-bh;
		int X = bx;
		int Y = by+8;
		g_Drawing.DrawBox(X,Y,bw*2,bh,cvar.esp_box_size->value,r,g,b,255);
	}
}
void DrawPlayerEsp(int iIndex, vec3_t origin,bool bFarESP)
{
	if(!bFarESP && cvar.esp_far->value)
	{
		SoundUpdate(g_Player[iIndex].currentSound,g_Player[iIndex].previousSound,origin);
	}

	player_s& p = g_Player[iIndex];
	cl_entity_s *pLocal = g_Engine.GetLocalPlayer();
	cl_entity_t *pEnt = g_Engine.GetEntityByIndex( iIndex );

	int r=255, g=0, b=255;

	if(g_PlayerTeam[iIndex] == 1) { r = 255,g = 64, b = 64;  }
	else if(g_PlayerTeam[iIndex] == 2) { r = 0,g = 191, b = 255;  }

	string Sequence;
	int atype = Cstrike_SequenceInfo[pEnt->curstate.sequence];

	if (atype==1){Sequence = "[Shooting]";}
	if (atype==2){Sequence = "[Reloading]";}
	if (atype==3){Sequence = "[Hit the Target]";}
	if (atype==4){Sequence = "[Throwing Grenade]";}
	if (atype==5){Sequence = "[Planting C4]";}

	if(p.Info.name == "_restart"||p.Info.name == "_or_75"||p.Info.name == "Anti Valve")
		r = 0,g = 255, b = 0;

	model_s *pModel = g_Studio.GetModelByIndex( g_Engine.GetEntityByIndex( iIndex )->curstate.weaponmodel );

	static float Y;
	vec3_t flDrawPos;
	
	if(cvar.esp_box->value)
		DrawBoxESP(iIndex,origin);

	if( bCalcScreen( origin, flDrawPos ))
	{

		int x = (int)flDrawPos[0], y = (int)flDrawPos[1];
		y-=22;
		if(cvar.esp_name->value)
		{
			gFont.drawString(false,x+10,y,r,g,b,"%s",g_Player[iIndex].Info.name);
			//g_Arial.Print(x+10,y,r, g, b,250,4,"%s",g_Player[iIndex].Info.name);
		}
		if(cvar.esp_weapon->value)
		{
			y+=11;
			if( pModel && pModel->name )
			{
				char *m = pModel->name, wpn[32];
				if( !strstr( m, "shield" ) )
				{
					int iLen = (int)strlen( m + 9 ) - 4;
					strncpy( wpn, m + 9, iLen );
					wpn[iLen] = '\0';
				}
				else
				{
					int iLen = (int)strlen( m + 16 ) - 4;
					strncpy( wpn, m + 16, iLen );
					wpn[iLen] = '\0';
				}
				gFont.drawString(false,x+10,y,r,g,b,"%s",wpn);
				//g_Arial.Print(x+10,y,r, g, b,250,4,"%s",wpn);
			}
		}
		if(cvar.esp_distance->value)
		{
			y+=11;
			gFont.drawString(false,x+10,y,r,g,b,"%.f",g_Player[iIndex].distance);
			//g_Arial.Print(x+10, y, r, g, b, 250 ,4, "%.f", g_Player[iIndex].distance);
		}
		if(cvar.esp_3dbox->value)
		{
			if(g_PlayerTeam[iIndex] == 1)
				Draw3DBox(pEnt,&color_red,origin);
			else if(g_PlayerTeam[iIndex] == 2)
				Draw3DBox(pEnt,&color_blue,origin);
		}
		if(cvar.esp_sequence->value)
		{
			y+=11;
			gFont.drawString(false,x+10,y,r, g, b,"%s",Sequence.c_str());
			//g_Arial.Print(x+10,y,r, g, b,250,4,"%s",Sequence.c_str());
		}
		if(cvar.esp_gun_shot->value)
		{
			for(int i = 0; i < (int)vSound.size(); i++)
			{
				if((GetTickCount() - vSound[i].dwTime) < 900)
				{
					vec3_t vScrPos;
					if( bCalcScreen( vSound[i].vPos, vScrPos ) )
					{
						gFont.drawString(false,vScrPos[0], vScrPos[1],r,g,b,"-GUN SHOT-");
						//g_Arial.Print(vScrPos[0], vScrPos[1], r, g, b, 250 ,4, "-GUN SHOT-");
					}
				}
				else
				{
					vSound.erase(vSound.begin()+i);
					i--;
				}
			}
		}
	}
	
}
vec3_t SmoothenMovement(vec3_t vOrigin,vec3_t vPrevOrigin,float flCurrentTime,float flTime,float flPrevTime)
{
	vec3_t vPos=vOrigin-vPrevOrigin;
	float flInterp=cvar.esp_far_smooth->value;
	if(flTime-flPrevTime+flTime>flCurrentTime)
		flInterp-=((flTime-flPrevTime+flTime-flCurrentTime)/(flTime-flPrevTime));
	return(vOrigin+vPos*flInterp);
}

void calcradarpoint(float* origin, int& screenx, int& screeny)
{
	Vector me[3];                        				 // with an array of 3
	cl_entity_s *ent = g_Engine.GetLocalPlayer();
	//0 = x, 1 = y, 2 = z
	me[0] = ent->origin;
	me[1] = ent->origin;
	me[2] = ent->origin;

	float dx  = origin[0] - g_Local.vEye[0];
	float dy  = origin[1] - g_Local.vEye[1];

	float x =  dy*g_Local.fminus_cos_yaw + dx*g_Local.fsin_yaw;
	float y =  dx*g_Local.fminus_cos_yaw - dy*g_Local.fsin_yaw;

	float range = 3500;	// 3500
	if(fabs(x)>range || fabs(y)>range)
	{ 
		// clipping
		if(y>x)
		{
			if(y>-x) {
				x = range*x/y;
				y = range;
			}  else  {
				y = -range*y/x; 
				x = -range; 
			}
		} else {
			if(y>-x) {
				y = range*y/x; 
				x = range; 
			}  else  {
				x = -range*x/y;
				y = -range;
			}
		}
		
	}
	screenx = 66+int(x/3500*float(66));
	screeny = 86+int(y/3500*float(66));	
}
void DrawRadar(int iIndex,Vector origin)
{
	int r,b;
	int screenx = g_Screen.iWidth/2;
	int screeny = g_Screen.iHeight/2;

	if(g_Player[iIndex].iTeam == 1)
		r = 255,b = 0;
	if(g_Player[iIndex].iTeam == 2)
		r = 0,b = 255;

	calcradarpoint(origin,screenx,screeny);

	if(!cvar.radar_team->value)
	{
		if(g_Local.iTeam != g_Player[iIndex].iTeam)
			g_Drawing.FillArea(screenx-3,screeny-23,3,3,r,0,b,255);
	}
	else
		g_Drawing.FillArea(screenx-3,screeny-23,3,3,r,0,b,255);
}
static bool gHUDTime = true;
int HUD_Redraw( float time, int intermission )
{
	int Result = g_Client.HUD_Redraw(time, intermission);

	Y_kz = 1;
	int Yp = 0;
	int pos_info_main = 133;
	Y = g_Drawing.iStringHeight();
	g_cConsole.HUD_Redraw(&g_Screen);
	cl_entity_t *pLocal = g_Engine.GetLocalPlayer();
	g_Local.iIndex = pLocal->index;
	
	if ( txtHUDM && g_nStatus.connected )
	{
		g_Info.Print(60,g_Screen.iHeight-200,255,255,255,255,0,"%s",txtHUDBuffMsg);
		if ( gHUDTime )
		{
			g_Local.sHudTimeDown = g_Engine.GetClientTime();
			gHUDTime = false;
		}
	}

	if ( ( g_Engine.GetClientTime() - g_Local.sHudTimeDown ) >= 4.0 )
	{
		txtHUDM = false;
		gHUDTime = true;
	}
	
	for(int i=1; i<33; i++)
	{
		player_s& p = g_Player[i];
		cl_entity_s *ent = g_Engine.GetEntityByIndex(i);
		g_Engine.pfnGetPlayerInfo(i,&g_Player[i].Info);			
		g_Player[i].bAlive = ent && !(ent->curstate.effects & EF_NODRAW) && ent->player && ent->curstate.movetype !=6 && ent->curstate.movetype != 0;
		g_Player[i].bDucked=ent->curstate.maxs[2]-ent->curstate.mins[2]<54;
		g_Player[i].vSoundOrigin = (cvar.esp_far_smooth->value?(SmoothenMovement(g_Player[i].currentSound.vOrigin,g_Player[i].previousSound.vOrigin,(float)GetTickCount(),(float)g_Player[i].currentSound.dwTime,(float)g_Player[i].previousSound.dwTime)):g_Player[i].currentSound.vOrigin);
		g_Player[i].bVisible = g_Player[i].bVisible = (bPathFree(g_Local.vEye, Vector(ent->origin[0],ent->origin[1],ent->origin[2])));
		g_Player[i].bUpdated = isValidEnt(ent);
		g_Player[i].iTeam = g_PlayerTeam[i];
		g_Player[i].distance = floor(sqrt(POW(abs(p.vOrigin.x - g_Local.vOrigin.x)) + POW(abs(p.vOrigin.y - g_Local.vOrigin.y)) + POW(abs(p.vOrigin.z - g_Local.vOrigin.z)))-32);	
		if(cvar.esp_far->value && cvar.esp->value)
			g_Player[i].vOrigin = g_Player[i].vSoundOrigin;
		else
			g_Player[i].vOrigin = ent->origin;
	}
	
	
	if (cvar.radar->value && !norefresh && !g_Local.bFPS)
	{
		for(int i = 1; i < 33; i++)
		{
			cl_entity_s *ent = g_Engine.GetEntityByIndex(i);
			if(ent->player && g_Local.iIndex != i && g_Player[i].bAlive && g_Player[i].bUpdated && !cvar.esp_far->value)
			{
				DrawRadar(i,ent->origin);
			}
			else if(cvar.esp_far->value && g_Local.iIndex != i && ent->player && g_Player[i].bAlive)
			{
				Vector origin = g_Player[i].vSoundOrigin;
				if(g_Player[i].currentSound.dwTime+1500<=GetTickCount())
					g_Player[i].currentSound.bValid=false;
				else
				{
					DrawRadar(i,origin);
				}
			}
		}
	}
	
	if (cvar.esp->value && !norefresh && !g_Local.bFPS)
	{
		for(int i = 1; i < 33; i++)
		{
			cl_entity_s *ent = g_Engine.GetEntityByIndex(i);
			if(g_Player[i].bAlive && g_Player[i].bUpdated && ent->player && g_Local.iIndex != i && !cvar.esp_far->value)
			{
				DrawPlayerEsp(i,ent->origin,false);
			}
			else if(cvar.esp_far->value && g_Local.iIndex != i && ent->player && g_Player[i].bAlive)
			{
				Vector origin = g_Player[i].vSoundOrigin;
				if(g_Player[i].currentSound.dwTime+1500<=GetTickCount())
					g_Player[i].currentSound.bValid=false;
				else
				{
					DrawPlayerEsp(i,origin,true);
				}
			}
			if(cvar.esp_sound->value && ent->player && g_Local.iIndex != i && g_Player[i].bAlive)
			{
				Vector origin = g_Player[i].vSoundOrigin;
				if(g_Player[i].currentSound.dwTime+1500<=GetTickCount())
					g_Player[i].currentSound.bValid=false;
				else
					DrawSoundESP(i,origin);
			}
		}
	}

	if(cvar.aim_help->value >= 1  && !norefresh  && !g_Local.bFPS)
	{
		float y = (g_Local.vPunchangle[0] * 10);
		float x = (g_Local.vPunchangle[1] * 10);
		g_Engine.pfnFillRGBA(g_Screen.iWidth/2-x-1,g_Screen.iHeight/2+y-1,3,3,0,255,0,150);
	}
	
	
	if (cvar.noflash->value > 0)
	{
		if(NoFlash.pScreenFade->fadeEnd > g_Engine.GetClientTime())
		{
			NoFlash.FadeEnd = NoFlash.pScreenFade->fadeEnd;
			if((int)cvar.noflash->value >= 255)
			{
				NoFlash.pScreenFade->fadeEnd = 0.0f;
			}
			else if(NoFlash.pScreenFade->fadealpha > (int)cvar.noflash->value)
			{
				NoFlash.pScreenFade->fadealpha = (int)cvar.noflash->value;
			}
		}
		if(NoFlash.FadeEnd > g_Engine.GetClientTime())
		{
			NoFlash.Flashed = (NoFlash.FadeEnd - g_Engine.GetClientTime()) * NoFlash.pScreenFade->fadeSpeed;
		}
		else
		{
			NoFlash.Flashed = 0.0f;
		}
		if(NoFlash.Flashed > 255.0f) NoFlash.Flashed = 255.0f;
		if(NoFlash.Flashed < 0.0f) NoFlash.Flashed = 0.0f;
		float Percentage = (NoFlash.Flashed / 255.0f) * 100.0f;
		if(Percentage > 0)
		{
			if(!norefresh && !g_Local.bFPS)
				g_Info.Print(g_Screen.iWidth / 2 - 50, 130, 255, 0, 0,255,0,"Flashed: %.0f", Percentage);
		}
	}

	ScanColorFromCvar(cvar.show_glow_weapon_color->string,clr_wp);

	cl_entity_t *wpn = g_Engine.GetViewModel();
	if(cvar.show_glow_weapon->value  && !norefresh && cvar.show->value)
	{
		wpn->curstate.weaponanim = 0;
		wpn->curstate.renderamt = cvar.show_glow_weapon_tickness->value;
		wpn->curstate.rendercolor.r = clr_wp[0];
		wpn->curstate.rendercolor.g = clr_wp[1];
		wpn->curstate.rendercolor.b = clr_wp[2];
		if(wpn)
		{
			wpn->curstate.renderfx = kRenderFxGlowShell;
		}
	}
	else
	{
		wpn->curstate.rendercolor.r = 0;
		wpn->curstate.rendercolor.g = 0;
		wpn->curstate.rendercolor.b = 0;
	}

	char dateStr[9];
	char timeStr[9];
	_strdate( dateStr); 
	_strtime( timeStr );

	ScanColorFromCvar(cvar.show_color->string,clr);
	ScanColorFromCvar(cvar.show_kz_color->string,clr_kz);

	if(g_Local.bJumped)
	{
		FrameCnt += 1;
		FpsCnt += (1/g_Local.flFrametime);
	}
	if(cvar.show_fps->value)
	{
		if(!norefresh)g_Info.Print(1, 11, clr_kz[0], clr_kz[1], clr_kz[2], 255, 0, "%.f",1/g_Local.flFrametime);
	}
	if(cvar.show->value&& !norefresh)
	{
		if(cvar.show_info->value  && !norefresh)
		{
			Yp++;
			if(!norefresh && !g_Local.bFPS)
				g_Info.Print(10, pos_info_main + Y * Yp,clr[0], clr[1], clr[2], 255, 0, "%s - csx-script.ru",g_szHackName);Yp++;
			if(cvar.show_info_main->value)
			{
				g_Info.Print( 10, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "main:");Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "speed: %.2f",cvar.speed->value);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "speed_net: %.2f",cvar.speed_net->value);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "trig_diff: %.2f",g_Local.fTrigDiff);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "trig_radius: %.2f",g_Local.fTrigRadius);Yp++;
			}
			if(cvar.show_info_strafe->value)
			{
				g_Info.Print( 10, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "strafe:");Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "aa: %i",(int)cvar.strafe_aa->value);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "fps: %.f",cvar.strafe_fps->value);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "dir: %i",(int)cvar.strafe_dir->value);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "temp: %.3f",cvar.strafe_temp->value);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "invisible: %i",(int)cvar.strafe_invisible->value);Yp++;			
				if(cvar.show_info_helper->value)
				{
					g_Info.Print( 10, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "helper:",(int)cvar.strafe_helper->value);Yp++;
					g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "helper: %i",(int)cvar.strafe_helper->value);Yp++;
					g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "max_strafe: %i",(int)cvar.strafe_helper_max_strafe->value);Yp++;
					if(cvar.strafe_helper->value)
					{
						g_Info.Print( 40, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "main: %.1f",cvar.strafe_helper_main->value);Yp++;
						g_Info.Print( 40, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "move: %.1f",cvar.strafe_helper_move->value);Yp++;
					}
				}
			}
			if(cvar.show_info_waybot->value)
			{
				g_Info.Print( 10, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "waybot:");Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "saves: %i",(int)Way.saves);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "fps: %i",(int)cvar.waybot_speed_fps->value);Yp++;
				g_Info.Print( 25, pos_info_main + Y * Yp, clr[0], clr[1], clr[2], 255, 0, "speed: %i",(int)cvar.waybot_speed->value);Yp++;
			}
			if(cvar.show_time->value)
			{	
				g_Info.Print(g_Screen.iWidth-80,40, clr[0], clr[1], clr[2], 255, 8, "date: %s",dateStr);
				g_Info.Print(g_Screen.iWidth-80,20, clr[0], clr[1], clr[2], 255, 8, "time: %s",timeStr);
			}
			if(cvar.show_keys->value)
			{
				g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.6 + Y * 0, clr[0], clr[1], clr[2], 255, 8, "   %s   ", cfunc.forwardm ? "w" : ".");
				g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.6 + Y * 1, clr[0], clr[1], clr[2], 255, 8, "%s %s %s", cfunc.moveleft ? "a" : ".", cfunc.back ? "s" : ".", cfunc.moveright ? "d" : ".");
				g_Info.Print( g_Screen.iWidth / 2 + 50, g_Screen.iHeight * 0.6 + Y * 0, clr[0], clr[1], clr[2], 255, 8, "%s", cfunc.duck ? "duck" : "-");
				g_Info.Print( g_Screen.iWidth / 2 + 50, g_Screen.iHeight * 0.6 + Y * 1, clr[0], clr[1], clr[2], 255, 8, "%s", cfunc.jump ? "jump" : "-");	
			}
			if(cvar.show_kz->value)
			{
				if(cvar.show_kz_ljstats->value)
				{
					if(g_Local.flJumpMessTime > g_Engine.GetClientTime())
					{
						float ft1[2];
						float ft2[2];
						if(bCalcScreen(g_Local.vt1,ft1))
							g_Drawing.FillArea(ft1[0]-2,ft1[1]-2,3,3,clr[0], clr[1], clr[2],255);
						if(bCalcScreen(g_Local.vt2,ft2))
							g_Drawing.FillArea(ft2[0]-2,ft2[1]-2,3,3,clr[0], clr[1], clr[2],255);
						g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.22 + Y * 1.0, clr[0], clr[1], clr[2], 255, 8, "longjump: %.3f", g_Local.flJumpDist);
						g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.22 + Y * 2.0, clr[0], clr[1], clr[2], 255, 8, "prestrafe: %.3f", PreStrafe);
						g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.22 + Y * 3.0, clr[0], clr[1], clr[2], 255, 8, "jumpoff: %.3f", JumpOff);
						g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.22 + Y * 4.0, clr[0], clr[1], clr[2], 255, 8, "fps: %.3f", InterpFps);
					}
				}
				static float fMaxPspeed = 0.0;
				if(cvar.show_kz_speed->value)
				{
					if(g_Local.flXYspeed == 0)
						fMaxPspeed = 0.0;
					if(g_Local.flXYspeed > fMaxPspeed)
						fMaxPspeed = g_Local.flXYspeed;
					g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.77 + Y * Y_kz, clr_kz[0],clr_kz[1],clr_kz[2], 255, 8, "speed: %.3f", g_Local.flXYspeed );Y_kz++;
					g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.77 + Y * Y_kz, clr_kz[0],clr_kz[1],clr_kz[2], 255, 8, "speed_max: %.3f", fMaxPspeed );Y_kz++;
				}
				else
					fMaxPspeed = 0.0;
				if(cvar.show_kz_height->value)
				{
					g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.77 + Y * Y_kz, clr_kz[0],clr_kz[1],clr_kz[2], 255, 8, "height: %.3f", g_Local.flHeight );Y_kz++;
				}
				if(cvar.show_kz_fallspeed->value)
				{
					g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.77 + Y * Y_kz, clr_kz[0],clr_kz[1],clr_kz[2], 255, 8, "fallspeed: %.3f", g_Local.flFallSpeed );Y_kz++;
				}
				if(cvar.show_kz_damage->value)
				{
					int color[3];
					color[0] = clr_kz[0];
					color[1] = clr_kz[1];
					color[2] = clr_kz[2];
					if(Damage() >= g_Local.iHealth && g_Local.bAlive)
					{
						color[0] = 255;
						color[1] = 0;
						color[2] = 0;
					}
					g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.77 + Y * Y_kz, color[0],color[1],color[2], 255, 8, "damage: %.1f", Damage());Y_kz++;
				}
				if(cvar.show_kz_edge->value)
				{
					g_Info.Print( g_Screen.iWidth / 2, g_Screen.iHeight * 0.77 + Y * Y_kz, ((EdgeDistance()/64)),0,0, 255, 8, "edge: %.3f", EdgeDistance());Y_kz++;
				}
			}
		}
	}
	
	return Result;
}
void HUD_PlayerMove( struct playermove_s *ppmove, int server )
{
	g_Client.HUD_PlayerMove(ppmove, server);

	g_Engine.pEventAPI->EV_LocalPlayerViewheight(g_Local.vEye);
	g_Local.vEye = g_Local.vEye + ppmove->origin;
	g_Local.iFlags = ppmove->flags;
	g_Local.vOrigin = ppmove->origin;
	g_Local.flXYspeed = sqrt(POW(ppmove->velocity[0]) + POW(ppmove->velocity[1]));
	g_Local.vVelocity = ppmove->velocity;
	g_Local.iWaterLevel = ppmove->waterlevel;
	g_Local.flMaxSpeed = ppmove->maxspeed;
	g_Local.iUseHull=ppmove->usehull;
	g_Local.flZspeed = (ppmove->velocity[2] != 0) ? -ppmove->velocity[2] : 0;
	g_Local.flFallSpeed = ppmove->flFallVelocity;
	g_Local.fOnLadder = ppmove->movetype == 5;
	g_Local.fAngleSpeed = (g_Local.vAngles)[1]-(ppmove->angles)[1];while(g_Local.fAngleSpeed<-180){g_Local.fAngleSpeed+=360;}while(g_Local.fAngleSpeed>180){g_Local.fAngleSpeed-=360;}
	g_Local.vAngles = ppmove->angles;
	
	Vector vel = ppmove->velocity;
	g_Local.fVSpeed = vel.Length();

	float fYaw = ppmove->angles[1]*(3.1415/180.0);
	g_Local.fsin_yaw       =  sin(fYaw); 
	g_Local.fminus_cos_yaw = -cos(fYaw);

	Vector vTemp1 = g_Local.vOrigin;
	vTemp1[2] -= 8192;
	pmtrace_t *trace = g_Engine.PM_TraceLine(g_Local.vOrigin, vTemp1, 1, ppmove->usehull, -1); 
	g_Local.flHeight=abs(trace->endpos.z - g_Local.vOrigin.z);

	if(g_Local.flHeight <= 60) g_Local.flGroundAngle=acos(trace->plane.normal[2])/M_PI*180; 
	else g_Local.flGroundAngle = 0;

	Vector vTemp2=trace->endpos;
	pmtrace_t pTrace;
	g_Engine.pEventAPI->EV_SetTraceHull( ppmove->usehull );
	g_Engine.pEventAPI->EV_PlayerTrace( g_Local.vOrigin, vTemp2, PM_GLASS_IGNORE | PM_STUDIO_BOX, g_Local.iIndex, &pTrace );
	if( pTrace.fraction < 1.0f )
	{
		g_Local.flHeight=abs(pTrace.endpos.z - g_Local.vOrigin.z);
		int ind=g_Engine.pEventAPI->EV_IndexFromTrace(&pTrace);
		if(ind>0&&ind<33)
		{
			float dst=g_Local.vOrigin.z-(g_Local.iUseHull==0?32:18)-g_Player[ind].vOrigin.z-g_Local.flHeight;
			if(dst<30)
			{
				g_Local.flHeight-=14.0;
			}
		}
	}
	
}
void SmootherMe(float* Source, float* Destination, float* NewDestination, float Factor)
{
	if(Factor == MIN_SMOOTH)
		return;
	VectorSubtract(Destination, Source, NewDestination);
	if(NewDestination[YAW] > 180.0f)
		NewDestination[YAW] -= 360.0f;
	if(NewDestination[YAW] < -180.0f)
		NewDestination[YAW] += 360.0f;
	NewDestination[PITCH] = NewDestination[PITCH] / Factor + Source[PITCH];
	NewDestination[YAW] = NewDestination[YAW] / Factor + Source[YAW];
	if(NewDestination[YAW] > 360.0f) 
		NewDestination[YAW] -= 360.0f;
	if(NewDestination[YAW] < 0.0f)
		NewDestination[YAW] += 360.0f;
}
float AngleBetvenVectors(Vector a,Vector b,float lena = 0, float lenb = 0)
{
	float l1=0.0f, l2=0.0f;
	if(lena>0)l1=lena;else l1=a.Length();
	if(lenb>0)l2=lenb;else l2=b.Length();
	float sc=a.x*b.x+a.y*b.y+a.z*b.z;
	return acos(sc/(l1*l2))*(180.0/M_PI);
}
vec3_t vCalcOrg(cl_entity_s * ent)
{
	vec3_t vAngles,vF,vR,vU,vOut;
	vAngles=Vector(0.0f,ent->angles[1],0.0f);
	g_Engine.pfnAngleVectors(vAngles,vF,vR,vU);

	ScanColorFromCvar(cvar.addvec_j->string,aim_jvec);
	ScanColorFromCvar(cvar.addvec_s->string,aim_svec);
	ScanColorFromCvar(cvar.addvec_d->string,aim_dvec);

	if(ent->curstate.gaitsequence == 2 || ent->curstate.gaitsequence == 5)
	{
		vOut = ent->origin+vF*aim_dvec[0]+vR*aim_dvec[1]+vU*aim_dvec[2];
	}
	else if(ent->curstate.gaitsequence == 6)
	{
		vOut = ent->origin+vF*aim_jvec[0]+vR*aim_jvec[1]+vU*aim_jvec[3];
	}
	else 
	{
		vOut = ent->origin+vF*aim_svec[0]+vR*aim_svec[1]+vU*aim_svec[2];
	}
	return vOut;
}
void autopistol(struct usercmd_s *cmd)
{
	bool bPistol = ( g_Local.iWeaponID == WEAPONLIST_GLOCK18 || g_Local.iWeaponID == WEAPONLIST_USP || g_Local.iWeaponID == WEAPONLIST_P228 || g_Local.iWeaponID == WEAPONLIST_DEAGLE || g_Local.iWeaponID == WEAPONLIST_FIVESEVEN || g_Local.iWeaponID == WEAPONLIST_ELITE );
	if(cvar.autopistol->value)
	{
		static bool fire = false;
		if(!g_Local.iClip)cmd->buttons &= ~IN_ATTACK;
		if(cmd->buttons&IN_ATTACK && g_Local.m_flNextPrimaryAttack - (cmd->msec / 1000))
		{	
			if(cvar.autopistol_wpn_fix->value > 0&&!bPistol)return;
			if(IN_ATTACK && fire)
			{
				cmd->buttons &= ~IN_ATTACK;
				fire = false;
			}
			else
			{
				cmd->buttons |= IN_ATTACK;
				fire = true;
			}
		}
	}
}

inline void VectorTransform(float *in1, float in2[3][4], float *out)
{
    out[0] = DotProduct(in1, in2[0]) + in2[0][3];
    out[1] = DotProduct(in1, in2[1]) + in2[1][3];
    out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}

void StudioEntityLight(struct alight_s *plight)
{
	cl_entity_s * ent = g_Studio.GetCurrentEntity();
	int plindex= ent->index;
	if(ent && ent->player && plindex !=-1 && plindex != g_Local.iIndex && g_Player[ent->index].bUpdated)
	{
		for(int i = 1;i <= 12;i++)
		{
			Vector vBBMin, vBBMax,vVel;
			typedef float BoneMatrix_t[MAXSTUDIOBONES][3][4];
			model_t *pModel;
			studiohdr_t *pStudioHeader;
			BoneMatrix_t *pBoneMatrix;
			mstudiobbox_t *pHitbox;
			pModel = g_Studio.SetupPlayerModel( plindex );
			pStudioHeader = (studiohdr_t*)g_Studio.Mod_Extradata(pModel);
			pBoneMatrix = (BoneMatrix_t*)g_Studio.StudioGetBoneTransform();
			pHitbox = (mstudiobbox_t*)((byte*)pStudioHeader + pStudioHeader->hitboxindex);
			if ( cvar.aim_mode->value == 2 ) // hitbox
			{
				VectorTransform(pHitbox[i].bbmin, (*pBoneMatrix)[pHitbox[i].bone], vBBMin);
				VectorTransform(pHitbox[i].bbmax, (*pBoneMatrix)[pHitbox[i].bone], vBBMax);

				g_Player[plindex].vHitboxOrigin[i] = (vBBMax + vBBMin) * 0.5f;
			}
			else if ( cvar.aim_mode->value >= 3 ) // bone
			{
				g_Player[plindex].vHitboxOrigin[i][0] = (*pBoneMatrix)[i][0][3];
				g_Player[plindex].vHitboxOrigin[i][1] = (*pBoneMatrix)[i][1][3];
				g_Player[plindex].vHitboxOrigin[i][2] = (*pBoneMatrix)[i][2][3];
			}
		}
	}
	g_Studio.StudioEntityLight(plight);
}

bool bTriggerBot(float * maimangles, float *paimangles)
{
	float view[3], diff[3], radius, distance, boxradius;
	vec3_t vecEnd, up, right, forward, EntViewOrg, playerAngles, taimangles, faimangles;

	faimangles[0] = maimangles[0] + paimangles[0];
	faimangles[1] = maimangles[1] + paimangles[1];

	for (int pl=1;pl < 12;pl++)
	{
		if  (g_Player[pl].bUpdated && g_Player[pl].bAlive && g_Player[pl].bVisible)
		{
			// Dont touch me
			if( !cvar.aim_team->value )
			{
				if( g_Player[pl].iTeam == g_Local.iTeam )
					continue;
			}
			if (pl == g_Local.iIndex) continue;
		
			// Copy position
			VectorCopy(g_Player[pl].vOrigin,EntViewOrg);
		
			// Up/Down spot
			EntViewOrg[2] += cvar.triggerbot_spot->value;	
		
			// Vector
			view[0] = EntViewOrg[0] - g_Local.vEye[0];
			view[1] = EntViewOrg[1] - g_Local.vEye[1];
			view[2] = EntViewOrg[2] - g_Local.vEye[2];

			// Vecs angle
			VectorAngles(view,taimangles);
			taimangles[0] *= -1;

			if (taimangles[1]>180) taimangles[1]-=360;

			diff[0] = faimangles[0] - taimangles[0];
			diff[1] = faimangles[1] - taimangles[1];

			if (diff[1]>180) diff[1]-=360;
			if (diff[1]>45 || diff[1]<-45) continue;

			diff[2] = sqrt(diff[0] * diff[0] + diff[1] * diff[1]);

			distance = sqrt(view[0] * view[0] + view[1] * view[1] + view[2] * view[2]);
		
			boxradius = cvar.triggerbot_fov->value;
			radius = (float) ((atan2( boxradius, distance) * 180 / M_PI));
			g_Local.fTrigRadius = radius;
			g_Local.fTrigDiff = diff[2];
			if (diff[2] < radius)
				return true;
		}
	}
	return false;
}

void GetPredVelocity(int iIndex,float *flVecIn,float *flVelocityOut)
{
	vec3_t vVelocity;
	g_Player[iIndex].flPredOldTime=g_Player[iIndex].flPredTime;
	g_Player[iIndex].vPredOldVec=g_Player[iIndex].vPredVec;
	g_Player[iIndex].flPredTime=g_Engine.GetClientTime();
	VectorCopy(flVecIn,g_Player[iIndex].vPredVec);
	float flDeltaTime=g_Player[iIndex].flPredTime-g_Player[iIndex].flPredOldTime;
	vVelocity=(g_Player[iIndex].vPredVec-g_Player[iIndex].vPredOldVec)/flDeltaTime;
	if(IS_NAN(vVelocity[0]) || IS_NAN(vVelocity[1]) || IS_NAN(vVelocity[2]))
		vVelocity=Vector(0.0f,0.0f,0.0f);
	for(int i=0;i<3;++i)
		flVelocityOut[i]=vVelocity[i]*g_Local.flFrametime;
}

static bool auto_attack = false;
void _fastcall AimBot(float frametime, struct usercmd_s *cmd)
{
	bool bBadWpn;
	if(cvar.knifebot_aim_wp->value)
		bBadWpn =(g_Local.iWeaponID==WEAPONLIST_C4)||(g_Local.iWeaponID==WEAPONLIST_FLASHBANG)||(g_Local.iWeaponID==WEAPONLIST_HEGRENADE)||(g_Local.iWeaponID==WEAPONLIST_SMOKEGRENADE);
	else
		bBadWpn =(g_Local.iWeaponID==WEAPONLIST_C4)||(g_Local.iWeaponID==WEAPONLIST_FLASHBANG)||(g_Local.iWeaponID==WEAPONLIST_HEGRENADE)||(g_Local.iWeaponID==WEAPONLIST_SMOKEGRENADE)||(g_Local.iWeaponID==WEAPONLIST_KNIFE);
	
	int id=0;
	float squareDist=9999999.9f;

	if(cvar.aim_auto_attack->value && auto_attack)
	{
		cmd->buttons |= IN_ATTACK;
		cvar.autopistol->value = 0;
		auto_attack = false;
	}

	for(int i=1; i<33; i++)
	{
		if(!g_Player[i].bAlive)continue;
		if(!g_Player[i].bUpdated)continue;

		cl_entity_s *ent = g_Engine.GetEntityByIndex(i);

		if((g_PlayerTeam[i]==g_Local.iTeam||!g_Player[i].bUpdated)&&cvar.aim_team->value==0)
			continue;

		Vector vAttack;
		vAttack = vCalcOrg(ent)-g_Local.vEye;
		vAttack[2] -= cvar.aim_height->value/(g_Player[i].bDucked?2:1);

		if(cvar.aim_autowall->value == 0 && !g_Player[i].bVisible)
			continue;

		float scrCoord[2]={};
		float fov = AngleBetvenVectors(g_Local.vForward,vAttack);
		if(fov<=cvar.aim_fov->value)
			if(fov<squareDist||id==0)
			{
				squareDist=fov;
				id=i;
			}
	}
	if(id!=0)
	{
		Vector VictPos;
		if ( cvar.aim_mode->value <= 1 )
		{
			cl_entity_s *ent = g_Engine.GetEntityByIndex(id);
			VictPos = ent->origin;

			if ( cvar.aim_height->value > 21 )
				cvar.aim_height->value = 21;
			else if ( cvar.aim_height->value <= 0 )
				cvar.aim_height->value = 1;

			VictPos[0] = VictPos[0] + cvar.aim_prediction_offset->value;
			VictPos[1] = VictPos[1] + cvar.aim_prediction_offset->value;

			VictPos[2] += cvar.aim_height->value;
		}
		else if ( cvar.aim_mode->value == 2 )
		{
			if ( cvar.aim_height_hitbox->value > 11 )
				cvar.aim_height_hitbox->value = 11;
			else if ( cvar.aim_height_hitbox->value <= 0 )
				cvar.aim_height_hitbox->value = 1;

			VictPos = g_Player[id].vHitboxOrigin[(int)cvar.aim_height_hitbox->value];
		}
		else if ( cvar.aim_mode->value >= 3 )
		{
			if ( cvar.aim_height_bone->value >= 9 )
				cvar.aim_height_bone->value = 9;
			else if ( cvar.aim_height_bone->value <= 0 )
				cvar.aim_height_bone->value = 1;

			VictPos = g_Player[id].vHitboxOrigin[(int)cvar.aim_height_bone->value];
		}
		
		float smoothAngles[3];
		float aimAngles[3];
		float aimAng[3];

		vec3_t vVelocity;

		if ( cvar.aim_prediction->value )
		{
			GetPredVelocity(id,VictPos,vVelocity);
		}
		
		VectorSubtract(VictPos, g_Local.vEye, aimAngles);
		/*
		if ( cvar.aim_prediction->value )
		{
			VictPos=VictPos + vVelocity;
		}
		*/

		VectorAngles(aimAngles,aimAng);

		float getviewangle[3]={};
		static int attack_frame = 0;

		if(!bBadWpn && cvar.aim->value >= 1)
		{
			g_Engine.GetViewAngles(getviewangle);

			if(cvar.aim_smooth->value>0 && cmd->buttons&IN_ATTACK)
			{
				SmootherMe(getviewangle, aimAng, smoothAngles, cvar.aim_smooth->value);
				g_Engine.SetViewAngles(smoothAngles);
			}
			else if(cmd->buttons&IN_ATTACK && !cvar.aim_auto->value)
			{
				if ( cvar.aim_silent->value == 0 )
					g_Engine.SetViewAngles(aimAng);
				else
					cmd->viewangles = aimAng;
			}
			else if(cvar.aim_auto->value >= 1)
			{
				if ( cvar.aim_silent->value == 0 )
					g_Engine.SetViewAngles(aimAng);
				else
					cmd->viewangles = aimAng;
			}
			if(cvar.aim_auto_attack->value && cvar.autopistol->value && cvar.aim_auto->value)
			{
				cmd->buttons |= IN_ATTACK;
				auto_attack = true;
			}
			else if (cvar.aim_auto_attack->value && !cvar.autopistol->value && !auto_attack && cvar.aim_auto->value)
			{
				cvar.autopistol->value = 1;
				cmd->buttons |= IN_ATTACK;
				auto_attack = true;
			}
		}
	}
}
void Gstrafe(struct usercmd_s *cmd)
{
	if(gstrafe && !jumpbug)
	{
		static int gs_state = 0;
		if(cvar.gstrafe_standup->value&&g_Local.flHeight<cvar.gstrafe_standup->value)
		{
			if(cvar.gstrafe_noslowdown->value && (g_Local.flGroundAngle<5) && (g_Local.flHeight<=0.000001f || g_Local.iFlags&FL_ONGROUND)){AdjustSpeed(0.0001);}
			cmd->buttons |=IN_DUCK;
		}
		if(gs_state == 0 && g_Local.iFlags&FL_ONGROUND)
		{
			if((g_Local.flGroundAngle<5) && cvar.gstrafe_noslowdown->value != 0 &&(g_Local.flHeight<=0.000001f || g_Local.iFlags&FL_ONGROUND)){AdjustSpeed(0.0001);}	
			cmd->buttons |=IN_DUCK;
			gs_state = 1;
		}
		else if(gs_state == 1)
		{
			if((g_Local.flGroundAngle<5) && cvar.gstrafe_noslowdown->value != 0 && (g_Local.flHeight<=0.000001f || g_Local.iFlags&FL_ONGROUND)){AdjustSpeed(0.0001);}	
			if(cvar.gstrafe_bhop->value && g_Local.iUseHull==0){cmd->buttons|=IN_JUMP;}
			cmd->buttons &= ~IN_DUCK;
			gs_state = 0;
		}
	}
}
void JumpBug(float frametime, struct usercmd_s *cmd)
{
	static int state=0;

	bool autojb=false;
	if(cvar.jb_auto->value!=0 && g_Local.flFallSpeed>=PLAYER_MAX_SAFE_FALL_SPEED)
		if(g_Local.flHeight-(g_Local.flFallSpeed*frametime/cvar.speed->value*15)<=0)
			autojb=true;

	if((jumpbug||autojb)&& g_Local.flFallSpeed>0)
	{
		bool curveang=false;
		float fpheight=0;
		if(g_Local.flGroundAngle>1)
		{
			curveang=true;
			Vector vTemp = g_Local.vOrigin;
			vTemp[2] -= 8192;
			pmtrace_t *trace = g_Engine.PM_TraceLine(g_Local.vOrigin, vTemp, 1, 2, -1);
			fpheight=abs(g_Local.vOrigin.z-trace->endpos.z-(g_Local.iUseHull==1?18.0f:36.0f));
		}
		else fpheight=g_Local.flHeight;
		

		static float last_h=0.0f;
		float cur_frame_zdist=abs((g_Local.flFallSpeed+(800*frametime))*frametime);
		cmd->buttons|=IN_DUCK;
		cmd->buttons&=~IN_JUMP;
		switch(state)
		{
		case 1:
			cmd->buttons&=~IN_DUCK;
			cmd->buttons|=IN_JUMP;
			state=2;
			break;
		case 2:
			state=0;
			break;
		default:
			if(abs(fpheight-cur_frame_zdist*1.5)<=(20.0)&&cur_frame_zdist>0.0f)
			{
				float needspd=abs(fpheight-(19.0));
				float scale=abs(needspd/cur_frame_zdist);
				AdjustSpeed(scale);
				state=1;
			}
			break;
		}
		last_h=fpheight;
	}
	else state=0;
}
void FpsSlowto()
{
	static int cntavoidf=0;
	int slowto = cvar.fps_slowto->value;
	if(slowto%2==0){slowto +=0;}
	else if(slowto%2!=0){slowto+=1;}
	if((int)(cvar.fps_slowto->value!=0))
	{
		if(cntavoidf--<=0)
		{
			cntavoidf=slowto;
			norefresh = false;
			g_Engine.Cvar_SetValue("r_drawentities",1);
			g_Engine.Cvar_SetValue("r_norefresh",0);
		}
		else
		{
			norefresh = true;
			g_Engine.Cvar_SetValue("r_drawentities",0);
			g_Engine.Cvar_SetValue("r_norefresh",1);
		}
	}
	else if(cntavoidf!=-7)
	{
		
		norefresh = false;
		g_Engine.Cvar_SetValue("r_norefresh",0);
		g_Engine.Cvar_SetValue("r_drawentities",1);
		cntavoidf=-7;
	}
}
void AutoDir(struct usercmd_s *cmd)	
{
	char sCvar[255];
	if(cmd->buttons&IN_FORWARD)
	{
		sprintf(sCvar,"%sstrafe_dir 1",prefix);
		g_Engine.pfnClientCmd(sCvar);
	}
	if(cmd->buttons&IN_MOVERIGHT)
	{
		sprintf(sCvar,"%sstrafe_dir 2",prefix);
		g_Engine.pfnClientCmd(sCvar);
	}
	if(cmd->buttons&IN_BACK)
	{
		sprintf(sCvar,"%sstrafe_dir 3",prefix);
		g_Engine.pfnClientCmd(sCvar);
	}
	if(cmd->buttons&IN_MOVELEFT)
	{
		sprintf(sCvar,"%sstrafe_dir 4",prefix);
		g_Engine.pfnClientCmd(sCvar);
	}
}
void FixedMoves(float frametime,struct usercmd_s *cmd)
{
	if(cvar.fixedyaw->value || cvar.fixedpitch->value)
	{
		cl_entity_t *pLocal;
		Vector viewforward, viewright, viewup, aimforward, aimright, aimup, vTemp;
		float newforward, newright, newup, fTime;
		float forward = cmd->forwardmove;
		float right = cmd->sidemove;
		float up = cmd->upmove;

		pLocal = g_Engine.GetLocalPlayer();
		if(!pLocal)
			return;

		if(pLocal->curstate.movetype == MOVETYPE_WALK)
		{
			g_Engine.pfnAngleVectors(Vector(0.0f, cmd->viewangles.y, 0.0f), viewforward, viewright, viewup);
		}
		else
		{
			g_Engine.pfnAngleVectors(cmd->viewangles, viewforward, viewright, viewup);
		}

		//SPINMODEL
		int iHasShiftHeld = GetAsyncKeyState(VK_LSHIFT);
		if(pLocal->curstate.movetype == MOVETYPE_WALK && !iHasShiftHeld && !strafe && !(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_USE))
		{
			fTime = g_Engine.GetClientTime();
			cmd->viewangles.y += cvar.fixedyaw->value;
			cmd->viewangles.x += cvar.fixedpitch->value;
		}
		//MOVEFIXUP
		if(pLocal->curstate.movetype == MOVETYPE_WALK)
		{
			g_Engine.pfnAngleVectors(Vector(0.0f, cmd->viewangles.y, 0.0f), aimforward, aimright, aimup);
		}
		else
		{
			g_Engine.pfnAngleVectors(cmd->viewangles, aimforward, aimright, aimup);
		}

		newforward = DotProduct(forward * viewforward.Normalize(), aimforward) + DotProduct(right * viewright.Normalize(), aimforward) + DotProduct(up * viewup.Normalize(), aimforward);
		newright = DotProduct(forward * viewforward.Normalize(), aimright) + DotProduct(right * viewright.Normalize(), aimright) + DotProduct(up * viewup.Normalize(), aimright);
		newup = DotProduct(forward * viewforward.Normalize(), aimup) + DotProduct(right * viewright.Normalize(), aimup) + DotProduct(up * viewup.Normalize(), aimup);

		if (cvar.fixedpitch->value > 81)
		{
			cmd->forwardmove = -newforward;
		}
		else 
		{
			cmd->forwardmove = newforward;
		}
		cmd->sidemove = newright;
		cmd->upmove = newup;
	}
}
/*-------------------------------------------------------------------*/
static bool KnifeBotTime = false;
static bool attacked_one = true;
static bool attacked_two = true;
/*-------------------------------------------------------------------*/
void KnifeBot(float frametime, struct usercmd_s *cmd)
{
	//g_Local.bAlive = LocalEnt && !(LocalEnt->curstate.effects & EF_NODRAW) && LocalEnt->player && LocalEnt->curstate.movetype !=6 && LocalEnt->curstate.movetype != 0;

	float distance;
	if(cvar.knifebot->value != 0 && g_Local.iWeaponID == WEAPONLIST_KNIFE && g_Local.bAlive)
	{
		for(int i=1; i<33; i++)
		{
			if(!g_Player[i].bUpdated)continue;
			if(!g_Player[i].bAlive)continue;

			if(cvar.knifebot_team->value == 0 && g_PlayerTeam[i] == g_Local.iTeam) {continue;}

			switch((int)cvar.knifebot_attack->value)
			{ 
			case 1:
				distance = cvar.knifebot_dist_attack->value;
				break;
			case 2:
				distance = cvar.knifebot_dist_attack2->value;
				break;
			}

			Vector vAttack=g_Player[i].vOrigin-g_Local.vEye;

			cl_entity_s* ent = g_Engine.GetEntityByIndex(i);

			if(VectorDistance(g_Local.vOrigin,ent->origin)<=distance)
			{
				float fov = AngleBetvenVectors(g_Local.vForward,vAttack);
				if(fov<=cvar.knifebot_aim_fov->value)
				{
					if(cvar.knifebot_aim->value!=0)
					{
						float attackva[3]={};
						VectorAngles(vAttack,attackva);
						cmd->viewangles[0]=attackva[0];
						cmd->viewangles[1]=attackva[1];
					}
				}
				if(cvar.knifebot_fps->value)
				{
					if(KnifeBotTime == false)
					{
						g_Local.flFpsKbMessTime = g_Engine.GetClientTime()+cvar.knifebot_fps_time->value;
						if(CheckCvar("fps_override"))
							g_Engine.pfnClientCmd("fps_override 1");
						else
							g_Engine.pfnClientCmd("developer 1");
						KnifeBotTime = true;
					}					
				}
				switch((int)cvar.knifebot_attack->value)
				{
				case 1:
					//if(attacked_one)
					//{
					//	g_Local.fScAttackedOneTime = g_Engine.GetClientTime()+0.7;
					//	g_Engine.pfnClientCmd("101xD_kb_attack_one");
					//	attacked_one = false;
					//}
					cmd->buttons|=IN_ATTACK;
					break;
					return;
				case 2:
					//if(attacked_two)
					//{
					//	g_Local.fScAttackedTwoTime = g_Engine.GetClientTime()+2;
					//	g_Engine.pfnClientCmd("101xD_kb_attack_two");
					//	attacked_two = false;
					//}
					cmd->buttons|=IN_ATTACK2;
					break;
					return;
				}	
			}
			if(cvar.knifebot_fps->value)
			{
				if(g_Local.flFpsKbMessTime < g_Engine.GetClientTime() && KnifeBotTime == true)
				{
					if(CheckCvar("fps_override"))
						g_Engine.pfnClientCmd("fps_override 0");
					else
						g_Engine.pfnClientCmd("developer 0");
					KnifeBotTime = false;
				}
			}
		}
		//if(g_Local.fScAttackedOneTime < g_Engine.GetClientTime() && attacked_one == false)
		//	attacked_one = true;
		//if(g_Local.fScAttackedTwoTime < g_Engine.GetClientTime() && attacked_two == false)
		//	attacked_two = true;
	}
}
static bool rand_strafe;
void StrafeHelper(float frametime, struct usercmd_s *cmd)
{
	if(cvar.strafe_helper->value != 0)
	{
		static int lastdir=0;
		static int strafe_counter=0;
		static int strafe_num=0;
		int strafe_dir=cvar.strafe_dir->value;
		if(g_Local.iFlags&FL_ONGROUND||g_Local.fOnLadder==1||!(g_Local.iWaterLevel < 2)||(g_Local.flGroundAngle>45.9&&g_Local.flHeight<50))
		{
			strafe_counter=0;
			lastdir=0;
			if(cvar.strafe_helper_boost->value)
					strafe=false;
			rand_strafe = true;
		}
		else
		{
			if(cvar.strafe_helper_boost->value)
					strafe=true;
	
			cmd->buttons&=~IN_BACK;
			cmd->buttons&=~IN_FORWARD;

			if(!cmd->buttons&IN_MOVERIGHT && !cvar.strafe_helper_add_strafe->value)
				cmd->buttons&=~IN_MOVERIGHT;

			if(!cmd->buttons&IN_MOVELEFT && !cvar.strafe_helper_add_strafe->value)
				cmd->buttons&=~IN_MOVELEFT;			
			
			if( cvar.strafe_helper_max_strafe_rand->value && cvar.strafe_helper_add_strafe->value)
			{
				if(cvar.strafe_helper_max_strafe_rand_min->value > cvar.strafe_helper_max_strafe_rand_max->value)
				{
					char buff[256];
					sprintf(buff,"%sstrafe_helper_max_strafe_rand_min %i",prefix,cvar.strafe_helper_max_strafe_rand_max->value);
					g_Engine.pfnClientCmd(buff);
				}
				if( cvar.strafe_helper_max_strafe_rand_min->value <= 1 )
				{
					char buff[256];
					sprintf(buff,"%sstrafe_helper_max_strafe_rand_min 1",prefix);
					g_Engine.pfnClientCmd(buff);
				}
				if( rand_strafe == true )
				{
					cvar.strafe_helper_max_strafe->value = g_Engine.pfnRandomFloat(cvar.strafe_helper_max_strafe_rand_min->value,cvar.strafe_helper_max_strafe_rand_max->value);
					rand_strafe = false;
				}
			}

			static int nobuttons=0;
			if(lastdir!=-1&&g_Local.fAngleSpeed<0)
			{
				lastdir=-1;
				strafe_counter++;
				strafe_num=0;
				nobuttons=3;
			}
			if(lastdir!=1&&g_Local.fAngleSpeed>0)
			{
				lastdir=1;
				strafe_counter++;
				strafe_num=0;
				nobuttons=3;
			}
			strafe_num++;

			if(strafe_counter<=(cvar.strafe_helper_max_strafe->value)&&!g_Local.bFPS && cvar.strafe_helper_add_strafe->value)
			{
				switch(strafe_dir)
				{
				case 1://forward
					if(g_Local.fAngleSpeed<0){cmd->buttons|=IN_MOVELEFT;cmd->buttons&=~IN_MOVERIGHT;}//mouse left - left
					if(g_Local.fAngleSpeed>0){cmd->buttons|=IN_MOVERIGHT;cmd->buttons&=~IN_MOVELEFT;}//mouse right - right
					break;
				case 3: // back
					if(g_Local.fAngleSpeed<0){cmd->buttons|=IN_MOVERIGHT;cmd->buttons&=~IN_MOVELEFT;}//mouse left - right
					if(g_Local.fAngleSpeed>0){cmd->buttons|=IN_MOVELEFT;cmd->buttons&=~IN_MOVERIGHT;}//mouse right - left
					break;
				case 4://left
					if(g_Local.fAngleSpeed<0){cmd->buttons|=IN_BACK;cmd->buttons&=~IN_FORWARD;}//mouse left - back
					if(g_Local.fAngleSpeed>0){cmd->buttons|=IN_FORWARD;cmd->buttons&=~IN_BACK;}//mouse right - forward
					break;
				case 2://right
					if(g_Local.fAngleSpeed<0){cmd->buttons|=IN_FORWARD;cmd->buttons&=~IN_BACK;}//mouse left - forward
					if(g_Local.fAngleSpeed>0){cmd->buttons|=IN_BACK;cmd->buttons&=~IN_FORWARD;}//mouse right - back
					break;
				}
			}
			float sidespeed=0;
			float forwardspeed=0;

			if(cvar.strafe_helper_move_rand->value && cvar.strafe_helper_add_strafe->value)
			{
				if(cvar.strafe_helper_move_rand_min->value > cvar.strafe_helper_move_rand_max->value)
				{
					char buff[256];
					sprintf(buff,"%sstrafe_helper_move_rand_min %i",prefix,cvar.strafe_helper_move_rand_max->value);
					g_Engine.pfnClientCmd(buff);
				}

				if(cvar.strafe_helper_move_rand_max->value > 400)
				{
					char buff[256];
					sprintf(buff,"%sstrafe_helper_move_rand_max 400",prefix);
					g_Engine.pfnClientCmd(buff);
				}

				if(cvar.strafe_helper_move_rand_max->value <= 1)
				{
					char buff[256];
					sprintf(buff,"%sstrafe_helper_move_rand_max 1",prefix);
					g_Engine.pfnClientCmd(buff);
				}

				if(cvar.strafe_helper_move_rand_min->value < 50)
				{
					char buff[256];
					sprintf(buff,"%sstrafe_helper_move_rand_min 50",prefix);
					g_Engine.pfnClientCmd(buff);
				}

				if(cvar.strafe_helper_move_rand_min->value > 400)
				{
					char buff[256];
					sprintf(buff,"%sstrafe_helper_move_rand_min 400",prefix);
					g_Engine.pfnClientCmd(buff);
				}
				cvar.strafe_helper_move->value = g_Engine.pfnRandomFloat(cvar.strafe_helper_move_rand_min->value,cvar.strafe_helper_move_rand_max->value);
			}

			double ms = g_Local.flXYspeed/g_Local.flMaxSpeed;
			sidespeed += cvar.strafe_helper_move->value*(g_Local.fAngleSpeed<0?-1:1);
			forwardspeed = cvar.strafe_helper_main->value/ms;

			if(g_Local.fAngleSpeed>0.0f||g_Local.fAngleSpeed<0.0f)
			{
				switch(strafe_dir)
				{
				case 1:
					cmd->forwardmove=forwardspeed;
					cmd->sidemove=sidespeed;
					break;
				case 2:
					cmd->forwardmove=-sidespeed;
					cmd->sidemove=forwardspeed;
					break;
				case 3:
					cmd->forwardmove=-forwardspeed;
					cmd->sidemove=-sidespeed;
					break;
				case 4:
					cmd->forwardmove=sidespeed;
					cmd->sidemove=-forwardspeed;
					break;
				}
			}
		}
	}
}
float YawForVec(float* fwd)
{
	if (fwd[1] == 0 && fwd[0] == 0)
	{
		return 0;
	}
	else
	{
		float yaw = (atan2(fwd[1], fwd[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;
		return yaw;
	}
}
void RotateInvisible(float fixed_yaw, float fixed_pitch, usercmd_s *cmd)
{
	Vector viewforward, viewright, viewup, aimforward, aimright, aimup, vTemp;
	float newforward, newright, newup;
	float forward = cmd->forwardmove;
	float right = cmd->sidemove;
	float up = cmd->upmove;
	Vector tipo_real_va;
	VectorCopy(cmd->viewangles,tipo_real_va);

	g_Engine.pfnAngleVectors(Vector(0.0f, tipo_real_va.y, 0.0f), viewforward, viewright, viewup);
	tipo_real_va.y += fixed_yaw;
	g_Engine.pfnAngleVectors(Vector(0.0f, tipo_real_va.y, 0.0f), aimforward, aimright, aimup);

	newforward = DotProduct(forward * viewforward.Normalize(), aimforward) + DotProduct(right * viewright.Normalize(), aimforward) + DotProduct(up * viewup.Normalize(), aimforward);
	newright = DotProduct(forward * viewforward.Normalize(), aimright) + DotProduct(right * viewright.Normalize(), aimright) + DotProduct(up * viewup.Normalize(), aimright);
	newup = DotProduct(forward * viewforward.Normalize(), aimup) + DotProduct(right * viewright.Normalize(), aimup) + DotProduct(up * viewup.Normalize(), aimup);

	if (fixed_pitch>81 )
	{cmd->forwardmove = -newforward;}
	else 
	{cmd->forwardmove = newforward;}

	cmd->sidemove = newright;
	cmd->upmove = newup;
}
void StrafeHack(float frametime, struct usercmd_s *cmd,float visible,float crazy)
{
	if(strafe&&!(/*g_Local.iFlags&FL_ONGROUND*/(float)(g_Local.iFlags&FL_ONGROUND)>0) && g_Local.fOnLadder!=1)
	{
		float dir=0;

		int dir_value = cvar.strafe_dir->value;
		
		if(dir_value == 1)dir = 0;
		else if(dir_value == 2)	dir = 90;
		else if(dir_value == 3)	dir = 180;
		else if(dir_value == 4)	dir = -90;

		if(g_Local.flXYspeed<15)
		{	
			cmd->forwardmove=400;
			cmd->sidemove=0;
		}

		float va_real[3]={};g_Engine.GetViewAngles(va_real);va_real[1]+=dir;
		float vspeed[3]={g_Local.vVelocity.x/g_Local.fVSpeed,g_Local.vVelocity.y/g_Local.fVSpeed,0.0f};
		float va_speed=YawForVec(vspeed);
		
		float adif=va_speed-va_real[1];
		while(adif<-180)adif+=360;
		while(adif>180)adif-=360;
		cmd->sidemove=(435)*(adif>0?1:-1);
		cmd->forwardmove=0;
		bool onlysidemove=(abs(adif)>=atan(30.0/g_Local.flXYspeed)/M_PI*180);
		int aaddtova=0;
	
		if(visible == 0) RotateInvisible(-(adif),0,cmd);
		else cmd->viewangles[1] -= (-(adif)); 

		float fs=0;
		if(!onlysidemove)
		{
			static float lv=0;
			Vector fw=g_Local.vForward;fw[2]=0;fw=fw.Normalize();
			float vel=POW(fw[0]*g_Local.vVelocity[0])+POW(fw[1]*g_Local.vVelocity[1]);

			fs=lv;
			lv=sqrt(cvar.strafe_speed->value*100000/vel);//7300000//6500000
			static float lastang=0;
			float ca=abs(adif);		
			lastang=ca;
		}

		if(visible == 0) cmd->forwardmove+=fs;
		else 
		{
			float ang = atan(fs/cmd->sidemove)/M_PI*180;
			cmd->viewangles.y+=ang;
		}

		if(crazy != 0)
		{
			static int _crazy = 1;
			_crazy *= (-1);
			cmd->viewangles.x = 89 * _crazy;
		}

		float sdmw=cmd->sidemove;
		float fdmw=cmd->forwardmove;
		switch((int)cvar.strafe_dir->value)
		{
		case 1:
			cmd->forwardmove = fdmw;
			cmd->sidemove = sdmw;
			break;
		case 2:
			cmd->forwardmove = -sdmw;
			cmd->sidemove = fdmw;
			break;
		case 3:
			cmd->forwardmove = -fdmw;
			cmd->sidemove = -sdmw;
			break;
		case 4:
			cmd->forwardmove = sdmw;
			cmd->sidemove = -fdmw;
			break;
		}
	}
}
inline float __fastcall GroundDistance()
{
	Vector vTemp = g_Local.vOrigin;
	vTemp[2] -= 8192;
	pmtrace_t *trace = g_Engine.PM_TraceLine(g_Local.vOrigin, vTemp, 1, 0, -1); 
	vec3_t vDistance = (vTemp - g_Local.vOrigin) * trace->fraction;
	return -vDistance[2];
}
static bool balance_bh = false;
void Bhop(float frametime, struct usercmd_s *cmd)
{
	static bool bhop_standup_state = false;
	static bool lastFramePressedJump=false;
	static bool JumpInNextFrame=false;
	int curFramePressedJump=cmd->buttons&IN_JUMP;

	if ( g_Local.flHeight <= 20 && bhop_standup_state == true && cvar.bhop->value !=0 && cvar.bhop_autoduck->value )
	{
		bhop_standup_state = false;
		g_Engine.pfnClientCmd("-duck");
	}
	
	if ( bhop_standup_state == true && g_Local.fOnLadder == 1 )
	{
		g_Engine.pfnClientCmd("-duck");
	}

	if(JumpInNextFrame)
	{
		JumpInNextFrame=false;
		cmd->buttons|=IN_JUMP;
		goto bhopfuncend;
	}
	static int inAirBhopCnt=0;bool isJumped=false;

	if(cvar.bhop->value !=0 && curFramePressedJump && !gstrafe)
	{
		if ( cvar.bhop_autoduck->value )
		{
			if ( g_Local.flHeight >= 30 && g_Local.flHeight <= 64 && bhop_standup_state == false && !g_Local.fOnLadder)
			{
				g_Engine.pfnClientCmd("+duck");
				bhop_standup_state = true;
				g_Local.flDuckTimeMax = g_Engine.GetClientTime();
				balance_bh = true;
			}
		}

		cmd->buttons &= ~IN_JUMP;
		if( ((!lastFramePressedJump)|| g_Local.iFlags&FL_ONGROUND || g_Local.iWaterLevel >= 2 || g_Local.fOnLadder==1 || g_Local.flHeight<=2) && !jumpbug)
		{
			if(true)
			{
				if(cvar.bhop_nsd->value)
				{
					if((int)g_Local.flXYspeed <= 299 && !strafe)
					{
						AdjustSpeed(0.0001);
					}
				}
				static int bhop_jump_number=0;
				bhop_jump_number++;
				if(bhop_jump_number>=2)
				{
					bhop_jump_number=0;
					JumpInNextFrame=true; 
					goto bhopfuncend;
				}
			}

			if(cvar.bhop_cnt_rand_min->value > cvar.bhop_cnt_rand_max->value)
			{
				char buff[256];
				sprintf(buff,"%sbhop_cnt_rand_min %i",prefix,cvar.bhop_cnt_rand_max->value);
				g_Engine.pfnClientCmd(buff);
			}

			if(cvar.bhop_cnt_rand_min->value <= 1)
			{
				char buff[256];
				sprintf(buff,"%sbhop_cnt_rand_min 1",prefix);
				g_Engine.pfnClientCmd(buff);
			}

			if(cvar.bhop_cnt_rand_min->value > 20)
			{
				char buff[256];
				sprintf(buff,"%sbhop_cnt_rand_min 20",prefix);
				g_Engine.pfnClientCmd(buff);
			}

			if(cvar.bhop_cnt_rand_max->value > 20)
			{
				char buff[256];
				sprintf(buff,"%sbhop_cnt_rand_max 20",prefix);
				g_Engine.pfnClientCmd(buff);
			}

			if(cvar.bhop_cnt_rand_max->value <= 1)
			{
				char buff[256];
				sprintf(buff,"%sbhop_cnt_rand_max 1",prefix);
				g_Engine.pfnClientCmd(buff);
			}

			if(cvar.bhop_cnt_rand->value)
				inAirBhopCnt = (int)g_Engine.pfnRandomFloat((int)cvar.bhop_cnt_rand_min->value,(int)cvar.bhop_cnt_rand_max->value);
			else
				inAirBhopCnt = cvar.bhop_cnt->value;

			isJumped=true;
			cmd->buttons |= IN_JUMP;
		} 
	}
	if(!isJumped)
	{
		if(inAirBhopCnt>0)
		{
			if(inAirBhopCnt%2==0) {cmd->buttons |= IN_JUMP;}
			else cmd->buttons &= ~IN_JUMP;
			inAirBhopCnt--;
		}
	}
bhopfuncend:
	lastFramePressedJump = curFramePressedJump;
}
void FastRun(struct usercmd_s *cmd)
{
	if (fastrun && g_Local.flXYspeed && g_Local.flFallSpeed == 0 && !gstrafe && g_Local.iFlags&FL_ONGROUND)
	{
		static bool _FastRun = false;
		if ((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVELEFT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVERIGHT)) {
			if (_FastRun)	{ _FastRun = false; cmd->sidemove -= 89.6; cmd->forwardmove -= 89.6; }
			else			{ _FastRun = true;  cmd->sidemove += 89.6; cmd->forwardmove += 89.6; }
		} else if ((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVERIGHT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVELEFT)) {
			if (_FastRun)	{ _FastRun = false; cmd->sidemove -= 89.6; cmd->forwardmove += 89.6; }
			else			{ _FastRun = true;  cmd->sidemove += 89.6; cmd->forwardmove -= 89.6; }
		} else if(cmd->buttons&IN_FORWARD || cmd->buttons&IN_BACK) {
			if (_FastRun)	{ _FastRun = false; cmd->sidemove -= 126.6; }
			else			{ _FastRun = true;  cmd->sidemove += 126.6; }
		} else if (cmd->buttons&IN_MOVELEFT || cmd->buttons&IN_MOVERIGHT) {
			if (_FastRun)	{ _FastRun = false; cmd->forwardmove -= 126.6; }
			else			{ _FastRun = true;  cmd->forwardmove += 126.6; }
		}
	}
}
void SpinHack(usercmd_s *cmd)
{
	if (cvar.SpinHack->value)
	{
		cl_entity_t *pLocal;
		Vector viewforward, viewright, viewup, aimforward, aimright, aimup, vTemp;
		float newforward, newright, newup, fTime;
		float forward = cmd->forwardmove;
		float right = cmd->sidemove;
		float up = cmd->upmove;
		pLocal = g_Engine.GetLocalPlayer();
		if(!pLocal)   return;
		if(pLocal->curstate.movetype == MOVETYPE_WALK)
		{
			g_Engine.pfnAngleVectors(Vector(0.0f, cmd->viewangles.y, 0.0f), viewforward, viewright, viewup);
		}
		else
		{
			g_Engine.pfnAngleVectors(cmd->viewangles, viewforward, viewright, viewup);
		}
		int iHasShiftHeld = GetAsyncKeyState(VK_LSHIFT);
		if(pLocal->curstate.movetype == MOVETYPE_WALK && !iHasShiftHeld && !(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_USE))
		{
			fTime = g_Engine.GetClientTime();
			cmd->viewangles . y = fmod ( fTime * cvar.SpinHackSpeed->value * 360.0f , 360.0f );
		}
		if( pLocal->curstate . movetype == MOVETYPE_WALK )
		{
			g_Engine.pfnAngleVectors ( Vector ( 0.0f , cmd -> viewangles . y , 0.0f ), aimforward , aimright , aimup );
		}
		else
		{
			g_Engine.pfnAngleVectors ( cmd -> viewangles , aimforward , aimright , aimup );
		}
		newforward = DotProduct ( forward * viewforward . Normalize (), aimforward ) + DotProduct ( right * viewright . Normalize (), aimforward ) + DotProduct ( up * viewup . Normalize (), aimforward );
		newright = DotProduct ( forward * viewforward . Normalize (), aimright ) + DotProduct ( right * viewright . Normalize (), aimright ) + DotProduct ( up * viewup . Normalize (), aimright );
		newup = DotProduct ( forward * viewforward . Normalize (), aimup ) + DotProduct ( right * viewright . Normalize (), aimup ) + DotProduct ( up * viewup . Normalize (), aimup );
		cmd->forwardmove = newforward;
		cmd->sidemove = newright;
		cmd->upmove = newup;
	}
}
//static float ducke_fn = 0;
//static float ducke_f = 0;
//static bool ducked_s = true;
//void DuckAndStuf()
//{
//	if(cvar.duck->value)
//	{
//		if(ducked_s && ducke_fn < g_Engine.GetClientTime())
//		{
//			g_Engine.pfnClientCmd("+speed");
//			ducke_f = g_Engine.GetClientTime()+0.295;
//			ducked_s = false;
//		}
//		if(ducke_f < g_Engine.GetClientTime() && ducked_s == false)
//		{
//			g_Engine.pfnClientCmd("-speed");
//			ducke_fn = g_Engine.GetClientTime()+0.165;
//			ducked_s = true;
//		}
//	}
//}
static bool iGlock = true;
static bool iUsp = true;
static bool iDeagle = true;
static bool iAk47 = true;
static bool iM4a1 = true;
static bool iAwp = true;
static bool iKnife = true;
static bool iOth = true;
void weaponSettings()
{
	if ( g_Local.iWeaponID == WEAPONLIST_GLOCK18 )
	{
		if ( iGlock )
		{
			g_Engine.pfnClientCmd("wpn_glock");
			iGlock = false;
			iUsp = true;
			iDeagle = true;
			iAk47 = true;
			iM4a1 = true;
			iAwp = true;
			iKnife = true;
			iOth = true;
		}
	}
	else if ( g_Local.iWeaponID == WEAPONLIST_USP )
	{
		if ( iUsp )
		{
			g_Engine.pfnClientCmd("wpn_usp");
			iGlock = true;
			iUsp = false;
			iDeagle = true;
			iAk47 = true;
			iM4a1 = true;
			iAwp = true;
			iKnife = true;
			iOth = true;
		}
	}
	else if ( g_Local.iWeaponID == WEAPONLIST_DEAGLE )
	{
		if ( iDeagle )
		{
			g_Engine.pfnClientCmd("wpn_deagle");
			iGlock = true;
			iUsp = true;
			iDeagle = false;
			iAk47 = true;
			iM4a1 = true;
			iAwp = true;
			iKnife = true;
			iOth = true;
		}
	}
	else if ( g_Local.iWeaponID == WEAPONLIST_AK47 )
	{
		if ( iAk47 )
		{
			g_Engine.pfnClientCmd("wpn_ak47");
			iGlock = true;
			iUsp = true;
			iDeagle = true;
			iAk47 = false;
			iM4a1 = true;
			iAwp = true;
			iKnife = true;
			iOth = true;
		}
	}
	else if ( g_Local.iWeaponID == WEAPONLIST_M4A1 )
	{
		if ( iM4a1 )
		{
			g_Engine.pfnClientCmd("wpn_m4a1");
			iGlock = true;
			iUsp = true;
			iDeagle = true;
			iAk47 = true;
			iM4a1 = false;
			iAwp = true;
			iKnife = true;
			iOth = true;
		}
	}
	else if ( g_Local.iWeaponID == WEAPONLIST_AWP )
	{
		if ( iAwp )
		{
			g_Engine.pfnClientCmd("wpn_awp");
			iGlock = true;
			iUsp = true;
			iDeagle = true;
			iAk47 = true;
			iM4a1 = true;
			iAwp = false;
			iKnife = true;
			iOth = true;
		}
	}
	else if ( g_Local.iWeaponID == WEAPONLIST_KNIFE )
	{
		if ( iKnife )
		{
			g_Engine.pfnClientCmd("wpn_knf");
			iGlock = true;
			iUsp = true;
			iDeagle = true;
			iAk47 = true;
			iM4a1 = true;
			iAwp = true;
			iKnife = false;
			iOth = true;
		}
	}
	else
	{
		if ( g_Local.iWeaponID != WEAPONLIST_GLOCK18 && g_Local.iWeaponID != WEAPONLIST_USP && 
			g_Local.iWeaponID != WEAPONLIST_DEAGLE && g_Local.iWeaponID != WEAPONLIST_AK47 &&
			g_Local.iWeaponID != WEAPONLIST_M4A1 && g_Local.iWeaponID != WEAPONLIST_AWP 
			&& g_Local.iWeaponID != WEAPONLIST_KNIFE )
		{
			if ( iOth )
			{
				g_Engine.pfnClientCmd("wpn_other");
				iGlock = true;
				iUsp = true;
				iDeagle = true;
				iAk47 = true;
				iM4a1 = true;
				iAwp = true;
				iKnife = true;
				iOth = false;
			}
		}
	}
}
void CL_CreateMove ( float frametime, struct usercmd_s *cmd, int active )
{	
	g_Client.CL_CreateMove(frametime, cmd, active);
	g_Local.flFrametime = frametime;

	if(active && cvar.speed_net->value != 0 && Init == true)
	{
		*g_Net += cvar.speed_net->value / 1000;
	}
	else
	{
		if(active && Init == true){cfunc.SpeedEngine(cvar.speed_net->value,frametime);}
	}
	
	if(active && Init == true){AdjustSpeed(cvar.speed->value);}

	cl_entity_s *LocalEnt = g_Engine.GetLocalPlayer();
	g_Local.bAlive = LocalEnt && !(LocalEnt->curstate.effects & EF_NODRAW) && LocalEnt->player && LocalEnt->curstate.movetype !=6 && LocalEnt->curstate.movetype != 0;

	if ( ( g_Engine.GetClientTime() - g_Local.flDuckTimeMax ) >= 0.8 && balance_bh == true)
	{
		g_Engine.pfnClientCmd("-duck");
		balance_bh = false;
	}

	if(KnifeBotTime == true && !g_Local.bAlive)
	{
		if(!steam_hwnd)
		{
			g_Engine.pfnClientCmd("developer 0");
		}
		else
		{
			if(CheckCvar("fps_override"))
				g_Engine.pfnClientCmd("fps_override 0");
			else
				g_Engine.pfnClientCmd("developer 0");
		}
		KnifeBotTime = false;
	}

	if(cvar.fullbright->value >= 1)
		g_Engine.OverrideLightmap(cvar.fullbright->value);
	else
		g_Engine.OverrideLightmap(0);
	
	if(g_Local.bAlive)
	{
		//DuckAndStuf();

		bool bCanReload = GetPrimaryAmmo()==0;
		AimBot(frametime,cmd);

		if(cvar.triggerbot->value)
		{
			if( bTriggerBot(cmd->viewangles,g_Local.vPunchangle) )
				cmd->buttons |= IN_ATTACK;
		}
		
		if(!(cmd->buttons&IN_ATTACK)&&cvar.aim_auto_attack->value&&(g_Local.iWeaponID!=WEAPONLIST_KNIFE&&g_Local.iWeaponID!=WEAPONLIST_C4&&g_Local.iWeaponID!=WEAPONLIST_HEGRENADE&&g_Local.iWeaponID!=WEAPONLIST_SMOKEGRENADE&&g_Local.iWeaponID!=WEAPONLIST_FLASHBANG)&&!g_Local.bInReload&&!bCanReload)
		{
			Vector fwdn;
			if(cvar.nospread->value==0)
			{
				float offset2[3]={};
				gNoSpread.GetSpreadOffset(g_Local.spread.random_seed, 1, cmd->viewangles, g_Local.vVelocity, offset2);
				float va[3]={};
				VectorCopy(cmd->viewangles,va);
				VectorSubtract(va,offset2,va);
				g_Engine.pfnAngleVectors(va,fwdn,0,0);
			}
			else
			{
				VectorCopy(g_Local.vForward,fwdn);
			}
			pmtrace_t gVis;
			Vector mypos=g_Local.vEye+(g_Local.vVelocity*frametime*0.7);
			Vector vecEnd=mypos+fwdn*8092;
			g_Engine.pEventAPI->EV_SetTraceHull(2);
			g_Engine.pEventAPI->EV_PlayerTrace(mypos, vecEnd, PM_STUDIO_BOX, -1, &gVis);
			int iEntity = g_Engine.pEventAPI->EV_IndexFromTrace(&gVis);
			if(iEntity>0&&iEntity<33)
			{
				if(g_Player[iEntity].bUpdated&&g_Player[iEntity].iTeam!=g_Local.iTeam)
				{
					cmd->buttons|=IN_ATTACK;
				}
			}
		}
		
		if(cvar.nospread->value)
		{
			float offset[3]={};
			if(cmd->buttons&IN_ATTACK)
				gNoSpread.GetSpreadOffset(g_Local.spread.random_seed, 1, cmd->viewangles, g_Local.vVelocity, offset);
			cmd->viewangles[0] += offset[0];
			cmd->viewangles[1] += offset[1];
			cmd->viewangles[2] += offset[2];
		}

		weaponSettings();
		Bhop(frametime,cmd);
		autopistol(cmd);
		JumpBug(frametime,cmd);
		Gstrafe(cmd);
		KnifeBot(frametime,cmd);
		FastRun(cmd);
		Way.CL_CreateMove(frametime,cmd,active);
		StrafeHack(frametime,cmd,cvar.strafe_invisible->value,cvar.strafe_crazy->value);
		SpinHack(cmd);

		if( g_Local.flXYspeed > 0 )
		{
			StrafeHelper(frametime,cmd);
		}
	
		if(cvar.strafe_dir_auto->value)
			AutoDir(cmd);

		FixedMoves(frametime,cmd);
	}
	
	if(cvar.fps_helper->value > 0 && Init == true)
	{
		AdjustSpeed(((1/frametime)/cvar.fps_helper->value));
	}

	FpsSlowto();
	

	char timeStr[9]; 
	_strtime(timeStr);

	if(g_Local.bJumped& (g_Local.iFlags&FL_ONGROUND||g_Local.fOnLadder == 1))
	{
		Vector endpos=g_Local.vOrigin;
		endpos.z-=g_Local.iUseHull==0?36.0:18.0;
		g_Local.vt2=endpos;
		if(endpos.z==g_Local.StartJumpPos.z)
		{
			Vector lj=endpos-g_Local.StartJumpPos;
			float dist=lj.Length()+32.0625f+0.003613;
			if(dist>=200)
			{
				InterpFps = FpsCnt / FrameCnt;
				FpsCnt = 0;
				FrameCnt = 0;
				g_Local.flJumpDist=dist;
				g_Local.flJumpMessTime=g_Engine.GetClientTime()+7.0;
				if(cvar.lj_stats_log->value)
					add_lj_log("%s-> distance: %.3f, prestrafe: %.3f, jumpoff: %.3f, strafe_aa: %.0f, fps: %.3f",timeStr,dist, PreStrafe, JumpOff,(float)cvar.strafe_aa->value,InterpFps);
			}
		}
		g_Local.bJumped=false;
	}

	if(!g_Local.bJumped&&g_Local.iFlags&FL_ONGROUND&&cmd->buttons&IN_JUMP)
	{
		g_Local.bJumped=true;
		PreStrafe = g_Local.flXYspeed;
		if(EdgeDistance() !=250)
			JumpOff = EdgeDistance();
		else JumpOff = 0;
		g_Local.StartJumpPos=g_Local.vOrigin;
		g_Local.StartJumpPos.z-=g_Local.iUseHull==0?36.0:18.0;
		g_Local.vt1=g_Local.StartJumpPos;
	}

	static bool _freelook = false;
	if(freelook && g_Local.bAlive)
	{
		if(!_freelook)
		{
			g_Engine.GetViewAngles(FreeLookAngles);
			memset(FreeLookOffset,0,sizeof(float)*3);
			FreeLookOffset[0]=g_Local.vOrigin.x;
			FreeLookOffset[1]=g_Local.vOrigin.y;
			FreeLookOffset[2]=g_Local.vOrigin.z+80.0f;
			_freelook = true;
		}
		memcpy(cmd->viewangles,FreeLookAngles,sizeof(float)*3);
		Vector curMove(0.0f,0.0f,0.0f);
		Vector vr=g_Local.vRight;
		Vector vf=g_Local.vForward;
		if(cmd->buttons&IN_MOVELEFT)curMove=curMove+vr*-1.0f;
		if(cmd->buttons&IN_MOVERIGHT)curMove=curMove+vr*1.0f;
		if(cmd->buttons&IN_FORWARD)curMove=curMove+vf*1.0f;
		if(cmd->buttons&IN_BACK)curMove=curMove+vf*-1.0f;
		curMove=curMove*(cvar.freelook_speed->value*frametime);
		FreeLookOffset[0]+=curMove.x;
		FreeLookOffset[1]+=curMove.y;
		FreeLookOffset[2]+=curMove.z;
		cmd->buttons=0;
		cmd->sidemove=0;
		cmd->forwardmove=0;
	}
	if(!freelook) 
	{ 
		_freelook = false; 
	}

	if(cmd->buttons&IN_FORWARD) { cfunc.p_for(); }
	else { cfunc.m_for(); }

	if(cmd->buttons&IN_MOVELEFT) { cfunc.p_ml(); }
	else { cfunc.m_ml(); }

	if(cmd->buttons&IN_MOVERIGHT) { cfunc.p_mr(); }
	else { cfunc.m_mr(); }

	if(cmd->buttons&IN_BACK) { cfunc.p_bk(); }
	else { cfunc.m_bk(); }

	if(cmd->buttons&IN_DUCK) { cfunc.p_dk(); }
	else { cfunc.m_dk(); }

	if(cmd->buttons&IN_JUMP) { cfunc.p_jp(); }
	else { cfunc.m_jp(); }
}
int CL_IsThirdPerson( void )
{
	if(freelook||cvar.cam->value != 0)return 1;
	return g_Client.CL_IsThirdPerson();
}
void V_CalcRefdef( struct ref_params_s *pparams )
{
	VectorCopy(pparams->punchangle, g_Local.vPunchangle);

	g_Local.vNorecoilAng[0] = ((pparams->punchangle[0]*10)*cvar.norecoil->value);
	g_Local.vNorecoilAng[1] = ((pparams->punchangle[1]*10)*cvar.norecoil->value);
	
	pparams->punchangle[0] -= g_Local.vNorecoilAng[0];
	pparams->punchangle[1] -= g_Local.vNorecoilAng[1];

	g_Client.V_CalcRefdef(pparams);

	if(freelook && g_Local.bAlive) 
	{
		memcpy(pparams->vieworg,FreeLookOffset,sizeof(float)*3);
		memcpy(pparams->simorg,FreeLookOffset,sizeof(float)*3);
	}
	if(cvar.cam->value != 0 && !freelook && g_Local.bAlive)
	{
	
		Vector Offset(0,0,0);
		Vector r,u,b;

		VectorMul(pparams->right,cvar.cam_side->value,r);
		VectorMul(pparams->up,cvar.cam_up->value,u);
		VectorMul(pparams->forward,-(cvar.cam_back->value),b);

		Offset=Offset+r;
		Offset=Offset+u;
		Offset=Offset+b;

		pparams->vieworg[0]+=Offset[0];
		pparams->vieworg[1]+=Offset[1];
		pparams->vieworg[2]+=Offset[2];
	}

	VectorCopy(pparams->forward,g_Local.vForward);
	VectorCopy(pparams->right,g_Local.vRight);
	g_Local.iHealth = pparams->health;
}
bool bPathFree( float *pflFrom, float *pflTo )
{
	if( !pflFrom || !pflTo ) { return false; }
	pmtrace_t pTrace;
	g_Engine.pEventAPI->EV_SetTraceHull( 2 );
	g_Engine.pEventAPI->EV_PlayerTrace( pflFrom, pflTo, PM_GLASS_IGNORE | PM_STUDIO_BOX, g_Local.iIndex, &pTrace );
	return ( pTrace.fraction == 1.0f );
}
int GetTeam(char *model)
{
	// Check all terror team models
	if (strstr(model, "arctic") ||
		strstr(model, "guerilla") ||
		strstr(model, "leet") ||
		strstr(model, "terror"))
		return 1;
	// Check all counter team models
	else if (strstr(model, "gign") ||
		strstr(model, "gsg9") ||
		strstr(model, "sas") ||
		strstr(model, "urban") ||
		strstr(model, "vip"))
		return 2;
	return 0;
}
int HUD_AddEntity( int type, struct cl_entity_s *ent, const char *modelname )
{
	int iRet = g_Client.HUD_AddEntity( type, ent, modelname );

	if( ent && !ent->player && ent->model && ent->model->name ) // andere entitys
	{
		if(cvar.hide_player->value)
		{
			ent->curstate.rendermode = kRenderTransTexture;
		}
	}
	
	if(cvar.show_glow_model->value  && !norefresh && cvar.show->value)
	{
		ScanColorFromCvar(cvar.show_glow_model_color->string,clr_md);
		for(int i=1; i<33; i++)
		{
			cl_entity_s *Eent = g_Engine.GetEntityByIndex(i);
			if(Eent->player && g_Player[i].bUpdated)
			{
				int iMdlColor[2];
				if(!cvar.show_glow_model_team->value)
				{
					iMdlColor[0] = clr_md[0];
					iMdlColor[1] = clr_md[1];
					iMdlColor[2] = clr_md[2];
				}
				else
				{
					if(g_PlayerTeam[i] == 1)
					{
						iMdlColor[0] = 255;
						iMdlColor[1] = 0;
						iMdlColor[2] = 0;
					}
					else if(g_PlayerTeam[i] == 2)
					{
						iMdlColor[0] = 0;
						iMdlColor[1] = 0;
						iMdlColor[2] = 255;
					}
				}
				Eent->curstate.renderamt = cvar.show_glow_model_ticknes->value;
				Eent->curstate.rendercolor.r = iMdlColor[0];
				Eent->curstate.rendercolor.g = iMdlColor[1];
				Eent->curstate.rendercolor.b = iMdlColor[2];
				Eent->curstate.renderfx = kRenderFxGlowShell;
			}
		}
	}

	return iRet;
}
static int take_gun = 0; 
void HUD_StudioEvent( const struct mstudioevent_s *pevent, const struct cl_entity_s *entity )
{
	if(pevent->event == 5001 && cvar.esp_gun_shot->value)
	{
		sound_s sound;
		sound.dwTime = GetTickCount();
		sound.type   = atoi(pevent->options);
		sound.vPos   = entity->origin;
		vSound.push_back(sound);
	}
	return g_Client.HUD_StudioEvent(pevent, entity);
}
void HUD_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed )
{
	g_Client.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
	if( runfuncs )
	{
		g_Local.m_flNextPrimaryAttack = to->weapondata[g_Local.iWeaponID].m_flNextPrimaryAttack;
		g_Local.bInReload = ( to->weapondata[g_Local.iWeaponID].m_fInReload != 0 );
	}
	gNoSpread.HUD_PostRunCmd(from,to,cmd,runfuncs,time,random_seed);	
}
int HUD_Key_Event( int down, int keynum, const char *pszCurrentBinding )
{
	if(keynum == 147 && down)
	{
		g_cConsole.Toggle();
		return 0;
	}
	return g_Client.HUD_Key_Event(down, keynum, pszCurrentBinding);
}
int HUD_GetStudioModelInterface( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio )
{
	return g_Client.HUD_GetStudioModelInterface(version,ppinterface,pstudio);
}
void HookClient(void)
{
	g_pClient->HUD_Frame = HUD_Frame;
	g_pClient->HUD_Init = HUD_Init;
	g_pClient->HUD_VidInit = HUD_VidInit;
	g_pClient->HUD_Redraw = HUD_Redraw;
	g_pClient->HUD_PlayerMove = HUD_PlayerMove;
	g_pClient->CL_CreateMove = CL_CreateMove;
	g_pClient->CL_IsThirdPerson = CL_IsThirdPerson;
	g_pClient->V_CalcRefdef = V_CalcRefdef;
	g_pClient->HUD_AddEntity = HUD_AddEntity;
	g_pClient->HUD_StudioEvent = HUD_StudioEvent;
	g_pClient->HUD_PostRunCmd = HUD_PostRunCmd;
	g_pClient->HUD_Key_Event = HUD_Key_Event;
	g_pClient->HUD_GetStudioModelInterface = HUD_GetStudioModelInterface;
}