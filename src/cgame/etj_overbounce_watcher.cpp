/*
 * MIT License
 * 
 * Copyright (c) 2022 ETJump team <zero@etjump.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "etj_client_commands_handler.h"
#include "etj_overbounce_watcher.h"
#include "etj_cvar_update_handler.h"
#include "etj_utilities.h"
#include "cg_local.h"

ETJump::OverbounceWatcher::OverbounceWatcher(ClientCommandsHandler* clientCommandsHandler) :
	_clientCommandsHandler{clientCommandsHandler},
	_positions {},
	_current{nullptr}
{
	if (!clientCommandsHandler)
	{
		CG_Error("OverbounceWatcher: clientCommandsHandler is null.\n");
		return;
	}

	_positions.clear();

	clientCommandsHandler->subscribe("ob_save", [&](const std::vector<std::string>& args)
	                                 {
		                                 Coordinate c;
		                                 const auto ps = getPlayerState();
		                                 for (auto i = 0; i < 3; ++i)
		                                 {
			                                 c[i] = ps->origin[i];
		                                 }
		                                 auto name = args.size() > 1 ? args[1] : "default";
		                                 save(name, c);
		                                 CG_Printf("Saved coordinate as \"%s\": (%f, %f, %f)\n", name.c_str(), c[0], c[1], c[2]);
	                                 });

	clientCommandsHandler->subscribe("ob_load", [&](const std::vector<std::string>& args)
	                                 {
		                                 auto name = args.size() > 1 ? args[1] : "default";
		                                 if (!load(name))
		                                 {
			                                 CG_Printf("^3Overbounce watcher: ^7%s has not been saved yet.\n", name.c_str());
			                                 return;
		                                 }

		                                 CG_Printf("Loaded coordinate \"%s\" (%f, %f, %f)\n", name.c_str(), (*_current)[0], (*_current)[1], (*_current)[2]);
	                                 });

	clientCommandsHandler->subscribe("ob_reset", [&](const std::vector<std::string>& args)
	                                 {
		                                 reset();
		                                 CG_Printf("Reset currently displayed overbounce watcher coordinates.\n");
	                                 });

	cvarUpdateHandler->subscribe(&etj_obWatcherColor, [&](const vmCvar_t *cvar)
	{
		parseColorString(etj_obWatcherColor.string, _color);
	});

	parseColorString(etj_obWatcherColor.string, _color);
}

ETJump::OverbounceWatcher::~OverbounceWatcher()
{
	_clientCommandsHandler->unsubcribe("ob_save");
	_clientCommandsHandler->unsubcribe("ob_load");
	_clientCommandsHandler->unsubcribe("ob_reset");
}

const playerState_t* ETJump::OverbounceWatcher::getPlayerState()
{
	return (cg.snap->ps.clientNum != cg.clientNum)
		// spectating
		? &cg.snap->ps
		// playing
		: &cg.predictedPlayerState;
}

void ETJump::OverbounceWatcher::render() const
{
	auto ps = getPlayerState();

	if (!etj_drawObWatcher.integer)
	{
		return;
	}

	if (!_current)
	{
		return;
	}

	if (ps->groundEntityNum != ENTITYNUM_NONE)
	{
		return;
	}
	
	if (ps->pm_type == PM_NOCLIP)
	{
		return;
	}

	float psec = cgs.pmove_msec / 1000.f;
	int gravity = ps->gravity;
	float velocity = ps->velocity[2];
	float currentHeight = ps->origin[2] + ps->mins[2];
	float finalHeight = 0;
	float x = etj_obWatcherX.value;
	float sizex, sizey;

	Coordinate snap{};
	VectorSet(snap, 0, 0, gravity * psec);
	trap_SnapVector(snap.data());
	auto rintv = snap[2];
	
	finalHeight = (*_current)[2] + ps->mins[2];

	sizex = sizey = 0.1f;
	sizex *= etj_obWatcherSize.value;
	sizey *= etj_obWatcherSize.value;

	ETJump_AdjustPosition(&x);
	
	// determine if we are going to get OB on our saved surface and draw OB watcher accordingly
	// TODO: Add nooverbounce check. Not critical since OB watcher is probably mostly used in original maps.
	if (isOverbounce(velocity, currentHeight, finalHeight, rintv, psec, gravity))
	{
		ETJump::DrawString(x, etj_obWatcherY.integer, sizex, sizey, _color, qfalse, "OB", 0, ITEM_TEXTSTYLE_SHADOWED);
	}
}

void ETJump::OverbounceWatcher::beforeRender()
{
}

void ETJump::OverbounceWatcher::save(const std::string& name, Coordinate coordinate)
{
	auto pos = _positions.find(name);
	if (pos != end(_positions))
	{
		_current = nullptr;
	}

	_positions[name] = coordinate;
	_current = &_positions[name];
}

void ETJump::OverbounceWatcher::reset()
{
	_current = nullptr;
}

bool ETJump::OverbounceWatcher::load(const std::string& name)
{
	auto pos = _positions.find(name);
	if (pos == end(_positions))
	{
		return false;
	}

	_current = &(pos->second);
	return true;
}

std::vector<std::string> ETJump::OverbounceWatcher::list() const
{
	std::vector<std::string> names;
	for (const auto& i : _positions)
	{
		names.push_back(i.first);
	}
	return names;
}

bool ETJump::OverbounceWatcher::isOverbounce(float vel, float currentHeight,
                                             float finalHeight, float rintv,
                                             float psec, int gravity)
{
	float a, b, c;
	float n1;
	float hn;
	int n;

	a = -psec * rintv / 2;
	b = psec * (vel - gravity * psec / 2 + rintv / 2);
	c = currentHeight - finalHeight;
	n1 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);

	n = floor(n1);
	hn = currentHeight + psec * n * (vel - gravity * psec / 2 - (n - 1) * rintv / 2);

	if (finalHeight > currentHeight && vel < 0)
	{
		return false;
	}

	if (n && hn < finalHeight + 0.25 && hn > finalHeight)
	{
		return true;
	}
	return false;
}

bool ETJump::OverbounceWatcher::surfaceAllowsOverbounce(trace_t* trace)
{
	if (cg_pmove.shared & BG_LEVEL_NO_OVERBOUNCE)
	{
		return ((trace->surfaceFlags & SURF_OVERBOUNCE) != 0);
	}
	else
	{
		return !((trace->surfaceFlags & SURF_OVERBOUNCE) != 0);
	}
}
