# This is my personal version of ProffieOS, based on POS 8.4x Beta from Github Master (February 2026)
Changes includes, but not limited to:
- muliple blade detect code added in: part 1/5 to events.h 2/5 to config, part 3/5 to props,
  part 4/5 to ProffieOS.ino & part 5A/5, 5B/5 & 5C/5 to prop_base.h
- keep minimum talkie only: "no sd", "no font" & "font error" instead of "sd card not found",
  "font directory not found" & "error in font directory" the rest is either absent or replaced.
- config/olicomplex1.6.03.5_BT_re-organized.h added (my config - as a multi_prop config example)
- prop/detonator_Oli_buttons.h                added
- prop/jetpack_Oli_buttons.h                  added
- prop/multi_prop.h                           added
- code added to:
  - ProffieOS.ino                (for multiple blade detect compatibility)
  - buttons/button_base.h        (for multi_prop compatibility)
  - common/events.h              (for multiple blade detect & multi_prop compatibility)
  - common/saber_base.h          (for multi_prop, detonator_Oli_buttons & jetpack_Oli_buttons compatibility)
  - common/serial.h              (because I like the customized welcome info when I open serial monitor in Arduino)
  - props/blaster_BC_buttons.h   (for multi_prop compatibility)
  - props/detonator.h            (for multi_prop compatibility)
  - props/dual_prop.h            (based on potential issues I identified with multi_prop)
  - props/saber_caiwin_buttons.h (for multi_prop compatibility)
  - sound/sound_library.h        (for detonator_Oli_buttons & jetpack_Oli_buttons compatibility)

... changes to more props comming (for multi_prop and multiple blade detect compatibility)

# ProffieOS

The open source operating system. Proffie OS is supported on various platforms ranging from Teensy 3.2 development boards to its own dedicated ProffieBoard reference hardware.

Proffie OS supports:
- :fire: SmoothSwing V1/V2 Algorithm
- :fire: NEC styled lightsaber sound fonts (polyphonic)
- :fire: Plecter styled lightsaber sound fonts ( monophonic)
- :fire: Driving Adressable LED strips
- :fire: Driving Segmented LED strips
- :fire: Quad/Tri LED stars.

### Getting started  
* ProffieOS Documentation: https://pod.hubbe.net/
* ProffieOS: https://fredrik.hubbe.net/lightsaber/proffieos.html
* Proffieboard v1.5: https://fredrik.hubbe.net/lightsaber/v4
* Proffieboard v2.2: https://fredrik.hubbe.net/lightsaber/v5
* Proffieboard v3.9: https://fredrik.hubbe.net/lightsaber/v6
* TeensySaber: http://fredrik.hubbe.net/lightsaber/v3/
* Support forum: http://crucible.hubbe.net
