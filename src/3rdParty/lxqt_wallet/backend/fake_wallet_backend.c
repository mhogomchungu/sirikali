/*
 *
 *  Copyright (c) 2017
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lxqtwallet.h"

int lxqt_wallet_exists( const char * wallet_name,
			const char * application_name )
{
	if( wallet_name && application_name ){}

	return 0 ;
}

lxqt_wallet_error lxqt_wallet_delete_wallet( const char * wallet_name,
					     const char * application_name )
{
	if( wallet_name && application_name ){}
	return lxqt_wallet_no_error ;
}

void lxqt_wallet_application_wallet_path( char * path_buffer,
					  uint32_t path_buffer_size,
					  const char * application_name )
{
	if( path_buffer && path_buffer_size && application_name ){}
}

