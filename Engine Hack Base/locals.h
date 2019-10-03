#pragma once

#include <Windows.h>
#include <vector>
#include <iostream>
#include <time.h>

using namespace std;

#pragma warning(disable:4996)

extern char prefix[255];
extern char g_szHackName[255];
extern char g_szHackVersion[255];
extern char g_szHackCredits[255];
extern int g_PlayerTeam[33];
extern char txtHUDBuffMsg[256];
extern bool txtHUDM;

struct spread_info
{
	unsigned int random_seed;
	int recoil;
	float gtime;
	float prevtime;
	float brokentime; // This is only supposed to be set to zero when you buy the sg550 or the g3sg1
						// not when you reload, switch weapons or pick up a weapon, this is do to the
						// cs bugs for these guns (valve fix your code please)
	float spreadvar;
	float recoiltime;
	bool firing;
	int WeaponState;
	int prcflags;
};

struct GameInfo_s
{
	char* GameName;
	char* GameVersion;
	BYTE Protocol;
	int Build;
};
extern GameInfo_s BuildInfo;

struct local_s
{
	int prcflags;

	int iTeam;
	int iClip;
	int iWeaponID;
	int iIndex;
	int iWaterLevel;
	int iHealth;
	int iFlags;
	int iUseHull;
	int iFOV;

	float NospreadOffset;
	float m_flNextPrimaryAttack;

	float fOnLadder;
	float flHeight;
	float flGroundAngle;
	float flFpsKbMessTime;
	float flDuckTimeMax;
	float flFrametime;
	float flJumpMessTime;
	float flFallSpeed;
	float flZspeed;
	float flJumpDist;
	float flXYspeed;
	float flMaxSpeed;
	float fAngleSpeed;
	float fsin_yaw;
	float fminus_cos_yaw;
	float sHudTimeDown;
	float fVSpeed;

	bool bAlive;
	bool bFPS;
	bool bJumped;
	bool bInReload;

	Vector vRight;
	Vector vForward;
	Vector vEye;
	Vector vt1,vt2;
	Vector vOrigin;
	Vector vNorecoilAng;
	Vector vPunchangle;
	Vector vVelocity;
	Vector vAngles;
	Vector StartJumpPos;
	Vector pmEyePos;



	float fTrigRadius;
	float fTrigDiff;

	spread_info spread;
};
extern local_s g_Local;

struct player_sound_t
{
	vec3_t vOrigin;
	DWORD dwTime;
	bool bValid;
};
struct player_s
{
	bool bAlive;
	bool bUpdated;
	bool bDucked;
	bool bVisible;
	int iTeam;
	float distance;
	float fldmFixTime;
	vec3_t vHitboxOrigin[12];
	vec3_t vOrigin;
	vec3_t vSoundOrigin;
	player_sound_t currentSound, previousSound;
	hud_player_info_t Info;
	// prediction
	vec3_t vPredVec,vPredOldVec;
	float flPredTime,flPredOldTime;
};
extern player_s g_Player[33];

struct entity_s
{
	char szName[32];
	int iImportant;
	vec3_t vOrigin;
};
extern std::vector<entity_s> g_Entity;

struct sound_s
{
	DWORD dwTime;
	int type;
	Vector vPos;
};
extern std::vector<sound_s> vSound;

struct gNoFlash
{
public:
	float Flashed;
	float FadeEnd;
	screenfade_t* pScreenFade;
};

struct Color24
{
	BYTE R,G,B;
};

typedef Color24 TColor24;
typedef Color24 *PColor24;

extern PColor24 Console_TextColor;

#define PLAYER_FATAL_FALL_SPEED		980
#define PLAYER_MAX_SAFE_FALL_SPEED	504.8

#define SoundUpdate(a,b,origin) {b=a; a.vOrigin=origin; a.bValid=true; a.dwTime=GetTickCount();}
#define SoundClear(a) {a.vOrigin[0]=0; a.vOrigin[1]=0; a.vOrigin[2]=0; a.dwTime=0; a.bValid=false;}

#define POW(x) ((x)*(x))
#define VectorLengthSquared(v) ((v)[0]*(v)[0]+(v)[1]*(v)[1]+(v)[2]*(v)[2])
#define VectorDistance(a,b) sqrt(POW((a)[0]-(b)[0])+POW((a)[1]-(b)[1])+POW((a)[2]-(b)[2]))
#define VectorLength(a) sqrt(POW((a)[0])+POW((a)[1])+POW((a)[2]))
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorMul(vec,num,res){(res)[0]=(vec)[0]*(num);(res)[1]=(vec)[1]*(num);(res)[2]=(vec)[2]*(num);}
#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define IS_NAN(x)(((*(int*)&x)&(255<<23))==(255<<23))

#define WEAPONLIST_P228			1
#define	WEAPONLIST_UNKNOWN1		2
#define	WEAPONLIST_SCOUT		3
#define	WEAPONLIST_HEGRENADE	4
#define	WEAPONLIST_XM1014		5
#define	WEAPONLIST_C4			6
#define	WEAPONLIST_MAC10		7
#define	WEAPONLIST_AUG			8
#define	WEAPONLIST_SMOKEGRENADE	9
#define	WEAPONLIST_ELITE		10
#define	WEAPONLIST_FIVESEVEN	11
#define	WEAPONLIST_UMP45		12
#define	WEAPONLIST_SG550		13
#define	WEAPONLIST_GALIL		14
#define	WEAPONLIST_FAMAS		15
#define	WEAPONLIST_USP			16
#define	WEAPONLIST_GLOCK18		17
#define	WEAPONLIST_AWP			18
#define	WEAPONLIST_MP5			19
#define	WEAPONLIST_M249			20
#define	WEAPONLIST_M3			21
#define	WEAPONLIST_M4A1			22
#define	WEAPONLIST_TMP			23
#define	WEAPONLIST_G3SG1		24
#define	WEAPONLIST_FLASHBANG	25
#define	WEAPONLIST_DEAGLE		26
#define	WEAPONLIST_SG552		27
#define	WEAPONLIST_AK47			28
#define	WEAPONLIST_KNIFE		29
#define	WEAPONLIST_P90			30

