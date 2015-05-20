#ifndef _WbanSecurity
#define _WbanSecurity
#include "Wban.h"
#include "WbanTasks.h"

WBAN_PRIMITIVE SecurityIAckFrameReception( BYTE lastFrameSubtype );
WBAN_PRIMITIVE SecurityMngFrameReception( BYTE frameSubtype );
WBAN_PRIMITIVE SecurityTasks( WBAN_PRIMITIVE inputPrimitive );
WBAN_PRIMITIVE SecurityPrimitiveTransmission( WBAN_PRIMITIVE inputPrimitive );
WBAN_PRIMITIVE TheSecondSendingProcess();
#endif
