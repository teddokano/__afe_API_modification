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

	LogicalChannel( AFE_base& a, uint8_t ch, uint16_t cc0, uint16_t cc1, uint16_t cc2, uint16_t cc3 ) : afe( a ), ch_number( ch )
	{
		afe.open_logical_channel( ch_number, cc0, cc1, cc2, cc3 );
	}

	template<class T> T read(void);
	template<class T> operator T(void);

	double	raw2uv( long raw ) { return afe.raw2v( ch_number, raw ); }
};


template<>
int32_t	LogicalChannel::read( void )
{
	return afe.start_and_read( ch_number );
}

template<>
double LogicalChannel::read( void )
{
	int32_t	v	= read<int32_t>();
	return afe.raw2v( ch_number, v );
}

template<>
LogicalChannel::operator int32_t( void )
{
	return read<int32_t>();
}

template<>
LogicalChannel::operator double( void )
{
	return read<double>();
}



int main( void )
{
	printf( "***** Hello, NAFE13388 UIM board! *****\r\n" );

	spi.frequency( 1'000'000 );
	spi.mode( 1 );

	afe.begin();

	LogicalChannel	lc[ 2 ]	= {
			LogicalChannel( afe, 0, 0x1710, 0x00A4, 0xBC00, 0x0000 ),
			LogicalChannel( afe, 1, 0x2710, 0x00A4, 0xBC00, 0x0000 )
	};

	afe.use_DRDY_trigger( false );	//	default = true

	printf( "\r\nenabled logical channel(s) %2d\r\n", afe.enabled_logical_channels() );

	while ( true )
	{
		for ( auto ch = 0; ch < 2; ch++ )
		{
			int32_t	datai	= lc[ ch ];
			double	dataf	= lc[ ch ];
			printf( "   channel %2d : %8ld (%lfV),", ch, datai, dataf );
		}
		printf( "\r\n" );
	}
}
