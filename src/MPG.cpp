/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#include "MPG.h"

static HIDReport hidReport
{
	.buttons = 0,
	.hat = HID_HAT_NOTHING,
	.lx = HID_JOYSTICK_MID,
	.ly = HID_JOYSTICK_MID,
	.rx = HID_JOYSTICK_MID,
	.ry = HID_JOYSTICK_MID,
};

static SwitchReport switchReport
{
	.buttons = 0,
	.hat = SWITCH_HAT_NOTHING,
	.lx = SWITCH_JOYSTICK_MID,
	.ly = SWITCH_JOYSTICK_MID,
	.rx = SWITCH_JOYSTICK_MID,
	.ry = SWITCH_JOYSTICK_MID,
	.vendor = 0,
};

static XInputReport xinputReport
{
	.report_id = 0,
	.report_size = XINPUT_ENDPOINT_SIZE,
	.buttons1 = 0,
	.buttons2 = 0,
	.lt = 0,
	.rt = 0,
	.lx = GAMEPAD_JOYSTICK_MID,
	.ly = GAMEPAD_JOYSTICK_MID,
	.rx = GAMEPAD_JOYSTICK_MID,
	.ry = GAMEPAD_JOYSTICK_MID,
	._reserved = { },
};

static MdminiReport mdminiReport
{
	.id = 0x01,				// ID
	.notuse1 = 0x7f,		// [Not Use] 0x7f
	.notuse2 = 0x7f,		// [Not Use] 0x7f
	.hat1 = 0x7f,			// LEFT:0x00, RIGHT:0xFF
	.hat2 = 0x7f,			// UP:0x00, DOWN:0xFF
	.buttons1 = 0x0f,		// X A B Y 1b 1b 1b 1b
	.buttons2 = 0x00,		// 0b 0b START MODE 0b 0b C Z
	.notuse3 = 0x00,		// [Not Use] 0x00
};

void *MPG::getReport()
{
	switch (options.inputMode)
	{
		case INPUT_MODE_XINPUT:
			return getXInputReport();

		case INPUT_MODE_SWITCH:
			return getSwitchReport();

		case INPUT_MODE_MDMINI:
			return getMdminiReport();

		default:
			return getHIDReport();
	}
}


uint16_t MPG::getReportSize()
{
	switch (options.inputMode)
	{
		case INPUT_MODE_XINPUT:
			return sizeof(XInputReport);

		case INPUT_MODE_SWITCH:
			return sizeof(SwitchReport);

		case INPUT_MODE_MDMINI:
			return sizeof(MdminiReport);

		default:
			return sizeof(HIDReport);
	}
}


HIDReport *MPG::getHIDReport()
{
	switch (state.dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        hidReport.hat = HID_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   hidReport.hat = HID_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     hidReport.hat = HID_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: hidReport.hat = HID_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      hidReport.hat = HID_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  hidReport.hat = HID_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      hidReport.hat = HID_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    hidReport.hat = HID_HAT_UPLEFT;    break;
		default:                                     hidReport.hat = HID_HAT_NOTHING;   break;
	}

	hidReport.buttons = 0
		| (pressedB1() ? HID_MASK_CROSS    : 0)
		| (pressedB2() ? HID_MASK_CIRCLE   : 0)
		| (pressedB3() ? HID_MASK_SQUARE   : 0)
		| (pressedB4() ? HID_MASK_TRIANGLE : 0)
		| (pressedL1() ? HID_MASK_L1       : 0)
		| (pressedR1() ? HID_MASK_R1       : 0)
		| (pressedL2() ? HID_MASK_L2       : 0)
		| (pressedR2() ? HID_MASK_R2       : 0)
		| (pressedS1() ? HID_MASK_SELECT   : 0)
		| (pressedS2() ? HID_MASK_START    : 0)
		| (pressedL3() ? HID_MASK_L3       : 0)
		| (pressedR3() ? HID_MASK_R3       : 0)
		| (pressedA1() ? HID_MASK_PS       : 0)
		| (pressedA2() ? HID_MASK_TP       : 0)
	;

	hidReport.lx = static_cast<uint8_t>(state.lx >> 8);
	hidReport.ly = static_cast<uint8_t>(state.ly >> 8);
	hidReport.rx = static_cast<uint8_t>(state.rx >> 8);
	hidReport.ry = static_cast<uint8_t>(state.ry >> 8);

	return &hidReport;
}


