/*
   Copyright (C) 2003 - 2018 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project https://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#pragma once

#include "editor/editor_main.hpp"    // for EXIT_STATUS
#include "events.hpp"                // for event_context
#include "font/font_config.hpp"      // for manager
#include "game_end_exceptions.hpp"   // for LEVEL_RESULT, etc
#include "hotkey/hotkey_manager.hpp" // for manager
#include "picture.hpp"               // for manager
#include "preferences/game.hpp"      // for manager
#include "saved_game.hpp"            // for saved_game
#include "sound.hpp"                 // for music_thinker

#include <string>                       // for string
#include <vector>                       // for vector

class commandline_options;
class config;
class CVideo;

namespace savegame { struct load_game_metadata; }
namespace preferences { class advanced_manager; }

struct jump_to_campaign_info
{
public:
	jump_to_campaign_info(bool jump, bool skip_story, int difficulty, const std::string& campaign_id,const std::string& scenario_id)
		: jump_(jump)
		, skip_story_(skip_story)
		, difficulty_(difficulty)
		, campaign_id_(campaign_id)
		, scenario_id_(scenario_id)
	{
	}
	bool jump_;
	bool skip_story_;
	int difficulty_;
	std::string campaign_id_,scenario_id_;
};

class game_launcher
{
public:
	game_launcher(const commandline_options& cmdline_opts, const char* appname);
	~game_launcher();

	CVideo& video() { return *video_; }

	enum mp_selection {MP_CONNECT, MP_HOST, MP_LOCAL};

	/**
	 * Status code after running a unit test, should match the run_wml_tests
	 * script and the documentation for the --unit_test command-line option.
	 */
	enum class unit_test_result : int {
		TEST_PASS = 0,
		TEST_FAIL = 1,
		// 2 is reserved for timeouts
		TEST_FAIL_LOADING_REPLAY = 3,
		TEST_FAIL_PLAYING_REPLAY = 4,
		TEST_FAIL_BROKE_STRICT = 5,
		TEST_FAIL_WML_EXCEPTION = 6,
		TEST_FAIL_BY_DEFEAT = 7,
		TEST_PASS_BY_VICTORY = 8,
	};

	bool init_video();
	bool init_language();
	bool init_lua_script();
	void init_advanced_prefs_manager();

	bool play_test();
	bool play_screenshot_mode();
	bool play_render_image_mode();
	/// Runs unit tests specified on the command line
	unit_test_result unit_test();

	bool is_loading() const;
	void clear_loaded_game();
	bool load_game();
	void set_tutorial();
	void set_test(const std::string& id);

	/// Return the ID of the campaign to jump to (skipping the main menu).
	std::string jump_to_campaign_id() const;
	bool new_campaign();
	bool goto_campaign();
	bool goto_multiplayer();
	bool goto_editor();

	bool jump_to_editor() const { return jump_to_editor_; }

	void select_mp_server(const std::string& server) { multiplayer_server_ = server; }
	bool play_multiplayer(mp_selection res);
	bool play_multiplayer_commandline();
	bool change_language();

	void show_preferences();

	enum RELOAD_GAME_DATA { RELOAD_DATA, NO_RELOAD_DATA };
	void launch_game(RELOAD_GAME_DATA reload=RELOAD_DATA);
	void play_replay();

	editor::EXIT_STATUS start_editor() { return start_editor(""); }

	void start_wesnothd();

	const commandline_options & opts() const { return cmdline_opts_; }
private:
	game_launcher(const game_launcher&);
	void operator=(const game_launcher&);

	void mark_completed_campaigns(std::vector<config>& campaigns);

	editor::EXIT_STATUS start_editor(const std::string& filename);
	unit_test_result pass_victory_or_defeat(LEVEL_RESULT res);

	/// Internal to the implementation of unit_test(). If a single instance of
	/// Wesnoth is running multiple unit tests, this gets called once per test.
	unit_test_result single_unit_test();

	const commandline_options& cmdline_opts_;
	//Never null.
	const std::unique_ptr<CVideo> video_;

	font::manager font_manager_;
	const preferences::manager prefs_manager_;
	std::unique_ptr<preferences::advanced_manager> advanced_prefs_manager_;
	const image::manager image_manager_;
	const events::event_context main_event_context_;
	const hotkey::manager hotkey_manager_;
	sound::music_thinker music_thinker_;
	sound::music_muter music_muter_;

	std::vector<std::string> test_scenarios_;

	std::string screenshot_map_, screenshot_filename_;

	saved_game state_;
	bool play_replay_;

	std::string multiplayer_server_;
	bool jump_to_multiplayer_;
	jump_to_campaign_info jump_to_campaign_;

	bool jump_to_editor_;
	std::unique_ptr<savegame::load_game_metadata> load_data_;
};
