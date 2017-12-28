/* -------------------------------------------------------------------------------

   Copyright (C) 1999-2007 id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   -------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "q3map2.h"



/*
   BSPInfoMain()
   emits statistics about the bsp file
 */

int BSPInfoMain( int count, char **fileNames ){
	int i;
	char source[ 1024 ], ext[ 64 ];
	int size;
	FILE        *f;


	/* dummy check */
	if ( count < 1 ) {
		Sys_Printf( "No files to dump info for.\n" );
		return -1;
	}

	/* enable info mode */
	infoMode = qtrue;

	/* walk file list */
	for ( i = 0; i < count; i++ )
	{
		Sys_Printf( "---------------------------------\n" );

		/* mangle filename and get size */
		strcpy( source, fileNames[ i ] );
		ExtractFileExtension( source, ext );
		if ( !Q_stricmp( ext, "map" ) ) {
			StripExtension( source );
		}
		DefaultExtension( source, ".bsp" );
		f = fopen( source, "rb" );
		if ( f ) {
			size = Q_filelength( f );
			fclose( f );
		}
		else{
			size = 0;
		}

		/* load the bsp file and print lump sizes */
		Sys_Printf( "%s\n", source );
		LoadBSPFile( source );
		PrintBSPFileSizes();

		/* print sizes */
		Sys_Printf( "\n" );
		Sys_Printf( "          total         %9d\n", size );
		Sys_Printf( "                        %9d KB\n", size / 1024 );
		Sys_Printf( "                        %9d MB\n", size / ( 1024 * 1024 ) );

		Sys_Printf( "---------------------------------\n" );
	}

	/* return count */
	return i;
}

int BSPInfoExt( int count, char **fileNames )
{
	int			i, s;
	char		source[ 1024 ], ext[ 64 ];
	int			size;
	FILE		*f;
	const char		*classname;
	
	Sys_Printf( "BSPInfoExt-----------------------\n" );

	/* dummy check */
	if( count < 1 )
	{
		Sys_Printf( "No files to dump info for.\n");
		return -1;
	}
	
	/* enable info mode */
	infoMode = qtrue;
	
	/* walk file list */
	for( i = 0; i < count; i++ )
	{
		Sys_Printf( "---------------------------------\n" );
		
		/* mangle filename and get size */
		strcpy( source, fileNames[ i ] );
		ExtractFileExtension( source, ext );
		if( !Q_stricmp( ext, "map" ) )
			StripExtension( source );
		DefaultExtension( source, ".bsp" );
		f = fopen( source, "rb" );
		if( f )
		{
			size = Q_filelength (f);
			fclose( f );
		}
		else
			size = 0;
		
		/* load the bsp file and print lump sizes */
		Sys_Printf( "%s\n", source );
		LoadBSPFile( source );		

		if( numEntities <= 0 )
			ParseEntities();

		Sys_Printf( "shadercount: %9d\n",	numBSPShaders );
		Sys_Printf( "entitycount: %9d\n",	numEntities );
		if( numBSPLightBytes <= 0 && numBSPGridPoints <= 0 )
			Sys_Printf( "nolightmaps: 0\n" );

		for( s = 0; s < numBSPShaders; s++ )
		{
			Sys_Printf( "bspshader: %i %i %s\n", bspShaders[ s ].surfaceFlags,  bspShaders[ s ].contentFlags, bspShaders[ s ].shader );
		}
		for( s = 0; s < numEntities; s++ )
		{	
			classname = ValueForKey( &entities[ s ], "classname" );
			//Sys_Printf( "bspEntData: %s\n", bspEntData[ s ] );
			Sys_Printf( "classname: %s\n", classname );
		}
		Sys_Printf( "---------------------------------\n" );

	}
	
	/* return count */
	return i;
}