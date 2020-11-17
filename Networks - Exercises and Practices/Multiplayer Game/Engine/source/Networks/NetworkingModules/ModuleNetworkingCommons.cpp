#include "Core/Core.h"

uint16 PackInputControllerButtons(const InputController & input)
{
	uint16 buttonBits = 0;
	int buttonIndex = 0;
	for (ButtonState buttonState : input.buttons)
	{
		uint16 bit = (int)(buttonState == ButtonState::PRESS || buttonState == ButtonState::PRESSED);
		buttonBits |= (bit << buttonIndex);
		buttonIndex++;
	}

	return buttonBits;
}

void UnpackInputControllerButtons(uint16 buttonBits, InputController & input)
{
	// NOTE(jesus): This table contains the ButtonState depending on the pressed state (true or false) of a button
	static const ButtonState transition[2][2] =      // Index 0 means released, 1 means pressed
	{
		{ButtonState::IDLE,    ButtonState::PRESS},  // 0 0, 0 1
		{ButtonState::RELEASE, ButtonState::PRESSED} // 1 0, 1 1
	};

	int buttonIndex = 0;
	for (ButtonState &buttonState : input.buttons)
	{
		int wasPressed = (int)(buttonState == ButtonState::PRESS || buttonState == ButtonState::PRESSED);
		int isPressed = (int)(bool)(buttonBits & (1 << buttonIndex));

		buttonState = transition[wasPressed][isPressed];
		buttonIndex++;
	}
}

InputController InputControllerFromInputPacketData(const InputPacketData & inputPacketData, const InputController & previousGamepad)
{
	InputController gamepad = previousGamepad;
	gamepad.horizontalAxis = inputPacketData.horizontalAxis;
	gamepad.verticalAxis = inputPacketData.verticalAxis;

	UnpackInputControllerButtons(inputPacketData.buttonBits, gamepad);
	return gamepad;
}