#include <windows.h>
#include <iostream>
#include <string>


#define LOG_INFO(...)
// #define LOG_INFO(...) std :: cout << __VA_ARGS__ << std :: endl;
#define IS_LANDSCAPE(orientation) ((orientation) == DMDO_DEFAULT || (orientation) == DMDO_180)
#define LANDSCAPE_NAME(is_landscape) ((is_landscape) ? "Landscape" : "Portrait")

//	0 -   0 - Landscape (Default/0 degrees)
//	1 -  90 - Portrait (90 degrees)
//	2 - 180 - Landscape Flipped (180 degrees)
//	3 - 270 - Portrait Flipped (270 degrees)

char const* orientationName(int orientation) {
	switch(orientation) {
		case DMDO_DEFAULT: { return ("DMDO_DEFAULT"); } break;
		case DMDO_90:      { return ("DMDO_90"     ); } break;
		case DMDO_180:     { return ("DMDO_180"    ); } break;
		case DMDO_270:     { return ("DMDO_270"    ); } break;
		default:           { return ("----"        ); } break;
	}
}

bool changeDisplayOrientation(int orientation) {
	if (true 
		&& orientation != DMDO_DEFAULT
		&& orientation != DMDO_90
		&& orientation != DMDO_180
		&& orientation != DMDO_270
	) {
		LOG_INFO("Incorrect value " << orientation);
		return false;
	}
	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	// only change first/default display (index=0)
	if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm)) {
		LOG_INFO("Failed EnumDisplaySettings");
		return false;
	}
	int current = dm.dmDisplayOrientation;
	LOG_INFO(orientationName(current) << " --> " << orientationName(orientation));
	if (current == orientation) {
		LOG_INFO("No change");
		return true;
	}
	dm.dmFields = DM_DISPLAYORIENTATION;
	dm.dmDisplayOrientation = orientation;
	
	bool prev_is_landscape = current     == DMDO_DEFAULT || current     == DMDO_180;
	bool next_is_landscape = orientation == DMDO_DEFAULT || orientation == DMDO_180;
	
	LOG_INFO(LANDSCAPE_NAME(prev_is_landscape) << " --> " << LANDSCAPE_NAME(next_is_landscape));
	
	if (prev_is_landscape != next_is_landscape) {
		auto w = dm.dmPelsWidth;
		auto h = dm.dmPelsHeight;
		LOG_INFO("Switch " << w << "x" << h << " --> " << h << "x" << w);
		dm.dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;
		dm.dmPelsWidth  = h;
		dm.dmPelsHeight = w;
	}

	auto ret = ChangeDisplaySettingsEx(NULL, &dm, NULL, 0, NULL); //CDS_RESET, NULL); //0);
	switch (ret) {
		case DISP_CHANGE_SUCCESSFUL: { 
			LOG_INFO("display successfully changed");
			return true;
		}
		case DISP_CHANGE_BADDUALVIEW: { 
			LOG_INFO("The settings change was unsuccessful because the system is DualView capable");
			return false;
		}
		case DISP_CHANGE_BADFLAGS: { 
			LOG_INFO("An invalid set of flags was passed in.");
			return false;
		}
		case DISP_CHANGE_BADMODE: { 
			LOG_INFO("The graphics mode is not supported.");
			return false;
		}
		case DISP_CHANGE_BADPARAM: { 
			LOG_INFO("An invalid parameter was passed in. This can include an invalid flag or combination of flags.");
			return false;
		}
		case DISP_CHANGE_FAILED: { 
			LOG_INFO("The display driver failed the specified graphics mode.");
			return false;
		}
		case DISP_CHANGE_NOTUPDATED: { 
			LOG_INFO("Unable to write settings to the registry.");
			return false;
		}
		case DISP_CHANGE_RESTART: { 
			LOG_INFO("The computer must be restarted for the graphics mode to work.");
			return false;
		}
		default: {
			LOG_INFO("Unknown error: " << ret);
			return false;
		}
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		LOG_INFO("Required 1 positional argument (0-3)");
		return 1;
	}
	auto arg = argv[1];
	if (arg[0] == '\0' || arg[1] != '\0') {
		LOG_INFO("argument must be 0-3")
		return 1;
	}
	char orientation = arg[0] - '0';
	if (!changeDisplayOrientation(orientation)) {
		LOG_INFO("Failed to change display orientation.");
		return 1;
	}

	return 0;
}

