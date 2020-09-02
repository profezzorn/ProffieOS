#ifndef FUNCTIONS_WAV_EFFECT_H
#define FUNCTIONS_WAV_EFFECT_H

#include "../blades/blade_base.h"


class WavEffect{
public:	

  const char* GetEffectName(BladeEffectType type) {
	  switch (type){
	   case EFFECT_CLASH:      	return "clash,clsh";
	   case EFFECT_BLAST:      	return "blaster,blst";
	   case EFFECT_FORCE:      	return "force";
	   case EFFECT_STAB:       	return "stab";
	   case EFFECT_BOOT:	   	return "boot";
	   case EFFECT_LOCKUP_BEGIN:return "bgnlock";
	   case EFFECT_LOCKUP_END: 	return "endlock";
	   case EFFECT_DRAG_BEGIN:	return "bgndrag";
	   case EFFECT_DRAG_END:	return "enddrag";
	   case EFFECT_PREON:      	return "preon";
	   case EFFECT_PSTOFF:     	return "pstoff";
	   case EFFECT_IGNITION:   	return "poweron,out,poweronf";//including force poweron
	   case EFFECT_RETRACTION: 	return "poweroff,in";
	   case EFFECT_CHANGE:	   	return "ccbegin,ccchange,ccend";
	   case EFFECT_NEWFONT:    	return "font";
	   
	   case EFFECT_STUN:	   	return "stun";
	   case EFFECT_FIRE:	   	return "none";//no effect defined
	   case EFFECT_CLIP_IN:	   	return "clipin";
	   case EFFECT_CLIP_OUT:   	return "clipout";
	   case EFFECT_RELOAD:	   	return "reload";
	   case EFFECT_MODE:	   	return "mode";
	   case EFFECT_RANGE:	   	return "range";
	   case EFFECT_EMPTY:	   	return "empty";
	   case EFFECT_FULL:	   	return "full";
	   case EFFECT_JAM:		   	return "jam";
	   case EFFECT_UNJAM:	   	return "unjam";
	   case EFFECT_PLI_ON:	   	return "plion";
	   case EFFECT_PLI_OFF:	   	return "plioff";
	   case EFFECT_NONE:	   	return "none";
	  } 
	 return "none"; 
	  
  }

};

const char* pstoff_ = "pstoff";

#endif
/*  blade_base.h
#define DEFINE_ALL_EFFECTS()			\
    DEFINE_EFFECT(NONE)				\
    DEFINE_EFFECT(CLASH)			\
    DEFINE_EFFECT(BLAST)			\
    DEFINE_EFFECT(FORCE)			\
    DEFINE_EFFECT(STAB)				\
    DEFINE_EFFECT(BOOT)				\
    DEFINE_EFFECT(LOCKUP_BEGIN)			\
    DEFINE_EFFECT(LOCKUP_END)			\
    DEFINE_EFFECT(DRAG_BEGIN)			\
    DEFINE_EFFECT(DRAG_END)			\
    DEFINE_EFFECT(PREON)			\
    DEFINE_EFFECT(PSTOFF)			\
    DEFINE_EFFECT(IGNITION)			\
    DEFINE_EFFECT(RETRACTION)			\
    DEFINE_EFFECT(CHANGE)			\
    DEFINE_EFFECT(NEWFONT)			\
*/    /* Blaster effects */                       \
/*    DEFINE_EFFECT(STUN)				\
    DEFINE_EFFECT(FIRE)				\
    DEFINE_EFFECT(CLIP_IN)			\
    DEFINE_EFFECT(CLIP_OUT)			\
    DEFINE_EFFECT(RELOAD)			\
    DEFINE_EFFECT(MODE)				\
    DEFINE_EFFECT(RANGE)			\
    DEFINE_EFFECT(EMPTY)			\
    DEFINE_EFFECT(FULL)				\
    DEFINE_EFFECT(JAM)				\
    DEFINE_EFFECT(UNJAM)			\
    DEFINE_EFFECT(PLI_ON)			\
    DEFINE_EFFECT(PLI_OFF)
*/

/* effect.h
EFFECT(preon);
EFFECT(pstoff);

// Monophonic fonts
EFFECT(boot);     // also polyphonic
EFFECT(bladein);  // also polyphonic
EFFECT(bladeout);  // also polyphonic
EFFECT2(hum, hum);
EFFECT2(humm, humm);
EFFECT(swing);
EFFECT(poweron);
EFFECT2(poweroff, pstoff);
EFFECT2(pwroff, pstoff);
EFFECT(clash);
EFFECT(force);    // also polyphonic
EFFECT(stab);     // also polyphonic
EFFECT(blaster);
EFFECT2(lockup, lockup);
EFFECT(poweronf); // force poweron
EFFECT(font);     // also polyphonic
EFFECT(bgnlock);  // monophonic and polyphonic begin lock
EFFECT(endlock);  // Plecter endlock support, used for polyphonic name too

// Polyphonic fonts
EFFECT(blst);
EFFECT(clsh);
EFFECT2(in, pstoff);
EFFECT(out);
EFFECT2(lock, lock);
EFFECT(swng);  // -> no blade effect defined yet?
EFFECT(slsh);  // -> no blade effect defined yet?

// Looped swing fonts. (SmoothSwing V1/V2)
EFFECT2(swingl, swingl);  // Looped swing, LOW
EFFECT2(swingh, swingh);  // Looped swing, HIGH
EFFECT2(lswing, lswing);  // Looped swing, LOW (plecter naming)
EFFECT2(hswing, hswing);  // Looped swing, HIGH (plecter naming)

// Drag effect, replaces "lock/lockup" in drag mode if present.
EFFECT(bgndrag);
EFFECT2(drag, drag);
EFFECT(enddrag);

// Melt is like drag, but for door melting. Falls back to "drag".
EFFECT(bgnmelt);
EFFECT2(melt, melt);
EFFECT(endmelt);

// Lightning block is like "lockup", but for blocking force lightning.
// Falls back to standard lockup.
EFFECT(bgnlb);
EFFECT2(lb, lb);
EFFECT(endlb);

// Detonator effects
EFFECT(bgnarm);
EFFECT2(armhum, armhum);
EFFECT(endarm);
EFFECT(boom);

// Color change
EFFECT(color);
EFFECT(ccbegin);
EFFECT(ccend);
EFFECT(ccchange);

// Blaster effects
// hum, boot and font are reused from sabers and already defined.
EFFECT(bgnauto); // Doesn't exist in fonts, but I expect there may be use for autofire transitions
EFFECT2(auto,auto);
EFFECT(endauto); // Doesn't exist in fonts, but I expect there may be use for autofire transitions

EFFECT(blast); // Not to be confused with "blst" and "blaster" as blocking sounds in sabers
EFFECT(clipin);
EFFECT(clipout);
EFFECT(empty);
EFFECT(full);
EFFECT(jam);
EFFECT(mode);
EFFECT(plioff);
EFFECT(plion);
EFFECT(range);
EFFECT(reload);
EFFECT(stun);
EFFECT(unjam);

// battery low
EFFECT(lowbatt);	// battery low
*/