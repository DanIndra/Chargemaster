#ifndef CHARGER_HPP
#define CHARGER_HPP

#include "mbed.h"
#include "PwmIn.h"
#include "display.hpp"

/** Fake charger.
 *
 * Charger stub, uses LEDs to inform current action.
 */
class Charger {

    const float DEFAULT_AMPS = 32.0f;

public:
    /** Possible states.
     */
    enum ChargerState { IDLE = 23, CHARGE = 42, ERROR = 255, char6 = 10, char10 = 12, char16 = 14, char24 = 16, char0 = 18 };

    /** ctor
     */
    Charger(Display& display, PinName input_pin, PinName output_pin);

	/** Charge at 6 amps.
	*/
	void charge6();
	
	/** Charge at 10.
	*/
	void charge10();
	
	/** Charge at 16 amps.
	*/
	void charge16();
	
	/** Charge at 24 amps.
	*/
	void charge24();
	
	/** Charger goes into 0 amps.
	*/
	void charge0();
	
	
    /** Current state.
     */
    ChargerState state() const;

    /** Set to idle.
     */
    void idle();

    /** Set to charge device from supply.
     */
    void charge();

    float amps() const;

    void amps(float amps);
private:

    void effect_charge_state();

    ChargerState _state;
    float _amps;
    Display& _display;
    PwmIn _pwmin;
    PwmOut _pwmout;
};

#endif
