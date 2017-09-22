#include "display.hpp"
#include "log.hpp"

namespace {
    DigitalOut grn(LED1);
    DigitalOut blu(LED2);
    DigitalOut red(LED3);

    void led_off_pattern() {
        grn = 0;
        blu = 0;
        red = 0;
        Thread::wait(1000);
    }

    void led_error_pattern() {
        grn = 0;
        blu = 0;
        red = 1;
        Thread::wait(250);
        grn = 0;
        blu = 0;
        red = 0;
        Thread::wait(250);
    }

    void led_startup_pattern() {
        grn = 1;
        blu = 0;
        red = 0;
        Thread::wait(100);
        grn = 0;
        blu = 1;
        red = 0;
        Thread::wait(100);
        grn = 0;
        blu = 0;
        red = 1;
        Thread::wait(100);
    }

    void led_shutdown_pattern() {
        grn = 1;
        blu = 1;
        red = 1;
        Thread::wait(300);
        grn = 0;
        blu = 0;
        red = 0;
        Thread::wait(300);
    }

    void led_idle_pattern() {
        grn = 0;
        blu = 1;
        red = 0;
        Thread::wait(500);
    }

    void led_charge_pattern(int ms) {
        grn = 1;
        blu = 0;
        red = 0;
        Thread::wait(ms);
        grn = 0;
        blu = 0;
        red = 0;
        Thread::wait(ms);
    }

    void led_discharge_pattern() {
        grn = 0;
        blu = 0;
        red = 1;
        Thread::wait(500);
    }

    void display_thread(Display* c) {
        log("display thread start\r\n");
        bool running = true;
        while(running) {
            switch(c->pattern()) {
            case Display::INIT: led_off_pattern();
                break;
            case Display::OFF:
                led_off_pattern();
                running = false;
                break;
            case Display::ERROR: led_error_pattern();
                break;
            case Display::STARTUP: led_startup_pattern();
                break;
            case Display::SHUTDOWN: led_shutdown_pattern();
                break;
            case Display::IDLE: led_idle_pattern();
                break;
            case Display::CHARGE:
                led_charge_pattern(c->delay());
                break;
            default: led_error_pattern();
            }
        }

        log("display thread finish\r\n");
        return;
    }
}

Display::Display() : _pattern(INIT), _delay(1000) {
    grn = 0;
    blu = 0;
    red = 0;
}

Display::~Display() {
    stop();
}

void Display::start() {
    _runner.start(callback(display_thread,this));
    return;
}

void Display::stop() {
    _pattern = OFF;
    osStatus rv =_runner.join();

    log("display threaed joined rv: %d\r\n", rv);
    return;
}

void Display::pattern(LedPattern p) {
    _pattern = p;
    _delay = 1000;
}

void Display::timed_pattern(LedPattern p, int ms) {
    pattern(p);
    _delay = ms;
}
