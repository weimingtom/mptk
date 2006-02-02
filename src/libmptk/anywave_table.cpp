/******************************************************************************/
/*                                                                            */
/*                            anywave_table.cpp                               */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* R�mi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Tue Nov 03 2005 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2005 IRISA                                                  */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or             */
/*  modify it under the terms of the GNU General Public License               */
/*  as published by the Free Software Foundation; either version 2            */
/*  of the License, or (at your option) any later version.                    */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330,                            */
/*  Boston, MA  02111-1307, USA.                                              */
/*                                                                            */
/******************************************************************************/
/*
 * CVS log:
 *
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */

#include "mptk.h"
#include "mp_system.h"
#include <iostream.h>
/***********************************************************/
/*                                                         */
/* anywave_table.cpp: methods for class MP_Anywave_Table_c */
/*                                                         */
/***********************************************************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Anywave_Table_c::MP_Anywave_Table_c( void ) {
  set_null();
}

/********************/
/* FILE constructor */
MP_Anywave_Table_c::MP_Anywave_Table_c( FILE* pFile ) {

  set_null();
  /* parsing the file pFile */
  if ( parse_file(pFile) == false ) {
    mp_error_msg( "MP_Anywave_Table_c::MP_Anywave_Table_c", "Can't parse the file - Default initialization by 'void constructor'.\n");
    reset();      
  }
}

/************************/
/* fileName constructor */
MP_Anywave_Table_c::MP_Anywave_Table_c( char* fileName ) {

  set_null();
  FILE * pFile = NULL;
  pFile = fopen (fileName,"rt");
  if ( pFile == NULL) {
    mp_error_msg( "MP_Anywave_Table_c::MP_Anywave_Table_c", "Can't open the file %s - Default initialization by 'void constructor'.\n", fileName );
    reset();
  } else if (set_table_file_name( fileName ) == NULL) {
    mp_error_msg( "MP_Anywave_Table_c::MP_Anywave_Table_c", "Can't alloc tableFileName to the size of fileName : %s  - Default initialization by 'void constructor'.\n", fileName );
    reset();
  } else {
    if ( parse_file(pFile) == false ) {
      mp_error_msg( "MP_Anywave_Table_c::MP_Anywave_Table_c", "Can't parse the file %s - Default initialization by 'void constructor'.\n", fileName );
      reset();      
    }
  }
  fclose(pFile);
}

/**************/
/* destructor */
MP_Anywave_Table_c::~MP_Anywave_Table_c() {
#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Anywave_Table_c -- Entering the anywave table destructor.\n" );
#endif

  reset();

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Anywave_Table_c -- Exiting the anywave table destructor.\n" );
#endif
}

/***************************/
/* OTHER METHODS           */
/***************************/

/* Test */
bool MP_Anywave_Table_c::test( char* filename ) {
  
  unsigned long int sampleIdx;

  fprintf( stdout, "\n\n-- Entering MP_Anywave_Table_c::test \n" );

  /* Create a Anywave_Table */
  MP_Anywave_Table_c* tablePtr = new MP_Anywave_Table_c( filename );
  if (tablePtr == NULL) {
    return(false);
  }
  /* print the table */
  fprintf( stdout, "---- Printing the table:\n" );
  tablePtr->print( stdout );

  /* print the first channel of the first filter before normalization */
  fprintf( stdout, "---- Printing the 10 first samples of the first channel of the first filter before normalization:\n" );
  
  for ( sampleIdx = 0;
	(sampleIdx < tablePtr->filterLen) && (sampleIdx < 10);
	sampleIdx ++) {
    fprintf( stdout, "%lf ", *(tablePtr->wave[0][0]+sampleIdx));
  }
  fprintf( stdout, "\n" );

  tablePtr->normalize();

  /* print the first channel of the first filter before normalization */
  fprintf( stdout, "---- Printing the 10 first samples of the first channel of the first filter after normalization:\n" );

  for ( sampleIdx = 0;
	(sampleIdx < tablePtr->filterLen) && (sampleIdx < 10);
	sampleIdx ++) {
    fprintf( stdout, "%lf ", *(tablePtr->wave[0][0]+sampleIdx));
  }
  fprintf( stdout, "\n" );
  

  /* Destroy the anywave server */
  delete( tablePtr );

  fprintf( stdout, "\n-- Exiting MP_Anywave_Table_c::test \n" );
  fflush( stdout );
  return( true );
  
}

