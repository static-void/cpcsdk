// We need CapriceUI.cpp to include the CPC Key definitions (they are used in
// the input settings dialog). However, hte file is generated by wxWormBuilder
// and there is no way to add it there (?).
// So we cheat and compile this file instead of CapriceUI.cpp, making sure the
// key definitions are included.
#include "CPCKeyDefs.h"
#include "CapriceUI.cpp"
