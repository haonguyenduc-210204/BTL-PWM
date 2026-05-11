/*
 *  Copyright 2010 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  LED header file
 *
 */

#ifndef LED_
#define LED_

#include "ezdsp5535.h"

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 EZDSP5535_LED_init  ( );
Int16 EZDSP5535_LED_on    ( Uint16 number );
Int16 EZDSP5535_LED_off   ( Uint16 number );

Int16 EZDSP5535_ULED_init( );
Int16 EZDSP5535_ULED_getall( Uint16 *pattern );
Int16 EZDSP5535_ULED_setall( Uint16 pattern );
Int16 EZDSP5535_ULED_on( Uint16 number );
Int16 EZDSP5535_ULED_off( Uint16 number );
Int16 EZDSP5535_ULED_toggle( Uint16 number );

#endif