/* load the data contained in dataFileName, store it in
      storage and update the pointers in wave */
bool MP_Anywave_Table_c::load_data( void ) {

  FILE * pFile = NULL;
  unsigned long int numElements;
  unsigned long int filterIdx;
  unsigned short int chanIdx;  
  MP_Sample_t *pSample;

  /* if storage is not empty, do nothing */
  if ( storage != NULL ) {
    mp_error_msg( "MP_Anywave_Table_c::load_data", "the storage is already full. We leave this and do nothing\n" );
    return(false);
  } else {
    /* verify that all the parameters are available */
    if ( (numChans == 0) || (filterLen == 0) || (numFilters == 0) || (dataFileName == NULL) ) {
      mp_error_msg( "MP_Anywave_Table_c::load_data", "can't try to load the waveform data, missing parameters. Leave the storage empty\n" );
      return(false);
    } else {
      /* try to open the file */
      if ((pFile = fopen (dataFileName,"rb")) == NULL ) {
	mp_error_msg( "MP_Anywave_Table_c::load_data", "Can't open the file %s - Leave the storage empty'.\n", dataFileName );
	return(false);
      } else {
	/* number of elements to be read in the data file */
	if ((numChans != 0) && (numFilters != 0) && (filterLen != 0)) {
	  /* check that multiplying the three dimensions will not go over the maximum size of an unsigned long int */
	  if ( ((double)MP_MAX_SIZE_T / (double) numChans / (double) numFilters / (double) filterLen) / (double)sizeof(MP_Sample_t) <= 1.0) {
	    mp_error_msg( "MP_Anywave_Table_c::load_data", "numChans [%lu] . numFilters [%lu] . filterLen [%lu] . sizeof(MP_Sample_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for storing the anywave filters. Exiting from load_data().\n", numChans, numFilters, filterLen, sizeof(MP_Sample_t), MP_MAX_SIZE_T);
	    return( false );
	  }
	}
	numElements = numChans * numFilters * filterLen;
	
	/* try to allocate room for storage */
	if ( ( storage = (MP_Sample_t *)malloc( numElements * sizeof(MP_Sample_t) ) ) == NULL ) {
	  mp_error_msg( "MP_Anywave_Table_c::load_data", "Can't allocate room for storage - Leave the storage empty'.\n" );
	  return(false);
	} else {
	  /* try to read the file */	
	  
	  if ( mp_fread( storage, sizeof(MP_Sample_t), numElements, pFile ) != (size_t)numElements ) {
	    mp_error_msg( "MP_Anywave_Table_c::load_data", "number of elements read is %li instead of %i. Clear the storage\n" );
	    if (storage != NULL) {
	      free( storage );
	    }
	    return(false);
	  } else {
	    /* try to allocate room for wave */
	    if ( alloc_wave() == false ) {
	      mp_error_msg( "MP_Anywave_Table_c::load_data", "Can't allocate room for wave - Leave the wave array empty.\n" );
	      return(false);	      
	    } else {
	      /* fill in the pointers in wave */
	      for (filterIdx = 0, pSample = storage;
		   filterIdx < numFilters;
		   filterIdx ++) {
		for (chanIdx = 0;
		     chanIdx < numChans;
		     chanIdx ++, pSample += filterLen) {
		  wave[filterIdx][chanIdx] = pSample;
		}
	      }	      
	    }
	  }
	}
	fclose(pFile);
      }
    }   
  }
  return(true);
}

/*Allocate the pointers array wave, using the dimensions
      numFilters and numChans */
