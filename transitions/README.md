# Transitions

Transitions are similar to style templates, but are intended to blend between two different colors and run for a finite time. Transition classes are required to have four functions:

# void begin()

Called to start a transition.
Can be called again and and again to re-start a transition, even if the transition is not done yet.

# bool done()

Should return true when the transition is done.
When the transition is done getColor() should return the "b" color for all leds.

# void run(BladeBase* blade);

Called to start a frame.
It's ok to put semi-complicated calculations here, but not in getColor.

# OverDriveColor getColor(const OverDriveColor& a, const OverDriveColor& b, int led)

Called once per LED on the blade to calculate the color for that led.
The inputs are the colors to transition between.
Generally it's best to avoid complicated calculations in getColor since
it will be called thousands of times per second.
