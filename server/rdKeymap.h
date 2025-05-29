
#ifndef KEYMAP_H__
#define KEYMAP_H__

#include "rfb.h"

class rdKeymap {
public:
  static void keyEvent(CARD32 keysym, bool down);
  static void ClearShiftKeys();
};


#endif
