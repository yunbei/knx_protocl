#include <stdlib.h>
#include "KnxInterface.h"

DATA_TYPE pdu_alloc()
{
    DATA_TYPE pdu = (DATA_TYPE)malloc(120); // FIXME: buffer alloc

    return pdu;
}