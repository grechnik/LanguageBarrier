#ifndef __SIGSCAN_H__
#define __SIGSCAN_H__

#include <cstdint>

namespace lb {
uintptr_t sigScan(char* category, char* sigName, bool isData = false);
}

#endif  // !__SIGSCAN_H__
