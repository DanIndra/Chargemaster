#include <algorithm>
#include <cstring>

#include "smart_command.hpp"

namespace {
    SmartCommand::SmartCommandId decode_led_id(char c) {
        switch(c) {
        case 1: return SmartCommand::LED_RED_TOGGLE;
        case 2: return SmartCommand::LED_GREEN_TOGGLE;
        case 3: return SmartCommand::LED_BLUE_TOGGLE;
        default: return SmartCommand::UNKNOWN;
        }
    }

    SmartCommand::SmartCommandId decode_switch_id(char c) {
        switch(c) {
        case 101: return SmartCommand::V2G_SWITCH_OVERRIDE;
        case 100: return SmartCommand::V2G_SWITCH_PHYSICAL;
        case 0: return SmartCommand::V2G_SWITCH_IDLE;
        case 1: return SmartCommand::V2G_SWITCH_CHARGE;
        case 2: return SmartCommand::V2G_SWITCH_DISCHARGE;
		case 3: return SmartCommand::V2G_SWITCH_char0;
		case 4: return SmartCommand::V2G_SWITCH_char6;
		case 5: return SmartCommand::V2G_SWITCH_char10;
		case 6: return SmartCommand::V2G_SWITCH_char16;
		case 7: return SmartCommand::V2G_SWITCH_char24;
        default: return SmartCommand::UNKNOWN;
        }
    }
}

const std::string SmartCommand::to_string() const {
    switch(_id) {
    case ERROR: return "ERROR";
    case LED_RED_TOGGLE: return "LED_RED_TOGGLE";
    case LED_GREEN_TOGGLE: return "LED_GREEN_TOGGLE";
    case LED_BLUE_TOGGLE: return "LED_BLUE_TOGGLE";
    case V2G_SWITCH_OVERRIDE: return "V2G_SWITCH_OVERRIDE";
    case V2G_SWITCH_PHYSICAL: return "V2G_SWITCH_PHYSICAL";
    case V2G_SWITCH_IDLE: return "V2G_SWITCH_IDLE";
    case V2G_SWITCH_CHARGE: return "V2G_SWITCH_CHARGE";
    case V2G_SWITCH_DISCHARGE: return "V2G_SWITCH_DISCHARGE";
	case V2G_SWITCH_char0: return "V2G_SWITCH_char0";
	case V2G_SWITCH_char6: return "V2G_SWITCH_char6";
	case V2G_SWITCH_char10: return "V2G_SWITCH_char10";
	case V2G_SWITCH_char16: return "V2G_SWITCH_char16";
	case V2G_SWITCH_char24: return "V2G_SWITCH_char24";
    case MAX_CHARGE:
        return (std::string("MAX_CHARGE ") + std::to_string(max_amps()) + "A").c_str();
    default: return "UNKNOWN";
    }
}

SmartCommand SmartCommand::from_buffer(const char data[2]) {
    switch(data[0]) {
    case 108: return SmartCommand(decode_led_id(data[1]));
    case 115: return SmartCommand(decode_switch_id(data[1]));
    case 99: return SmartCommand(SmartCommand::MAX_CHARGE, data[1]);
    }
    return SmartCommand(SmartCommand::UNKNOWN);
}