SwitchReport *MPG::getSwitchReport()
{
	switch (state.dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        switchReport.hat = SWITCH_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   switchReport.hat = SWITCH_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     switchReport.hat = SWITCH_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: switchReport.hat = SWITCH_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      switchReport.hat = SWITCH_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  switchReport.hat = SWITCH_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      switchReport.hat = SWITCH_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    switchReport.hat = SWITCH_HAT_UPLEFT;    break;
		default:                                     switchReport.hat = SWITCH_HAT_NOTHING;   break;
	}

	switchReport.buttons = 0
		| (pressedB1() ? SWITCH_MASK_B       : 0)
		| (pressedB2() ? SWITCH_MASK_A       : 0)
		| (pressedB3() ? SWITCH_MASK_Y       : 0)
		| (pressedB4() ? SWITCH_MASK_X       : 0)
		| (pressedL1() ? SWITCH_MASK_L       : 0)
		| (pressedR1() ? SWITCH_MASK_R       : 0)
		| (pressedL2() ? SWITCH_MASK_ZL      : 0)
		| (pressedR2() ? SWITCH_MASK_ZR      : 0)
		| (pressedS1() ? SWITCH_MASK_MINUS   : 0)
		| (pressedS2() ? SWITCH_MASK_PLUS    : 0)
		| (pressedL3() ? SWITCH_MASK_L3      : 0)
		| (pressedR3() ? SWITCH_MASK_R3      : 0)
		| (pressedA1() ? SWITCH_MASK_HOME    : 0)
		| (pressedA2() ? SWITCH_MASK_CAPTURE : 0)
	;

	switchReport.lx = static_cast<uint8_t>(state.lx >> 8);
	switchReport.ly = static_cast<uint8_t>(state.ly >> 8);
	switchReport.rx = static_cast<uint8_t>(state.rx >> 8);
	switchReport.ry = static_cast<uint8_t>(state.ry >> 8);

	return &switchReport;
}


XInputReport *MPG::getXInputReport()
{
	xinputReport.buttons1 = 0
		| (pressedUp()    ? XBOX_MASK_UP    : 0)
		| (pressedDown()  ? XBOX_MASK_DOWN  : 0)
		| (pressedLeft()  ? XBOX_MASK_LEFT  : 0)
		| (pressedRight() ? XBOX_MASK_RIGHT : 0)
		| (pressedS2()    ? XBOX_MASK_START : 0)
		| (pressedS1()    ? XBOX_MASK_BACK  : 0)
		| (pressedL3()    ? XBOX_MASK_LS    : 0)
		| (pressedR3()    ? XBOX_MASK_RS    : 0)
	;

	xinputReport.buttons2 = 0
		| (pressedL1() ? XBOX_MASK_LB   : 0)
		| (pressedR1() ? XBOX_MASK_RB   : 0)
		| (pressedA1() ? XBOX_MASK_HOME : 0)
		| (pressedB1() ? XBOX_MASK_A    : 0)
		| (pressedB2() ? XBOX_MASK_B    : 0)
		| (pressedB3() ? XBOX_MASK_X    : 0)
		| (pressedB4() ? XBOX_MASK_Y    : 0)
	;

	xinputReport.lx = static_cast<int16_t>(state.lx) + INT16_MIN;
	xinputReport.ly = static_cast<int16_t>(~state.ly) + INT16_MIN;
	xinputReport.rx = static_cast<int16_t>(state.rx) + INT16_MIN;
	xinputReport.ry = static_cast<int16_t>(~state.ry) + INT16_MIN;

	if (hasAnalogTriggers)
	{
		xinputReport.lt = state.lt;
		xinputReport.rt = state.rt;
	}
	else
	{
		xinputReport.lt = pressedL2() ? 0xFF : 0;
		xinputReport.rt = pressedR2() ? 0xFF : 0;
	}

	return &xinputReport;
}

MdminiReport *MPG::getMdminiReport()
{
	mdminiReport.hat1 = 0x7f;
	mdminiReport.hat2 = 0x7f;

	if (pressedLeft()) { mdminiReport.hat1 = MDMINI_MASK_LEFT; }
	if (pressedRight()) { mdminiReport.hat1 = MDMINI_MASK_RIGHT; }

	if (pressedUp()) { mdminiReport.hat2 = MDMINI_MASK_UP; }
	if (pressedDown()) { mdminiReport.hat2 = MDMINI_MASK_DOWN; }

	mdminiReport.buttons1 = 0x0f
		| (pressedB1()    ? MDMINI_MASK_A     : 0)
		| (pressedB2()    ? MDMINI_MASK_B     : 0)
		| (pressedB3()    ? MDMINI_MASK_X     : 0)
		| (pressedB4()    ? MDMINI_MASK_Y     : 0)
	;

	mdminiReport.buttons2 = 0x00
		| (pressedR1()    ? MDMINI_MASK_Z     : 0)
		| (pressedR2()    ? MDMINI_MASK_C     : 0)
		| (pressedS2()    ? MDMINI_MASK_START : 0)
		| (pressedS1()    ? MDMINI_MASK_MODE  : 0)
	;

	return &mdminiReport;
}

