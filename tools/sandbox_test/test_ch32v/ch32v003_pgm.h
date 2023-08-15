#ifndef _CH32V003PGM_H
#define _CH32V003PGM_H

#define DisableISR()            do { XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); portbenchmarkINTERRUPT_DISABLE(); } while (0)
#define EnableISR()             do { portbenchmarkINTERRUPT_RESTORE(0); XTOS_SET_INTLEVEL(0); } while (0)
#define MAX_IN_TIMEOUT 1000
#define IO_MUX_REG(x) XIO_MUX_REG(x)
#define XIO_MUX_REG(x) IO_MUX_GPIO##x##_REG
#define GPIO_NUM(x) XGPIO_NUM(x)
#define XGPIO_NUM(x) GPIO_NUM_##x
#define R_GLITCH_HIGH

#include "ch32v003_swio.h"

static int Setup003( struct SWIOState * state )
{

	//CH32V003 Programming
	REG_WRITE( IO_MUX_REG(SWIO_PIN), 1<<FUN_IE_S | 1<<FUN_PU_S | 1<<FUN_DRV_S );  //Additional pull-up, 10mA drive.  Optional: 10k pull-up resistor. This is the actual SWIO.
	REG_WRITE( IO_MUX_REG(RESET_PIN), 1<<FUN_IE_S | 1<<FUN_DRV_S );

	GPIO.out_w1tc = 1<<RESET_PIN;
	memset( state, 0, sizeof( *state ) );
	state->t1coeff = 10;
	state->pinmask = 1<<SWIO_PIN;
	vTaskDelay( 1 );
	GPIO.out_w1ts = 1<<RESET_PIN;

	ResetInternalProgrammingState( state );
	int i;
	for( i = 0; i < 10; i++ )
	{
		MCFWriteReg32( state, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Shadow Config Reg
		MCFWriteReg32( state, DMCFGR, 0x5aa50000 | (1<<10) ); // CFGR (1<<10 == Allow output from slave)
		MCFWriteReg32( state, DMCFGR, 0x5aa50000 | (1<<10) ); // Bug in silicon?  If coming out of cold boot, and we don't do our little "song and dance" this has to be called.
		MCFWriteReg32( state, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCFWriteReg32( state, DMCONTROL, 0x80000001 );
		MCFWriteReg32( state, DMCONTROL, 0x80000001 );
	}

	uint32_t rrv;
	return MCFReadReg32( state, DMSTATUS, &rrv );
}

static int WriteImage003( struct SWIOState * state, uint8_t * binary, int length )
{
	const uint32_t flash_offset = 0x08000000;
	// First see if program already matches.
	int bp;
	for( bp = 0; bp < length; bp += 4 )
	{
		uint8_t readval[4] = { 0 };
		int r = ReadWord( state, bp + flash_offset, (uint32_t*)readval );
		if( r ) return -44;
		int remchk = length - bp;
		if( remchk > 4 ) remchk = 4;
		if( memcmp( readval, binary + bp, remchk ) ) break;
	}

	if( bp >= length )
	{
		// Program already matches. Do not flash.
		return 1;
	}

	// Otherwise need to flash.

	UnlockFlash( state );
	int r;
	r = EraseFlash( state, flash_offset, length, 1 ); // 1 = whole chip.
	if( r ) return -7;
	for( bp = 0; bp < length; bp += 64 )
	{
		uint8_t block[64] = { 0 };
		int remchk = length - bp;
		if( remchk > 64 ) remchk = 64;
		memcpy( block, binary+bp, remchk );
		int r = Write64Block( state, bp + flash_offset, block );
		if( r ) return -5;
	}
	return 0;
}

static int Boot003( struct SWIOState * state )
{
	MCFWriteReg32( state, DMCONTROL, 0x80000001 ); // Initiate a halt request.
	MCFWriteReg32( state, DMCONTROL, 0x80000003 ); // Reboot.
	MCFWriteReg32( state, DMCONTROL, 0x40000001 ); // resumereq
	return 0;
}

#endif

