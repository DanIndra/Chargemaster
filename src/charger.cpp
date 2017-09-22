#include <algorithm>

#include "charger.hpp"
#include "log.hpp"

namespace {
    const float DUTY_CYCLE_PERIOD_US = 1000.0;
    const float MIN_AMPS = 6.0;
    const float MAX_AMPS = 70.0;
	DigitalOut 		relay1	= (D12);
	DigitalOut		relay2  = (D13);
	DigitalOut 		relay3  = (D14);
	DigitalOut		relay4  = (D15);

    template<class T>
    const T& clamp(const T& x, const T& upper, const T& lower) {
        return std::min(upper, std::max(x, lower));
    }

    float amps_to_duty_cycle_percent(float n) {
        return n / MAX_AMPS;
    }

    float inverted_duty_cycle(float n) {
        return 1.0 - n;
    }

    int duty_cycle_to_pattern_delay(float n) {
        return 1005 - (1000 * n);
    }
};

Charger::Charger(Display& display, PinName input_pin, PinName output_pin) : _state(IDLE), _amps(DEFAULT_AMPS), _display(display), _pwmin(input_pin), _pwmout(output_pin)  {
    _display.pattern(Display::IDLE);
    _pwmout.period_us(DUTY_CYCLE_PERIOD_US);
    _pwmout.pulsewidth_us(0);
}

Charger::ChargerState Charger::state() const {
    return _state;
}
void Charger::charge6() {
	_state = char6;
	relay1 = 1;
	relay2 = 0;
	relay3 = 0;
	relay4 = 0;
}
void Charger::charge10() {
	_state = char10;
	relay1 = 0;
	relay2 = 1;
	relay3 = 0;
	relay4 = 0;
}
void Charger::charge16() {
	_state = char16;
	relay1 = 0;
	relay2 = 0;
	relay3 = 1;
	relay4 = 0;
}
void Charger::charge24() {
	_state = char24;
	relay1 = 0;
	relay2 = 0;
	relay3 = 0;
	relay4 = 1;
}
void Charger::charge0() {
	_state = char0;
	relay1 = 1;
	relay2 = 1;
	relay3 = 1;
	relay4 = 1;
}
void Charger::idle() {
    _state = IDLE;
    _display.pattern(Display::IDLE);
    relay1 = 1;
	relay2 = 0;
}

void Charger::charge() {
    _state = CHARGE;
    relay1 = 0;
	relay2 = 1;
}

float Charger::amps() const {
    return _amps;
}

void Charger::amps(float amps) {
    float clamped_amps = clamp(amps, MAX_AMPS, MIN_AMPS);
    _amps = clamped_amps;
    effect_charge_state();
}

void Charger::effect_charge_state() {
    if(_state != CHARGE) return;

    float requested_duty_cycle = amps_to_duty_cycle_percent(_amps);

    // TODO: should we do this some other time? i.e. when should we be sampling the pwmin?
    float allowed_duty_cycle = std::min(requested_duty_cycle, _pwmin.dutycycle());

    log("input duty cycle: %0.2f\r\n", _pwmin.dutycycle());
    log("requested duty cycle: %0.2f\r\n", requested_duty_cycle);
    log("allowed duty cycle: %0.2f\r\n", allowed_duty_cycle);

    float dc = inverted_duty_cycle(allowed_duty_cycle);

    log("%0.2f amps == %0.2f inverted duty cycle, %d us\r\n", _amps, dc, (DUTY_CYCLE_PERIOD_US * dc));

    _display.timed_pattern(Display::CHARGE, duty_cycle_to_pattern_delay(allowed_duty_cycle));

    _pwmout.write(dc);

    return;
}