bool MP_Anywave_Table_c::alloc_wave( void ) {

  unsigned long int filterIdx;

  if ((double)MP_MAX_SIZE_T / (double)numFilters / (double)sizeof(MP_Sample_t **) <= 1.0) {
    mp_error_msg( "MP_Anywave_Table_c::alloc_wave", "numFilters [%lu] . sizeof(MP_Sample_t**) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for arrays. Exiting from alloc_wave().\n", numFilters, sizeof(MP_Sample_t**), MP_MAX_SIZE_T);
    return( false );
  }
  if ((double)MP_MAX_SIZE_T / (double)numChans / (double)sizeof(MP_Sample_t *) <= 1.0) {
    mp_error_msg( "MP_Anywave_Table_c::alloc_wave", "numChans [%lu] . sizeof(MP_Sample_t*) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for arrays. Exiting from alloc_wave().\n", numChans, sizeof(MP_Sample_t*), MP_MAX_SIZE_T);
    return( false );
  }

  if ( (wave = (MP_Sample_t ***) malloc( numFilters * sizeof(MP_Sample_t **) ) ) == NULL ) {
    mp_error_msg( "MP_Anywave_Table_c::alloc_wave", "Can't allocate room for wave - Leave the wave array empty" );
    return(false);
  } else {
    for ( filterIdx = 0;
	  filterIdx < numFilters;
	  filterIdx ++ ) {
      if ( ( wave[filterIdx] = (MP_Sample_t **) malloc( numChans * sizeof(MP_Sample_t *) ) ) == NULL ) {
	mp_error_msg( "MP_Anywave_Table_c::alloc_wave", "Can't allocate room for wave - Leave the wave array empty" );
	free_wave();
	return(false);
      }
    }
  }

  /* if succeed */
  return(true);

}

/* Free the pointer array wave */
void MP_Anywave_Table_c::free_wave( void ) {

  unsigned long int filterIdx;

  if ( wave != NULL ) {
    for ( filterIdx = 0;
	  filterIdx < numFilters;
	  filterIdx ++ ) {
      if ( wave[filterIdx] != NULL ) {
	free( wave[filterIdx] );
	wave[filterIdx] = NULL;
      }
    }
    free(wave);
    wave = NULL;
  }
}

/* Initialize all the members */
void MP_Anywave_Table_c::set_null( void ) {

  set_table_file_name( "" );
  set_data_file_name( "" );
  numChans = 0;
  filterLen = 0;
  numFilters = 0;
  normalized = 0;
  storage = NULL;
  wave = NULL;
}

/* Re-initialize all the members */
void MP_Anywave_Table_c::reset( void ) {

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Anywave_Table_c::reset() -- Entering.\n" );
#endif
  
  set_table_file_name( "" );
  set_data_file_name( "" );
  numChans = 0;
  filterLen = 0;
  numFilters = 0;
  normalized = 0;
  if (storage != NULL) {
    free(storage);
    storage = NULL;
  }
  free_wave();

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Anywave_Table_c::reset() -- Exiting.\n" );
#endif

}

/** \brief Parses a anywave table import file and fills in the members of
 * a MP_Anywave_Table_Scan_Info_c object
 *
 * anywave_table_scanner() is a function of the file
 * anywave_table_scanner.lpp. It is a FLEX program used to parse a
 * anywave table import file (e.g. "PATH/anywave.xml").
 * 
 * After calling anywave_table_scanner(), to fill in the members of the
 * MP_Anywave_Table_c object with the members of the
 * MP_Anywave_Table_Scan_Info_c interface, call the function
 * MP_Anywave_Table_Scan_Info_c::pop_table()
 *
 * \param fid : the stream of the file to parse (e.g. after opening
 * "PATH/anywave.xml")
 *
 * \param scanInfo : a pointer to MP_Anywave_Table_Scan_Info_c
 * interface, members of which will be filled in with the data parsed
 * from fid.
 *
 */
extern unsigned short int anywave_table_scanner( FILE *fid, MP_Anywave_Table_Scan_Info_c *scanInfo );  

