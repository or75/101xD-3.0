#ifndef  _WAYBOT_H_
#define  _WAYBOT_H_

bool bPathFree( float *pflFrom, float *pflTo );

struct waypoint
{
	int type;
	float origin[3];
};

class waybot
{
public:
	static void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active);
	static int target;
	static int target_edit;
	static int saves;
	static void CreatePoint();
	static void EditPoint();
	static void ClearPoints();
	static void ClearLastPoint();
	static void Start();
	static void Stop();
	static void Resume();
	static void Save();
	static void Load();

}; extern waybot Way;
#endif