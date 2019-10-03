#ifndef __CLIENT_DLL__
#define __CLIENT_DLL__

typedef struct ClientDll_s
{
	int		(*Initialize)( cl_enginefunc_t *pEnginefuncs, int iVersion );
	void	(*HUD_Init)( void );
	int		(*HUD_VidInit)( void );
	int		(*HUD_Redraw)( float flTime, int intermission );
	int		(*HUD_UpdateClientData)( client_data_t *cdata, float flTime );
	void	(*HUD_Reset) ( void );
	void	(*HUD_PlayerMove)( struct playermove_s *ppmove, int server );
	void	(*HUD_PlayerMoveInit)( struct playermove_s *ppmove );
	char	(*HUD_PlayerMoveTexture)( char *name );
	void	(*IN_ActivateMouse)( void );
	void	(*IN_DeactivateMouse)( void );
	void	(*IN_MouseEvent)(int mstate);
	void	(*IN_ClearStates)(void);
	void	(*IN_Accumulate)(void);
	void	(*CL_CreateMove) ( float frametime, struct usercmd_s *cmd, int active );
	int		(*CL_IsThirdPerson)( void );
	void	(*CL_CameraOffset)( float *ofs );
	struct	kbutton_s* (*KB_Find)( const char *name );
	void	(*CAM_Think)( void );
	void	(*V_CalcRefdef)(struct ref_params_s *pparams);
	int		(*HUD_AddEntity)(int type, struct cl_entity_s *ent, const char *modelname);
	void	(*HUD_CreateEntities)(void);
	void	(*HUD_DrawNormalTriangles)(void);
	void	(*HUD_DrawTransparentTriangles)(void);
	void	(*HUD_StudioEvent)(const struct mstudioevent_s *, const struct cl_entity_s *);
	void	(*HUD_PostRunCmd)( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
	void	(*HUD_Shutdown)( void );
	void	(*HUD_TxferLocalOverrides)(struct entity_state_s *, const struct clientdata_s *);
	void	(*HUD_ProcessPlayerState)(struct entity_state_s *, const struct entity_state_s *);
	void	(*HUD_TxferPredictionData)(struct entity_state_s *, const struct entity_state_s *, struct clientdata_s *, const struct clientdata_s *, struct weapon_data_s *, const struct weapon_data_s *);
	void	(*Demo_ReadBuffer)(int, unsigned char *);
	int		(*HUD_ConnectionlessPacket)( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
	int		(*HUD_GetHullBounds)( int hullnumber, float *mins, float *maxs );
	void	(*HUD_Frame)( double time );
	int		(*HUD_Key_Event)( int eventcode, int keynum, const char *pszCurrentBinding );
	void	(*HUD_TempEntUpdate)(double, double, double, struct tempent_s **, struct tempent_s **, int(*Callback_AddVisibleEntity)(struct cl_entity_s*), void(*Callback_TempEntPlaySound)(struct tempent_s *pTemp, float damp ));
	struct cl_entity_s* (*HUD_GetUserEntity)(int index);
	void	(*HUD_VoiceStatus)(int entindex, qboolean bTalking);
	int		(*HUD_DirectorEvent) (unsigned char, unsigned int, unsigned int, unsigned int);
	int		(*HUD_GetStudioModelInterface)(int, struct r_studio_interface_s**, struct engine_studio_api_s*);
	unsigned long (*ClientFactory )(void);
} ClientDll_t;
extern ClientDll_t g_Client;
extern ClientDll_t*	g_pClient;

void HUD_TempEntUpdate (double frametime, double client_time, double cl_gravity, TEMPENTITY **ppTempEntFree, TEMPENTITY **ppTempEntActive, int ( *Callback_AddVisibleEntity )( cl_entity_t *pEntity ), void ( *Callback_TempEntPlaySound )( TEMPENTITY *pTemp, float damp ) );
cl_entity_t *HUD_GetUserEntity( int index );
int HUD_GetStudioModelInterface( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio );
void HUD_VoiceStatus(int entindex, qboolean bTalking);
int HUD_Key_Event( int down, int keynum, const char *pszCurrentBinding );
void HUD_Frame( double time );
int HUD_GetHullBounds( int hullnumber, float *mins, float *maxs );
int	HUD_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
void Demo_ReadBuffer( int size, unsigned char *buffer );
void HUD_TxferPredictionData ( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd );
void HUD_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src );
void HUD_TxferLocalOverrides( struct entity_state_s *state, const struct clientdata_s *client );
void HUD_Shutdown( void );
void HUD_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed );
void HUD_StudioEvent( const struct mstudioevent_s *pevent, const struct cl_entity_s *entity );
void HUD_DrawTransparentTriangles( void );
void HUD_DrawNormalTriangles( void );
void HUD_CreateEntities( void );
int HUD_AddEntity( int type, struct cl_entity_s *ent, const char *modelname );
void V_CalcRefdef( struct ref_params_s *pparams );
void CAM_Think( void );
struct kbutton_s *KB_Find( const char *name );
void CL_CameraOffset( float *ofs );
int CL_IsThirdPerson( void );
void CL_CreateMove ( float frametime, struct usercmd_s *cmd, int active );
void IN_ClearStates (void);
void IN_Accumulate (void);
void IN_MouseEvent (int mstate);
void IN_DeactivateMouse (void);
void IN_ActivateMouse (void);
void HUD_PlayerMove( struct playermove_s *ppmove, int server );
char HUD_PlayerMoveTexture( char *name );
void HUD_PlayerMoveInit( struct playermove_s *ppmove );
void HUD_Reset( void );
int HUD_UpdateClientData(client_data_t *pcldata, float flTime );
int HUD_Redraw( float time, int intermission );
int HUD_VidInit( void );
void HUD_Init( void );
bool bCalcScreen( float *pflOrigin, float *pflVecScreen );
int Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion );

extern bool strafe;
extern bool gstrafe;
extern bool jumpbug;
extern bool fastrun;
extern bool freelook;
extern bool norefresh;

#endif