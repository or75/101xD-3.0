#include <Windows.h>
#include <fstream>
#include "ways.h"
#include "../SDKIncludes.h"
#include "../cVars/cvars.h"
#include "../locals.h"

using namespace std;

#pragma warning(disable:4018)
#pragma warning(disable:4615)
#pragma warning(disable:4800)

waybot Way;
vector<waypoint> ways;
int waybot::saves = 0;
int waybot::target = -1;
int waybot::target_edit = -1;

void waybot::CL_CreateMove(float frametime, struct usercmd_s *cmd, int active)
{
	if(ways.size()>0)
	{
		int beamindex = g_Engine.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");
		Vector start1 =  ways[0].origin;start1.z-=5;
		Vector start2 =  ways[0].origin;start2.z-=5;
		g_Engine.pEfxAPI->R_BeamPoints(start1,start2,beamindex,frametime+0.01f,1,0,1,0,0,0,1,0,0);
		//g_Engine.pEfxAPI->R_TeleportSplash(ways[0].origin);
		for(int i = 1; i < ways.size();i++)
		{
			int color[3]={};
			ScanColorFromCvar(cvar.waybot_color->string,color);
			g_Engine.pEfxAPI->R_BeamPoints(ways[i-1].origin,ways[i].origin,beamindex,frametime+0.01f,1,0,1,0,0,0,color[0],color[1],color[2]);
			Vector jump1 =  ways[i].origin;jump1.z-=10;
			Vector jump2 =  ways[i].origin;jump2.z-=5;
			Vector duck1 =  ways[i].origin;duck1.z+=10;
			Vector duck2 =  ways[i].origin;duck2.z+=5;
			if(ways[i].type == 1)
				g_Engine.pEfxAPI->R_BeamPoints(duck1,duck2,beamindex,frametime+0.01f,1,0,1,0,0,0,1,0,0);
			if(ways[i].type == 2)
				g_Engine.pEfxAPI->R_BeamPoints(jump1,jump2,beamindex,frametime+0.01f,1,0,1,0,0,0,0,1,1);
		
		}
		if(target != (-1) && g_Local.bAlive)
		{
			float angles[3];
			VectorAngles(Vector(ways[Way.target].origin)-g_Local.vEye,angles);
			cmd->viewangles = angles;
			cmd->forwardmove = 999999;
			if(ways[target].type == 2)
				cmd->buttons |= IN_DUCK;
			if(VectorDistance(ways[target].origin,g_Local.vOrigin)<40)
			{
				if(g_Local.flXYspeed<10 &&ways[target].type != 1)
					target++;
				else cmd->forwardmove = 0;
				if(ways[target-1].type == 1 && g_Local.iFlags &FL_ONGROUND)
				{
					cmd->buttons |= IN_JUMP;
				}
			}
			if(target >= ways.size())
				target=-1;
		}
		float distance = 8912;
		int id = -1;
		for(int i = 0; i < ways.size();i++)
		{
			if(!bPathFree(ways[i].origin,g_Local.vEye))continue;
			if(VectorDistance(ways[i].origin,g_Local.vOrigin)<distance)
			{
				distance = VectorDistance(ways[i].origin,g_Local.vOrigin);
				id = i;
			}
		}
		if(id != -1)
		{
			target_edit = id;
			int beamindex = g_Engine.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");
			g_Engine.pEfxAPI->R_BeamPoints(Vector(ways[id].origin[0],ways[id].origin[1],ways[id].origin[2]+5),Vector(ways[id].origin[0],ways[id].origin[1],ways[id].origin[2]-5),beamindex,frametime+0.01f,1,0,1,0,0,0,1,1,0);
		}
	}
	if(target!=(-1))
		cfunc.SpeedEngine(cvar.waybot_speed->value,frametime);
}

void waybot::CreatePoint()
{
	waypoint ways2;
	ways2.type = 0;
	if(strstr(g_Engine.Cmd_Argv(1),"j") || strstr(g_Engine.Cmd_Argv(1),"jump"))
		ways2.type = 1;
	if(strstr(g_Engine.Cmd_Argv(1),"d") || strstr(g_Engine.Cmd_Argv(1),"duck"))
		ways2.type = 2;
	VectorCopy(g_Local.vOrigin,ways2.origin);
	ways.push_back(ways2);
	saves++;
}