string szDirFile( char* pszName );

inline void _fastcall VectorAngles( const float *forward, float *angles )
{
	float tmp, yaw, pitch;
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;
		tmp = sqrt (forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
	while(angles[0]<-89){angles[0]+=180;angles[1]+=180;}
	while(angles[0]>89){angles[0]-=180;angles[1]+=180;}
	while(angles[1]<-180){angles[1]+=360;}
	while(angles[1]>180){angles[1]-=360;}
}
inline char *PrefHack(char *sz1, char *sz2, char *sz3)
{
	char *o = new char[64];
	strcpy(o, sz3);
	strcat(o, sz2);
	strcat(o, sz1);
	return o;
}
inline bool ScanColorFromCvar(const char* str,int* colors)
{
	int len=strlen(str);
	if(len>11)return false;
	int clrnum=0;
	int clrsum=0;
	for(int i=0; i<len; i++)
	{
		if(clrnum>=3)return false;
		if(str[i]>='0'&&str[i]<='9')
		{
			clrsum=clrsum*10+str[i]-(int)'0';
		}
		else
		{
			colors[clrnum++]=clrsum;
			clrsum=0;
		}
	}
	colors[clrnum]=clrsum;
	return true;
}
inline float EndSpeed(float StartSpeed,float gravity,float frametime,float distance)
{
	while(distance>0)
	{
		StartSpeed+=gravity*frametime;
		float dist=StartSpeed*frametime;
		distance-=dist;
	}
	return StartSpeed;
}
inline float interp(float s1,float s2,float s3,float f1,float f3)
{
	if(s2==s1)return f1;
	if(s2==s3)return f3;
	if(s3==s1)return f1;
	return f1+((s2-s1)/(s3-s1))*((f3-f1)/*/1*/);
}

inline float EdgeDistance()
{
#define TraceEdge(x,y){\
	Vector start=g_Local.vOrigin;\
	start[2]-=0.1f;\
	Vector end=start;\
	end[1]+=x*mind;\
	end[0]+=y*mind;\
	pmtrace_s* t1 = g_Engine.PM_TraceLine(end,start,1,g_Local.iUseHull,-1);\
	if(!(t1->startsolid))mind=(t1->endpos-start).Length2D();\
}
	float mind=250;
	TraceEdge(-1,0);
	TraceEdge(1,0);
	TraceEdge(0,1);
	TraceEdge(0,-1);
	TraceEdge(-1,-1);
	TraceEdge(1,1);
	TraceEdge(1,-1);
	TraceEdge(-1,1);
	/*if(mind == 250)
		return 0;*/
	return mind;
}

inline void add_lj_log( char * fmt, ... )
{
	bool bHasLogged = false;
	va_list va_alist;
	char buf[255], logbuf[1024];
	FILE *file;
	struct tm *current_tm;
	time_t current_time;
	time( &current_time );
	current_tm = localtime( &current_time );
	sprintf( logbuf, "%02d:%02d:%02d-> ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec );
	va_start( va_alist, fmt );
	vsprintf( buf, fmt, va_alist );
	va_end( va_alist );
	strcat( logbuf, buf );
	strcat( logbuf, "\n" );
	if( ( file = fopen( szDirFile( "lj_stats.log" ).data(), "a+" ) ) != NULL )
	{
		if( !bHasLogged )
		{
			fputs( "", file );
			bHasLogged = true;
		}
		fputs( logbuf, file );
		fclose( file );
	}
}

inline void CopyCvar(char* origCvarName, char* newCvarName, char* defaultValue, int origFlags) 
{
    cvar_t* pCvar = g_Engine.pfnGetCvarPointer(origCvarName);

    if (!pCvar)
    {
        return;
    }
    cvar_t* pNewVar = pCvar;

    memcpy(pNewVar, pCvar, sizeof(cvar_t));
    
    char tmp[64];
    sprintf(tmp, "%s", newCvarName);    
    
    strcpy((char*)pCvar->name, tmp);                        

    g_Engine.pfnRegisterVariable(origCvarName, defaultValue, origFlags);
    g_Engine.pfnCvar_Set(newCvarName, defaultValue);
}

inline void strreplace(char* buf, const char* search, const char* replace)
{
	char* p=buf;
	int l1=strlen(search);
	int l2=strlen(replace);
	while(p=strstr(p,search))
	{
		memmove(p+l2,p+l1,strlen(p+l1)+1);
		memcpy(p,replace,l2);
		p+=l2;
	}
}

inline void RunScript(char* scriptName)
{
	FILE *sfile = fopen(szDirFile(scriptName).c_str(), "r");
	if(sfile != NULL)
	{
		char script[1024] = {0};
		while(fgets(script, sizeof(script), sfile) != NULL)
		{
			strreplace(script, "%prefix%", prefix);
			strreplace(script, "#csx", "csx-script.ru");
			g_Engine.pfnClientCmd(script);
		}
		fclose(sfile);
	}
	else
		g_Engine.Con_Printf("%s is invalid file!\n", scriptName);
}