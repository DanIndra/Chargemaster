#ifndef SMART_COMMAND_HPP
#define SMART_COMMAND_HPP

#include <string>

/** Smart Commands.
 */
class SmartCommand {
public:
    /** Command ID.
     */
    enum SmartCommandId {
        ERROR,

        UNKNOWN,

        LED_RED_TOGGLE, // (new byte[]{108, 1}),
        LED_GREEN_TOGGLE, // (new byte[]{108, 2}),
        LED_BLUE_TOGGLE, // (new byte[]{108, 3}),

        V2G_SWITCH_OVERRIDE, // (new byte[]{115, 101}),
        V2G_SWITCH_PHYSICAL, // (new byte[]{115, 100}),
        V2G_SWITCH_IDLE, //(new byte[]{115, 0}),
        V2G_SWITCH_CHARGE, // (new byte[]{115, 1}),
        V2G_SWITCH_DISCHARGE, // (new byte[]{115, 2}),
		V2G_SWITCH_char0, // (new byte[] {115, 3}),
		V2G_SWITCH_char6, // (new byte[] {115, 4}),
		V2G_SWITCH_char10, // (new byte[] {115, 5}),
		V2G_SWITCH_char16, // (new byte[] {115, 6}),
		V2G_SWITCH_char24, // (new byte[] {115, 7}),

        MAX_CHARGE // (new byte[]{99, n}),
    };

    /** ctor
     */
    SmartCommand(SmartCommandId id) : _id(id), _max_amps(0.0f) {}

    SmartCommand(SmartCommandId id, float max_amps) : _id(id), _max_amps(max_amps) {}

    /** Command ID
     */
    SmartCommandId id() const { return _id; }

    float max_amps() const { return _max_amps; }

    const std::string to_string() const;

    /** Parse command from buffer
     */
    static SmartCommand from_buffer(const char data[2]);

private:
    SmartCommandId _id;
    float _max_amps;
};

#endif
