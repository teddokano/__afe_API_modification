/*
 *  @author Tedd OKANO
 *
 *  Released under the MIT license
 */

#include	"r01lib.h"
#include	"afe/NAFE13388_UIM.h"

SPI				spi( D11, D12, D13, D10 );	//	MOSI, MISO, SCLK, CS
NAFE13388_UIM	afe( spi );

class LogicalChannel
{
public:
	AFE_base&	afe;
	uint8_t		ch_number;

	LogicalChannel( AFE_base& a, uint8_t ch ) : afe( a ), ch_number( ch ) {}
	long	start_and_read( void ) { return afe.start_and_read( ch_number ); }
	double	raw2uv( long raw ) { return afe.raw2uv( ch_number, raw ); }
};

int main( void )
{
	printf( "***** Hello, NAFE13388 UIM board! *****\r\n" );

	spi.frequency( 1'000'000 );
	spi.mode( 1 );

	afe.begin();

	afe.open_logical_channel( 0, 0x1710, 0x00A4, 0xBC00, 0x0000 );
	afe.open_logical_channel( 1, 0x2710, 0x00A4, 0xBC00, 0x0000 );

	LogicalChannel	lc[ 2 ]	= {
			LogicalChannel( afe, 0 ),
			LogicalChannel( afe, 1 )
	};

	afe.use_DRDY_trigger( false );	//	default = true

	printf( "\r\nenabled logical channel(s) %2d\r\n", afe.enabled_logical_channels() );

	while ( true )
	{
		for ( auto ch = 0; ch < 2; ch++ )
		{
			int32_t	data	= lc[ ch ].start_and_read();
			printf( "   channel %2d : %8ld (%lfuV),", ch, data, lc[ ch ].raw2uv( data ) );
		}
		printf( "\r\n" );
	}
}
