#ifndef SERVICE_H
#define SERVICE_H

#include <sdbus-c++/sdbus-c++.h>

void registerService(sdbus::MethodCall call);

void openFile(sdbus::MethodCall call);

void openFileUsingService(sdbus::MethodCall call);

#endif