/* Parse the stream pFile that describes the table */
bool MP_Anywave_Table_c::parse_file( FILE* pFile) {

  MP_Anywave_Table_Scan_Info_c scanInfo;
  unsigned short int event = ANYWAVE_TABLE_NULL_EVENT;

  while ( event != ANYWAVE_TABLE_REACHED_END_OF_FILE ) {
    /* Launch the scanner */
    event = anywave_table_scanner( pFile, &scanInfo );
    fflush( stderr ); /* Flush everything the scanner couldn't match */
    if (event == ANYWAVE_TABLE_CLOSE) {

      if ( scanInfo.pop_table( this ) == false ) {
	mp_error_msg( "MP_Anywave_Table_c::parse_file", "The table could not be parsed using MP_Anywave_Table_Scan_Info_c::pop_table(). Returning false (fails)\n" );
	return( false );
      }

    } else if (event == ANYWAVE_TABLE_ERROR_EVENT) {
      mp_error_msg( "MP_Anywave_Table_c::parse_file", "The parser crashed somewhere."
	      " Parsing interrupted, returning false (fails).\n");
      return( false );
    }
  }
  
#ifndef NDEBUG
  fprintf( stderr,"mplib DEBUG -- MP_Anywave_Table_c::parse_file( pFile ) - parsed the table.\n" );
  fflush( stderr );
#endif
  return ( true );

}

/* Normalization of the waveforms */
unsigned long int MP_Anywave_Table_c::normalize( void ) {
  unsigned long int filterIdx = 0;
  MP_Sample_t *pSample;
  MP_Sample_t *pSampleStart;
  MP_Sample_t *pSampleEnd;
  double energyCoeff;
 
  for (filterIdx = 0;
       filterIdx < numFilters;
       filterIdx++) {

    pSampleStart = wave[filterIdx][0];
    pSampleEnd = pSampleStart + filterLen*(unsigned long int)numChans;
  
    energyCoeff = 0.0;
    for (pSample = pSampleStart;
	 pSample < pSampleEnd;
	 pSample++) {
      energyCoeff += (*pSample) * (*pSample);    
    }
    energyCoeff = (MP_Sample_t) 1 / sqrt( (double) energyCoeff );
    for (pSample = pSampleStart;
	 pSample < pSampleEnd;
	 pSample++) {
      (*pSample) = (*pSample) * energyCoeff;
    }
  }
  normalized = 1;
  return(normalized);
}


/* set the tableFileName property to filename */
char* MP_Anywave_Table_c::set_table_file_name( char* filename ) {
  strcpy( tableFileName, filename );
  
  return(tableFileName);
}

/* set the dataFileName property to filename */
char* MP_Anywave_Table_c::set_data_file_name( char* filename ) {
  strcpy( dataFileName, filename );
  
  return(dataFileName);
}

/* printing to a stream */
unsigned long int MP_Anywave_Table_c::print( FILE *fid ) {

  unsigned long int nChar = 0;

  /* Print the xml declaration */
  fprintf( fid, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" );
  /* Print the lib version */
  nChar += fprintf( fid, "<libVersion>%s</libVersion>\n", VERSION );  
  /* Print opening <table> tag */
  nChar += fprintf( fid, "<table>\n" );
  /* Print the parameters */
  nChar += fprintf( fid, "\t<par type=\"numChans\">%i</par>\n", numChans );
  nChar += fprintf( fid, "\t<par type=\"filterLen\">%li</par>\n", filterLen );
  nChar += fprintf( fid, "\t<par type=\"numFilters\">%li</par>\n", numFilters );
  nChar += fprintf( fid, "\t<par type=\"data\">%s</par>\n", dataFileName );
  /* Print the closing </table> tag */
  nChar += fprintf( fid, "</table>\n");

  return( nChar );
}

/**********************/
/* Printing to a file */
unsigned long int MP_Anywave_Table_c::print( const char *fName ) {

  FILE *fid;
  unsigned long int nChar = 0;

  fid = fopen(fName,"w");
  if ( fid==NULL) {
    mp_error_msg( "MP_Anywave_Table_c::print", "Could not open file %s to write a table\n",fName);
    return(false);
  }
  nChar = print(fid);
  fclose (fid);
  return(nChar);

}