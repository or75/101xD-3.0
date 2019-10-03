#ifndef __CVARS__
#define __CVARS__

#pragma once

#include <Windows.h>
#include "../SDKIncludes.h"

struct CVars
{
	cvar_s *knifebot;
	cvar_s *knifebot_aim;
	cvar_s *knifebot_aim_fov;
	cvar_s *knifebot_aim_wp;
	cvar_s *knifebot_team;
	cvar_s *knifebot_attack;
	cvar_s *knifebot_dist_attack;
	cvar_s *knifebot_dist_attack2;
	cvar_s *knifebot_fps;
	cvar_s *knifebot_fps_time;

	cvar_s *lj_stats_log;

	cvar_s *hide_player;

	cvar_s *wallhack_xqz;
	cvar_s *whitewalls;

	cvar_s *bhop;
	cvar_s *bhop_nsd;
	cvar_s *bhop_cnt;
	cvar_s *bhop_autoduck;
	cvar_s *bhop_cnt_rand;
	cvar_s *bhop_cnt_rand_min;
	cvar_s *bhop_cnt_rand_max;

	cvar_s *fullbright;

	cvar_s *noflash;

	cvar_s *cam;
	cvar_s *cam_up;
	cvar_s *cam_side;
	cvar_s *cam_back;

	cvar_s *radar;
	cvar_s *radar_team;

	cvar_s *waybot_speed;
	cvar_s *waybot_color;
	cvar_s *waybot_speed_fps;
	cvar_s *waybot_speed_fps_file;

	cvar_s *nospread;

	cvar_s *norecoil;

	cvar_s *autopistol;
	cvar_s *autopistol_wpn_fix;

	cvar_s *aim;
	cvar_s *aim_help;
	cvar_s *aim_auto;
	cvar_s *aim_fov;
	cvar_s *aim_team;
	cvar_s *aim_mode;
	cvar_s *aim_autowall;
	cvar_s *aim_silent;
	cvar_s *aim_height;
	cvar_s *aim_height_hitbox;
	cvar_s *aim_height_bone;
	cvar_s *aim_smooth;
	cvar_s *aim_prediction;
	cvar_s *aim_prediction_offset;
	cvar_s *aim_auto_attack;

	cvar_s *triggerbot;
	cvar_s *triggerbot_spot;
	cvar_s *triggerbot_fov;

	cvar_s *addvec_j;
	cvar_s *addvec_s;
	cvar_s *addvec_d;

	cvar_s *show;
	cvar_s *show_fps;
	cvar_s *show_time;
	cvar_s *show_keys;
	cvar_s *show_color;
	cvar_s *show_info;
	cvar_s *show_info_main;
	cvar_s *show_info_strafe;
	cvar_s *show_info_helper;
	cvar_s *show_info_waybot;

	cvar_s *show_kz;
	cvar_s *show_kz_edge;
	cvar_s *show_kz_speed;
	cvar_s *show_kz_height;
	cvar_s *show_kz_damage;
	cvar_s *show_kz_ljstats;
	cvar_s *show_kz_fallspeed;

	cvar_s *show_kz_color;

	cvar_s *show_glow_model;
	cvar_s *show_glow_model_color;
	cvar_s *show_glow_model_ticknes;
	cvar_s *show_glow_model_team;
	
	cvar_s *show_glow_weapon;
	cvar_s *show_glow_weapon_color;
	cvar_s *show_glow_weapon_tickness;

	cvar_s *gl_font;

	cvar_s *freelook_speed;
	cvar_s *fps_slowto;
	cvar_s *fps_helper;
	cvar_s *fixedyaw;
	cvar_s *fixedpitch;

	cvar_s *speed;
	cvar_s *speed_net;

	cvar_s *strafe_aa;

	cvar_s *strafe_fps;
	cvar_s *strafe_temp;
	cvar_s *strafe_dir;
	cvar_s *strafe_dir_auto;
	cvar_s *strafe_invisible;

	cvar_s *strafe_speed;
	cvar_s *strafe_crazy;
	cvar_s *strafe_helper;
	cvar_s *strafe_helper_add_strafe;
	cvar_s *strafe_helper_max_strafe;
	cvar_s *strafe_helper_max_strafe_rand;
	cvar_s *strafe_helper_max_strafe_rand_min;
	cvar_s *strafe_helper_max_strafe_rand_max;
	cvar_s *strafe_helper_boost;
	cvar_s *strafe_helper_move;
	cvar_s *strafe_helper_move_rand;
	cvar_s *strafe_helper_move_rand_min;
	cvar_s *strafe_helper_move_rand_max;
	cvar_s *strafe_helper_main;

	cvar_s *gstrafe_bhop;
	cvar_s *gstrafe_standup; 
	cvar_s *gstrafe_noslowdown;

	cvar_s *esp;
	cvar_s *esp_name;
	cvar_s *esp_weapon;
	cvar_s *esp_sequence;
	cvar_s *esp_box;
	cvar_s *esp_box_size;
	cvar_s *esp_3dbox;
	cvar_s *esp_distance;
	cvar_s *esp_gun_shot;

	cvar_s *esp_far;
	cvar_s *esp_far_smooth;

	cvar_s *esp_sound;
	cvar_s *esp_sound_team;
	cvar_s *esp_sound_color;
	cvar_s *esp_sound_text;

	//cvar_s *duck;

	cvar_s *jb_auto;

	cvar_s *debug_sound;

	cvar_s *SpinHack;
	cvar_s *SpinHackSpeed;
};
extern CVars cvar;

void HackSteamID();

struct CFunc
{
public:
	void Init();

	void SpeedEngine(float speed, float frametime);
	static bool forwardm;
	static bool moveleft;
	static bool moveright;
	static bool back;
	static bool duck;
	static bool jump;

	static void fps_on();
	static void fps_off();

	void p_for();
	void m_for();

	void p_ml();
	void m_ml();

	void p_mr();
	void m_mr();

	void p_bk();
	void m_bk();

	void p_dk();
	void m_dk();

	void p_jp();
	void m_jp();
};
extern CFunc cfunc;

#endif