#include "mcc_generated_files/mcc.h"
#include <xc.h>
#include <stdint.h>     // used for uintx_t typedef

void Start_Signal(void);
bool Check_Response();
bool Read_Data(unsigned char* dht_data);