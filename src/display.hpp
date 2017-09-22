#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "mbed.h"

/** Device display.
 *
 * Currently this is just a set of different LED patterns to inform the user that the device is in a particular state.
 */
class Display {

public:
    /** Available patterns.
     */
    enum LedPattern { INIT,
                      OFF,
                      ERROR,
                      STARTUP,
                      SHUTDOWN,
                      IDLE,
                      CHARGE };

    /** ctor
     */
    Display();

    /** dtor
     */
    ~Display();

    /** Current pattern.
     */
    LedPattern pattern() const { return _pattern; }

    int delay() { return _delay; }

    /** Set pattern.
     */
    void pattern(LedPattern p);

    /** Set pattern.
     */
    void timed_pattern(LedPattern p, int ms);

    /** Start displaying pattern.
     */
    void start();

    /** Stop displaying pattern.
     */
    void stop();

private:
    LedPattern _pattern;
    Thread _runner;
    int _delay;
};

#endif
