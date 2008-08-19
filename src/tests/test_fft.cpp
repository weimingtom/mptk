/******************************************************************************/
/*                                                                            */
/*                                test_fft.cpp                                */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
/*                                                                            */
/* R�mi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2005 */
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
 * SVN log:
 *
 * $Author: slesage $
 * $Date: 2006-03-15 12:09:32 +0100 (Wed, 15 Mar 2006) $
 * $Revision: 533 $
 *
 */

/** \file test_fft.cpp
 * A file with some code that serves both as an example of how to use the
 *  FFT interface and as a test that it is properly working.
 *
 */
#include <mptk.h>

#include <stdio.h>
#include <stdlib.h>

#define WIN_SIZE 256
#define OUT_SIZE 257
#define IN_SIZE 512
#define MAX_SIZE 8192
#define MP_FFT_TEST_PRECISION 1e-14

int main(int argc, char ** argv)
{
  double presicion = 0.0;
  char *p;
  char* func = "test_fft";

  if (argc == 1) presicion = MP_FFT_TEST_PRECISION;
  else
    {
      if (argc == 2)
        {
          presicion = strtod(argv[1], &p);
          if ( (p == argv[1]) || (*p != 0) )
            {
              mp_error_msg( func, "Failed to convert argument [%s] to a double value.\n",
                            argv[1] );
              return( -1 );
            }
        }
      else mp_error_msg( func, "Bad Number of arguments, test_fft require precision as argument in double.\n"
                         );
    }


  MP_Real_t in[MAX_SIZE];
  MP_Real_t   mag_out[OUT_SIZE];
  FILE *fid;
  unsigned long int i;
  const char *configFileName = NULL;
  std::string strAppDirectory;
  bool testpassed= true;
  mp_info_msg( func, "--------------------------------------\n" );
  mp_info_msg( func, "TEST FFT - TESTING FFT FUNCTIONALITIES\n" );
  mp_info_msg( func, "--------------------------------------\n" );
  mp_info_msg( func, "Test FFT with [%g] double precision.\n",presicion);

  /* Creates the signal */
  for (i=0; i < MAX_SIZE; i++)
    {
      in[i] = ( MP_Real_t) rand();
    }

  if ( MP_FFT_Interface_c::test(presicion,2,DSP_RECTANGLE_WIN, 0.0, in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,4,DSP_RECTANGLE_WIN, 0.0, in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,8,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,16,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,32,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,64,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,128,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,256,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,512,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,1024,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,2048,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,4096,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,8192,DSP_RECTANGLE_WIN, 0.0,in) ==1 )testpassed= false;

  if ( MP_FFT_Interface_c::test(presicion,2,DSP_HAMMING_WIN, 0.0, in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,4,DSP_HAMMING_WIN, 0.0, in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,8,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,16,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,32,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,64,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,128,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,256,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,512,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,1024,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,2048,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,4096,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,8192,DSP_HAMMING_WIN, 0.0,in) ==1 )testpassed= false;

  if ( MP_FFT_Interface_c::test(presicion,2,DSP_EXPONENTIAL_WIN, 0.0, in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,4,DSP_EXPONENTIAL_WIN, 0.0, in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,8,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,16,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,32,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,64,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,128,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,256,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,512,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,1024,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,2048,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,4096,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;
  if ( MP_FFT_Interface_c::test(presicion,8192,DSP_EXPONENTIAL_WIN, 0.0,in) ==1 )testpassed= false;


  /* Load the MPTK environment */
  if(! (MPTK_Env_c::get_env()->load_environment(configFileName)) ) {
	if (! (MPTK_Env_c::get_env()->get_environment_loaded()) ) {
		mp_error_msg(func,"Could not load the MPTK environment.\n");
		mp_info_msg(func,"The most common reason is a missing or erroneous MPTK_CONFIG_FILENAME variable.\n");
		mp_info_msg("","The MPTK environment can be specified either by:\n");
		mp_info_msg("","  a) setting the MPTK_CONFIG_FILENAME environment variable\n");
		mp_info_msg("","     using e.g. 'setenv MPTK_CONFIG_FILENAME <path_to_config_file.xml>'\n");
		mp_info_msg("","     in a shell terminal, or\n");
		mp_info_msg("","     'setenv('MPTK_CONFIG_FILENAME','<path to configuration file.xml>')\n");
		mp_info_msg("","      from the Matlab command line\n");
		mp_info_msg("","  b) using the -C <path_to_configfile.xml> option in many MPTK command line utilities.\n");
		exit(-1);
		}
    }

  /*
   * 1/ FFT computation with internally computed window 
   */
  {
    MP_FFT_Interface_c * myFFT = MP_FFT_Interface_c::init( WIN_SIZE, DSP_HAMMING_WIN, 0.0, OUT_SIZE );

    /* Execute the FFT */
    myFFT->exec_mag( in, mag_out );

    /* Output to files */
    strAppDirectory = MPTK_Env_c::get_env()->get_config_path("reference");
    strAppDirectory += "/signal/window_out.dbl";
    if ( ( fid = fopen(strAppDirectory.c_str(),"w") ) == NULL )
      {
        mp_error_msg( func, "Can't open file [%s] in write mode.\n",
                      "signal/window_out.dbl" );
        exit(-1);
      }
    mp_fwrite( myFFT->window, sizeof(Dsp_Win_t), WIN_SIZE, fid );
    fclose(fid);
    strAppDirectory = MPTK_Env_c::get_env()->get_config_path("reference");
    strAppDirectory += "/signal/magnitude_out.dbl";
    if ( ( fid = fopen(strAppDirectory.c_str(),"w") ) == NULL )
      {
        mp_error_msg( func, "Can't open file [%s] in write mode.\n",
                      "signal/magnitude_out.dbl" );
        exit(-1);
      }
    mp_fwrite( mag_out, sizeof(MP_Real_t), OUT_SIZE, fid );
    fclose(fid);
    delete myFFT;
  }

  /*
   * 2/ FFT computation with externally tabulated window
   */

  /* OBSOLETE: all the windows are now tabulated in the window server. */

  /*
   * 3/ FFT computation for the "2 cosines" signal
   */
  mp_info_msg( func,"FFT computation for the 2_cosines signal \n");
  {
    MP_Real_t buffer[256];
    MP_Real_t   magbuf[512];
    MP_FFT_Interface_c * fft = MP_FFT_Interface_c::init( 256, DSP_HAMMING_WIN, 0.0, 512 );
    if (fft != NULL)
      {
        strAppDirectory = MPTK_Env_c::get_env()->get_config_path("reference");
        strAppDirectory += "/signal/2_cosines.flt";
        if ( ( fid = fopen(strAppDirectory.c_str(),"r") ) == NULL )
          {
            mp_error_msg( func, "Can't open file [%s] in read mode.\n",
                          "signal/2_cosines.flt" );
            exit(-1);
          }
        mp_fread( buffer, sizeof(float), 256, fid );
        fclose(fid);
        fft->exec_mag( buffer, magbuf);
        strAppDirectory = MPTK_Env_c::get_env()->get_config_path("reference");
        strAppDirectory += "/signal/out_two_peaks.dbl";
        if ( ( fid = fopen(strAppDirectory.c_str(),"w") ) == NULL )
          {
            mp_error_msg( func, "Can't open file [%s] in write mode.\n",
                          "signal/out_two_peaks.dbl" );
            exit(-1);
          }
        mp_fwrite( magbuf, sizeof(MP_Real_t), 512, fid );
        fclose(fid);
        mp_info_msg( func,"A FFT of the first [%d] samples of the signal in file [%s]\n"
                     "was computed with a [%d] points Hamming window and stored in file [%s]\n",
                     256,"signals/2_cosines.flt",256,"signals/out_two_peaks.dbl");
        delete fft;
      }

  }

  /*
   * 4/ FFT computation for the whole of the "2 cosines" signal
   */
  {
    MP_Real_t buffer[8192];
    MP_Real_t   magbuf[8192];
    mp_info_msg( func,"Testing something that makes the fft crash ... \n");
    MP_FFT_Interface_c * fft = MP_FFT_Interface_c::init( 2000, DSP_HAMMING_WIN, 0.0, 512 );
    if (fft!= NULL)
      {
        mp_error_msg( func,"FFT Interface can't create a FFT of size %lu smaller than the window size %lu. But create it\n",
                      2000,512);
        return(-1);
      }
    mp_info_msg( func,"Testing FFT on huge windows \n");
    strAppDirectory = MPTK_Env_c::get_env()->get_config_path("reference");
    strAppDirectory += "/signal/2_cosines.flt";
    if ( ( fid = fopen(strAppDirectory.c_str(),"r") ) == NULL )
      {
        mp_error_msg( func, "Can't open file [%s] in read mode.\n",
                      "signal/2_cosines.flt" );
        exit(-1);
      }
    mp_fread( buffer, sizeof(float), 8192, fid );
    fclose(fid);

    fft = MP_FFT_Interface_c::init( 8192, DSP_HAMMING_WIN, 0.0, 8192 );
    if (fft != NULL)
      {
        fft->exec_mag( buffer, magbuf);
        strAppDirectory = MPTK_Env_c::get_env()->get_config_path("reference");
        strAppDirectory += "/signal/out_two_peaks_whole.dbl";
        if ( ( fid = fopen(strAppDirectory.c_str(),"w") ) == NULL )
          {
            mp_error_msg( func, "Can't open file [%s] in write mode.\n",
                          "signals/out_two_peaks_whole.dbl" );
            exit(-1);
          }
        mp_fwrite( magbuf, sizeof(MP_Real_t), 512, fid );
        fclose(fid);
        mp_info_msg( func,"The first [%d] points of a FFT of the whole signal in file [%s]\n"
                     "was computed with a [%d] points Hamming window and stored in file [%s]\n",
                     512,"signals/2_cosines.flt",8192,"signals/out_two_peaks_whole.dbl");
        
      }
    else
      {
        mp_error_msg( func,"Cannot compute [%d] points of a FFT of the whole signal in file [%s]\n"
                      "because cannot create a [%d] points Hamming window\n",
                      512,"signals/2_cosines.flt",8192);
        delete fft;
        exit(-1);
      }
        delete fft;
  }
  
  if (testpassed) return( 0 );
  else return ( -1);
}