void waybot::EditPoint()
{
	float distance = 8912;
	int id = -1;
	for(int i = 0; i < ways.size();i++)
	{
		if(!bPathFree(ways[i].origin,g_Local.vEye))continue;
		if(VectorDistance(ways[i].origin,g_Local.vOrigin)<distance)
		{
			distance = VectorDistance(ways[i].origin,g_Local.vOrigin);
			id = i;
		}
	}
	ways[id].origin[0] = g_Local.vOrigin[0];
	ways[id].origin[1] = g_Local.vOrigin[1];
	ways[id].origin[2] = g_Local.vOrigin[2];
}

void waybot::ClearPoints()
{
	ways.clear();
	target = -1;
	saves = 0;
}

void waybot::ClearLastPoint()
{
	ways.pop_back();
	saves--;
}

void waybot::Start()
{
	target = 0;
}

void waybot::Stop()
{
	target = -1;
}

void waybot::Resume()
{
	float distance = 8912;
	int id = -1;
	for(int i = 0; i < ways.size();i++)
	{
		if(!bPathFree(ways[i].origin,g_Local.vEye))continue;
		if(VectorDistance(ways[i].origin,g_Local.vOrigin)<distance)
		{
			distance = VectorDistance(ways[i].origin,g_Local.vOrigin);
			id = i;
		}
	}
	if(id != -1)
		Way.target = id;
}

void waybot::Save()
{
	if(g_Engine.Cmd_Argc() > 1)
	{
		char name[256]={};
		strcpy(name,g_Engine.Cmd_Argv(1));
		char map[256]={};
		const char* mapp=g_Engine.pfnGetLevelName();
		memcpy(map,strstr(mapp,"/")+1,strlen(mapp)-4-(int)(strstr(mapp,"/")+1-mapp));
		char buf[256]={};
		sprintf(buf,"waybot\\%s\\",map);
		std::string pth0=szDirFile(buf);
		pth0.copy(buf,255);
		SECURITY_ATTRIBUTES sa={};
		CreateDirectoryA(buf,0);
		sprintf(buf,"waybot\\%s\\%s.way",map,name);
		std::string pth1=szDirFile(buf);
		pth1.copy(buf,255);
		std::ofstream wp(buf);
		if(wp.is_open())
		{
			for(int i=0; i < ways.size(); i++)
			{
				wp<<ways[i].type<<" "<<ways[i].origin[0]<<" "<<ways[i].origin[1]<<" "<<ways[i].origin[2]<<"\n";
			}
			wp.flush();
			wp.close();
			g_Engine.Con_Printf("[101xD] Sucessfully saved to %s\n",buf);
		}
		else g_Engine.Con_Printf("[101xD] Error. Cant save to %s\n",buf);
	}
	else g_Engine.Con_Printf("[101xD] Error. Syntax: %swaybot_save <waybot filename(without path)>\n",prefix);
}

void waybot::Load()
{
	if(g_Engine.Cmd_Argc() > 1)
	{
		char name[256]={};
		strcpy(name,g_Engine.Cmd_Argv(1));
		char map[256]={};
		char buf[256]={};
		const char* mapp=g_Engine.pfnGetLevelName();
		memcpy(map,strstr(mapp,"/")+1,strlen(mapp)-4-(int)(strstr(mapp,"/")+1-mapp));	
		sprintf(buf,"waybot\\%s\\%s.way",map,name);
		std::string pth1=szDirFile(buf);
		pth1.copy(buf,255);
		std::ifstream wp(buf);
		if(wp.is_open())
		{
			ways.clear();
			while(!wp.eof()&&!wp.bad())
			{
				waypoint ways2;
				memset(&ways2,0,sizeof(waypoint));
				wp>>ways2.type>>ways2.origin[0]>>ways2.origin[1]>>ways2.origin[2];
				if(wp.bad()||wp.eof())
					break;
				ways.push_back(ways2);
				saves++;
			}
			g_Engine.Con_Printf("[101xD] Sucessfully loaded from %s\n",buf);
		}else g_Engine.Con_Printf("[101xD] Don't can load from %s\n",buf);
	}
	else g_Engine.Con_Printf("[101xD] Error. Syntax: %swaybot_load <waybot filename(without path)>\n",prefix);
}

