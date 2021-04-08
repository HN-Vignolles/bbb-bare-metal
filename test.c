/*
* Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#include "beaglebone.h"
#include "soc_AM335x.h"
#include "interrupt.h"
#include "consoleUtils.h"
#include "hw_types.h"
#include <string.h>
#include "cache.h"
#include "mmu.h"
#include "perf.h"
#include "clock.h"
#if defined(gcc) && defined(__ARM_NEON__)
#include "port_mathLib.h"
#endif


/******************************************************************************
**                      INTERNAL MACRO DEFINITIONS
*******************************************************************************/
/* Definitions related to MMU Configuration. */
#define START_ADDR_DDR                  (0x80000000u)
#define START_ADDR_DEV                  (0x44000000u)
#define START_ADDR_OCMC                 (0x40300000u)
#define NUM_SECTIONS_DDR                (512u)
#define NUM_SECTIONS_DEV                (960u)
#define NUM_SECTIONS_OCMC               (1u)

#define CONST_PI_VAL                    (3.142)
#define VECTOR_SIZE                     (200u)
#define TIME_GRANULARITY                (1000000u)


/******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/

/* Page tables start must be aligned to 16K boundary */
static volatile unsigned int PageTable[MMU_PAGETABLE_NUM_ENTRY]
 __attribute__((aligned(MMU_PAGETABLE_ALIGN_SIZE)));



/****************************************************************************/
/*                      LOCAL FUNCTION PROTOTYPES                           */
/****************************************************************************/

static void _MMUConfigAndEnable(void);



/****************************************************************************/
/*                   FUNCTION DEFINITIONS                             */
/****************************************************************************/

int main(void)
{
    //_MMUConfigAndEnable();
    //CacheEnable(CACHE_ALL);

    ConsoleUtilsInit();
    ConsoleUtilsSetType(CONSOLE_UART);

    ConsoleUtilsPrintf("Hello World\r\n");

    while(1);
}

/*
** Function to setup MMU. This function Maps three regions ( 1. DDR
** 2. OCMC and 3. Device memory) and enables MMU.
*/
void _MMUConfigAndEnable(void)
{
    /*
    ** Define DDR memory region of AM335x. DDR can be configured as Normal
    ** memory with R/W access in user/privileged modes. The cache attributes
    ** specified here are,
    ** Inner - Write through, No Write Allocate
    ** Outer - Write Back, Write Allocate
    */
    REGION regionDdr = {
                        MMU_PGTYPE_SECTION, START_ADDR_DDR, NUM_SECTIONS_DDR,
                        MMU_MEMTYPE_NORMAL_NON_SHAREABLE(MMU_CACHE_WT_NOWA,
                                                         MMU_CACHE_WB_WA),
                        MMU_REGION_NON_SECURE, MMU_AP_PRV_RW_USR_RW,
                        (unsigned int*)PageTable
                       };
    /*
    ** Define OCMC RAM region of AM335x. Same Attributes of DDR region given.
    */
    REGION regionOcmc = {
                         MMU_PGTYPE_SECTION, START_ADDR_OCMC, NUM_SECTIONS_OCMC,
                         MMU_MEMTYPE_NORMAL_NON_SHAREABLE(MMU_CACHE_WT_NOWA,
                                                          MMU_CACHE_WB_WA),
                         MMU_REGION_NON_SECURE, MMU_AP_PRV_RW_USR_RW,
                         (unsigned int*)PageTable
                        };

    /*
    ** Define Device Memory Region. The region between OCMC and DDR is
    ** configured as device memory, with R/W access in user/privileged modes.
    ** Also, the region is marked 'Execute Never'.
    */
    REGION regionDev = {
                        MMU_PGTYPE_SECTION, START_ADDR_DEV, NUM_SECTIONS_DEV,
                        MMU_MEMTYPE_DEVICE_SHAREABLE,
                        MMU_REGION_NON_SECURE,
                        MMU_AP_PRV_RW_USR_RW  | MMU_SECTION_EXEC_NEVER,
                        (unsigned int*)PageTable
                       };

    /* Initialize the page table and MMU */
    MMUInit((unsigned int*)PageTable);

    /* Map the defined regions */
    MMUMemRegionMap(&regionDdr);
    MMUMemRegionMap(&regionOcmc);
    MMUMemRegionMap(&regionDev);

    /* Now Safe to enable MMU */
    MMUEnable((unsigned int*)PageTable);
}