GamepadHotkey MPG::hotkey()
{
	static GamepadHotkey lastAction = HOTKEY_NONE;

	GamepadHotkey action = HOTKEY_NONE;
	if (pressedF1())
	{
		switch (state.dpad & GAMEPAD_MASK_DPAD)
		{
			case GAMEPAD_MASK_LEFT:
				action = HOTKEY_DPAD_LEFT_ANALOG;
				options.dpadMode = DPAD_MODE_LEFT_ANALOG;
				state.dpad = 0;
				state.buttons &= ~(f1Mask);
				break;

			case GAMEPAD_MASK_RIGHT:
				action = HOTKEY_DPAD_RIGHT_ANALOG;
				options.dpadMode = DPAD_MODE_RIGHT_ANALOG;
				state.dpad = 0;
				state.buttons &= ~(f1Mask);
				break;

			case GAMEPAD_MASK_DOWN:
				action = HOTKEY_DPAD_DIGITAL;
				options.dpadMode = DPAD_MODE_DIGITAL;
				state.dpad = 0;
				state.buttons &= ~(f1Mask);
				break;

			case GAMEPAD_MASK_UP:
				action = HOTKEY_HOME_BUTTON;
				state.dpad = 0;
				state.buttons &= ~(f1Mask);
				state.buttons |= GAMEPAD_MASK_A1; // Press the Home button
				break;
		}
	}
	else if (pressedF2())
	{
		switch (state.dpad & GAMEPAD_MASK_DPAD)
		{
			case GAMEPAD_MASK_DOWN:
				action = HOTKEY_SOCD_NEUTRAL;
				options.socdMode = SOCD_MODE_NEUTRAL;
				state.dpad = 0;
				state.buttons &= ~(f2Mask);
				break;

			case GAMEPAD_MASK_UP:
				action = HOTKEY_SOCD_UP_PRIORITY;
				options.socdMode = SOCD_MODE_UP_PRIORITY;
				state.dpad = 0;
				state.buttons &= ~(f2Mask);
				break;

			case GAMEPAD_MASK_LEFT:
				action = HOTKEY_SOCD_LAST_INPUT;
				options.socdMode = SOCD_MODE_SECOND_INPUT_PRIORITY;
				state.dpad = 0;
				state.buttons &= ~(f2Mask);
				break;

			case GAMEPAD_MASK_RIGHT:
				if (lastAction != HOTKEY_INVERT_Y_AXIS)
					options.invertYAxis = !options.invertYAxis;
				action = HOTKEY_INVERT_Y_AXIS;
				state.dpad = 0;
				state.buttons &= ~(f2Mask);
				break;
		}
	}

	lastAction = action;
	return action;
}


void MPG::process()
{
	state.dpad = runSOCDCleaner(options.socdMode, state.dpad);

	switch (options.dpadMode)
	{
		case DpadMode::DPAD_MODE_LEFT_ANALOG:
			if (!hasRightAnalogStick) {
				state.rx = GAMEPAD_JOYSTICK_MID;
				state.ry = GAMEPAD_JOYSTICK_MID;
			}
			state.lx = dpadToAnalogX(state.dpad);
			state.ly = dpadToAnalogY(state.dpad);
			state.dpad = 0;
			break;

		case DpadMode::DPAD_MODE_RIGHT_ANALOG:
			if (!hasLeftAnalogStick) {
				state.lx = GAMEPAD_JOYSTICK_MID;
				state.ly = GAMEPAD_JOYSTICK_MID;
			}
			state.rx = dpadToAnalogX(state.dpad);
			state.ry = dpadToAnalogY(state.dpad);
			state.dpad = 0;
			break;

		default:
			if (!hasLeftAnalogStick) {
				state.lx = GAMEPAD_JOYSTICK_MID;
				state.ly = GAMEPAD_JOYSTICK_MID;
			}
			if (!hasRightAnalogStick) {
				state.rx = GAMEPAD_JOYSTICK_MID;
				state.ry = GAMEPAD_JOYSTICK_MID;
			}
			break;
	}
}
