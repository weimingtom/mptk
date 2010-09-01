/******************************************************************************/
/*                                                                            */
/*                                 gpd.cpp                                    */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*              															  */
/* Thomas Blumensath                                                          */
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
 * $Author: broy $
 * $Date: 2007-09-14 16:58:11 +0200 (ven., 14 sept. 2007) $
 * $Revision: 1145 $
 *
 */

#include <mptk.h>
#include "getopt.h"


static char *cvsid = "$Revision: 1145 $";

char* func = "gpd";



/********************/
/* Error types      */
/********************/
#define ERR_ARG        1
#define ERR_BOOK       2
#define ERR_DICT       3
#define ERR_SIG        4
#define ERR_CORE       5
#define ERR_DECAY      6
#define ERR_OPEN       7
#define ERR_WRITE      8

/********************/
/* Global variables */
/********************/

unsigned long int GPD_REPORT_HIT = ULONG_MAX; /* Default: never report during the main loop. */
unsigned long int GPD_SAVE_HIT   = ULONG_MAX; /* Default: never save during the main loop. */
unsigned long int GPD_SNR_HIT    = ULONG_MAX; /* Default: never test the snr during the main loop. */

int GPD_QUIET      = MP_FALSE;
int GPD_VERBOSE    = MP_FALSE;


#define GPD_DEFAULT_NUM_ITER   ULONG_MAX
unsigned long int GPD_NUM_ITER = GPD_DEFAULT_NUM_ITER;
int GPD_USE_ITER = MP_FALSE;

#define GPD_DEFAULT_SNR        0.0
double GPD_SNR  = GPD_DEFAULT_SNR;
int GPD_USE_SNR = MP_FALSE;

double GPD_PREEMP = 0.0;

/* Input/output file names: */
const char *dictFileName  = NULL;
const char *sndFileName   = NULL;
const char *bookFileName = NULL;
const char *resFileName   = NULL;
const char *decayFileName = NULL;
const char *configFileName = NULL;


/**************************************************/
/* HELP FUNCTION                                  */
/**************************************************/
void usage( void )
{

  fprintf( stdout, " \n" );
  fprintf( stdout, " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
  fprintf( stdout, " WARNING, CONSTRUCTION WORK IN PROGRESS, GPD UNDER DEVELOPMENT\n" );
  fprintf( stdout, " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
  fprintf( stdout, " Usage:\n" );
  fprintf( stdout, "     gpd [options] -D dictFILE.xml (-n N|-s SNR) (sndFILE.wav|-) (bookFILE.bin|-) [residualFILE.wav]\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Synopsis:\n" );
  fprintf( stdout, "     Iterates (Trunctated) Gradient Pursuit on signal sndFILE.wav with dictionary dictFILE.xml\n" );
  fprintf( stdout, "     and gives the resulting book bookFILE.bin (and an optional residual signal)\n" );
  fprintf( stdout, "     after N iterations or after reaching the signal-to-residual ratio SNR.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Mandatory arguments:\n" );
  fprintf( stdout, "     -D<FILE>, --dictionary=<FILE>  Read the dictionary from xml file FILE.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -n<N>, --num-iter=<N>|--num-atoms=<N>    Stop after N iterations.\n" );
  fprintf( stdout, "AND/OR -s<SNR>, --snr=<SNR>                   Stop when the SNR value SNR is reached.\n" );
  fprintf( stdout, "                                              If both options are used together, the algorithm stops\n" );
  fprintf( stdout, "                                              as soon as either one is reached.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     (sndFILE.wav|-)                          The signal to analyze or stdin (in WAV format).\n" );
  fprintf( stdout, "     (bookFILE.bin|-)                         The file to store the resulting book of atoms, or stdout.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Optional arguments:\n" );
  fprintf( stdout, "     -C<FILE>, --config-file=<FILE>  Use the specified configuration file, \n" );
  fprintf( stdout, "                                     otherwise the MPTK_CONFIG_FILENAME environment variable will be\n" );
  fprintf( stdout, "                                     used to find the configuration file and set up the MPTK environment.\n" );
  fprintf( stdout, "     -E<FILE>, --energy-decay=<FILE>  Save the energy decay as doubles to file FILE.\n" );
  fprintf( stdout, "     -R<N>,    --report-hit=<N>       Report some progress info (in stderr) every N iterations.\n" );
  fprintf( stdout, "     -S<N>,    --save-hit=<N>         Save the output files every N iterations.\n" );
  fprintf( stdout, "     -T<N>,    --snr-hit=<N>          Test the SNR every N iterations only (instead of each iteration).\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -p<double>, --preemp=<double>    Pre-emphasize the input signal with coefficient <double>.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     residualFILE.wav                The residual signal after subtraction of the atoms.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -q, --quiet                    No text output.\n" );
  fprintf( stdout, "     -v, --verbose                  Verbose.\n" );
  fprintf( stdout, "     -V, --version                  Output the version and exit.\n" );
  fprintf( stdout, "     -h, --help                     This help.\n" );
  fprintf( stdout, " \n" );

  exit(0);
}


/**************************************************/
/* PARSING OF THE ARGUMENTS                       */
/**************************************************/
int parse_args(int argc, char **argv)
{

  int c, i;
  char *p;

  struct option longopts[] =
      {
      	{"config-file",  required_argument, NULL, 'C'},
        {"dictionary",   required_argument, NULL, 'D'},
        {"energy-decay", required_argument, NULL, 'E'},
        {"report-hit",   required_argument, NULL, 'R'},
        {"save-hit",     required_argument, NULL, 'S'},
        {"snr-hit",      required_argument, NULL, 'T'},

        {"num-atoms",    required_argument, NULL, 'n'},
        {"num-iter",     required_argument, NULL, 'n'},
        {"preemp",       required_argument, NULL, 'p'},
        {"snr",          required_argument, NULL, 's'},

        {"quiet",   no_argument, NULL, 'q'},
        {"verbose", no_argument, NULL, 'v'},
        {"version", no_argument, NULL, 'V'},
        {"help",    no_argument, NULL, 'h'},
        {0, 0, 0, 0}
      };

  opterr = 0;
  optopt = '!';

  while ((c = getopt_long(argc, argv, "C:D:E:R:S:T:n:p:s:qvVh", longopts, &i)) != -1 )
    {

      switch (c)
        { case 'C':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -C : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -C or switch --config-file=.\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --config-file without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else configFileName = optarg;
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read config-file name [%s].\n", configFileName );
          
          break;


        case 'D':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -D : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -D or switch --dictionary=.\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --dictionary without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else dictFileName = optarg;
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read dictionary file name [%s].\n", dictFileName );
          break;


        case 'E':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -E : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -E or switch --energy-decay= :\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --energy-decay without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else decayFileName = optarg;
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read decay file name [%s].\n", decayFileName );
          break;


        case 'R':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -R : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -R or switch --report-hit= :\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --report-hit without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else GPD_REPORT_HIT = strtoul(optarg, &p, 10);
          if ( (p == optarg) || (*p != 0) )
            {
              mp_error_msg( func, "After switch -R or switch --report-hit= :\n" );
              mp_error_msg( func, "failed to convert argument [%s] to an unsigned long value.\n",
                            optarg );
              return( ERR_ARG );
            }
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read report hit [%lu].\n", GPD_REPORT_HIT );
          break;


        case 'S':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -S : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -S or switch --save-hit= :\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --save-hit without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else GPD_SAVE_HIT = strtoul(optarg, &p, 10);
          if ( (p == optarg) || (*p != 0) )
            {
              mp_error_msg( func, "After switch -S or switch --save-hit= :\n" );
              mp_error_msg( func, "failed to convert argument [%s] to an unsigned long value.\n",
                            optarg );
              return( ERR_ARG );
            }
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read save hit [%lu].\n", GPD_SAVE_HIT );
          break;


        case 'T':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -T : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -T or switch --snr-hit= :\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --snr-hit without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else GPD_SNR_HIT = strtoul(optarg, &p, 10);
          if ( (p == optarg) || (*p != 0) )
            {
              mp_error_msg( func, "After switch -T or switch --snr-hit= :\n" );
              mp_error_msg( func, "failed to convert argument [%s] to an unsigned long value.\n",
                            optarg );
              return( ERR_ARG );
            }
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read snr hit [%lu].\n", GPD_SNR_HIT );
          break;



        case 'n':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -n : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -n/--num-iter=/--num-atom= :\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --numiter or --numatom without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else GPD_NUM_ITER = strtoul(optarg, &p, 10);
          if ( (p == optarg) || (*p != 0) )
            {
              mp_error_msg( func, "After switch -n/--num-iter=/--num-atom= :\n" );
              mp_error_msg( func, "failed to convert argument [%s] to an unsigned long value.\n",
                            optarg );
              return( ERR_ARG );
            }
          GPD_USE_ITER = MP_TRUE;
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read numIter [%lu].\n", GPD_NUM_ITER );
          break;


        case 'p':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -p : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -p/--preemp= :\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --preemp without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else GPD_PREEMP = strtod(optarg, &p);
          if ( (p == optarg) || (*p != 0) )
            {
              mp_error_msg( func, "After switch -p/--preemp= :\n" );
              mp_error_msg( func, "failed to convert argument [%s] to a double value.\n",
                            optarg );
              return( ERR_ARG );
            }
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read preemp coeff [%g].\n", GPD_PREEMP );
          break;


        case 's':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -s : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -s/--snr= :\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --snr without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else GPD_SNR = strtod(optarg, &p);
          if ( (p == optarg) || (*p != 0) )
            {
              mp_error_msg( func, "After switch -s/--snr= :\n" );
              mp_error_msg( func, "failed to convert argument [%s] to a double value.\n",
                            optarg );
              return( ERR_ARG );
            }
          GPD_USE_SNR = MP_TRUE;
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read SNR [%g].\n", GPD_SNR );
          break;



        case 'h':
          usage();

        case 'q':
          GPD_QUIET = MP_TRUE;
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "GPD_QUIET is TRUE.\n" );
          break;


        case 'v':
          GPD_VERBOSE = MP_TRUE;
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "GPD_VERBOSE is TRUE.\n" );
          break;


        case 'V':
          fprintf(stdout, "gpd -- Matching Pursuit library version %s -- gpd %s\n", VERSION, cvsid);
          exit(0);
          break;


        default:
          mp_error_msg( func, "The command line contains the unrecognized option [%s].\n",

                        argv[optind-1] );
          return( ERR_ARG );

        } /* end switch */

    } /* end while */


  mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "When exiting getopt, optind is [%d].\n", optind );
  mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "(argc is [%d].)\n", argc );

  /* Check if some file names are following the options */
  if ( (argc-optind) < 1 )
    {
      mp_error_msg( func, "You must indicate a file name (or - for stdin) for the signal to analyze.\n");
      return( ERR_ARG );
    }
  if ( (argc-optind) < 2 )
    {
      mp_error_msg( func, "You must indicate a file name (or - for stdout) for the book file.\n");
      return( ERR_ARG );
    }

  /* Read the file names after the options */
  sndFileName = argv[optind++];
  mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read sound file name [%s].\n", sndFileName );
  bookFileName = argv[optind++];
  mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read book file name [%s].\n", bookFileName );
  if (optind < argc)
    {
      resFileName = argv[optind++];
      mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read residual file name [%s].\n", resFileName );
    }


  /***********************/
  /* Basic options check */

  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( GPD_QUIET && GPD_VERBOSE )
    {
      mp_error_msg( func, "Choose either one of --quiet or --verbose.\n");
      return( ERR_ARG );
    }

  /* Was dictionary file name given ? */
  if ( dictFileName == NULL )
    {
      mp_error_msg( func, "You must specify a dictionary using switch -D/--dictionary= .\n");
      return( ERR_ARG );
    }

  /* Must have one of --num-iter or --snr to tell the algorithm where to stop */
  if ( (!GPD_USE_SNR) && (!GPD_USE_ITER) )
    {
      mp_error_msg( func, "You must specify one of : --num-iter=n/--num-atoms=n\n" );
      mp_error_msg( func, "                     or   --snr=%%f\n" );
      return( ERR_ARG );
    }

  /* If snr is given without a snr hit value, test the snr on every iteration */
  if ((GPD_SNR_HIT == ULONG_MAX) && GPD_USE_SNR ) GPD_SNR_HIT = 1;

  /* If having both --num-iter AND --snr, warn */
  if ( (!GPD_QUIET) && GPD_USE_SNR && GPD_USE_ITER )
    {
      mp_warning_msg( func, "The option --num-iter=/--num-atoms= was specified together with the option --snr=.\n" );
      mp_warning_msg( func, "The algorithm will stop when the first of either conditions is reached.\n" );
      mp_warning_msg( func, "(Use --help to get help if this is not what you want.)\n" );
    }

  return( 0 );
}


/**************************************************/
/* GLOBAL FUNCTIONS                               */
/**************************************************/
void free_mem(MP_Dict_c* dict, MP_Book_c* book, MP_Signal_c* sig, GPD_Core_c* calculator )
{

  if ( sig  )  delete sig;
  if ( calculator ) delete calculator;
  if ( dict )  delete dict;
  if ( book )  delete book;
}


/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv )
{

  MP_Dict_c *dict = NULL;
  MP_Signal_c *sig = NULL;
  GP_Double_Index_Book_c *book = NULL;
  GPD_Core_c *calculator = NULL;
  unsigned long int i = 0;

  /**************************************************/
  /* PRELIMINARIES                                  */
  /**************************************************/

  
  /* Parse the command line */
  if ( argc == 1 ) usage();
  if ( parse_args( argc, argv ) )
    {
      mp_error_msg( func, "Please check the syntax of your command line."
                    " (Use --help to get some help.)\n" );
      exit( ERR_ARG );
    }
 
  /* Load environnement */
  MPTK_Env_c::get_env()->load_environment(configFileName);
  
  /* Re-print the command line */
  if ( !GPD_QUIET )
    {
      mp_info_msg( func, "------------------------------------\n" );
      mp_info_msg( func, "GPD - GRADIENT PURSUIT DECOMPOSITION\n" );
      mp_info_msg( func, "------------------------------------\n" );
      mp_info_msg( func, " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
      mp_info_msg( func, " WARNING, CONSTRUCTION WORK IN PROGRESS, GPD UNDER DEVELOPMENT\n" );
      mp_info_msg( func, " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
      mp_info_msg( func, "The command line was:\n" );
      for ( i=0; i<(unsigned long int)argc; i++ )
        {
          fprintf( stderr, "%s ", argv[i] );
        }
      fprintf( stderr, "\n");
      fflush( stderr );
      mp_info_msg( func, "End command line.\n" );
    }
    
 
  
  /********************************/
  /* Make the manipulated objects */
  //gettimeofday(&tim, NULL);
  //double t1=tim.tv_sec+(tim.tv_usec/1000000.0);


  
  /****/
  /* Load the dictionary */
  if ( !GPD_QUIET ) mp_info_msg( func, "Loading the dictionary...\n" );
  /* Add the blocks to the dictionnary */
  if ( !GPD_QUIET ) mp_info_msg( func, "(In the following, spurious output of dictionary pieces"
  
                                  " would be a symptom of parsing errors.)\n" );

  dict = MP_Dict_c::init( dictFileName );
  if ( dict == NULL )
    {
      mp_error_msg( func, "Failed to create a dictionary from XML file [%s].\n",
                    dictFileName );
      free_mem( dict, book, sig, calculator );

      return( ERR_DICT );
    }
  if ( dict->numBlocks == 0 )
   {
      mp_error_msg( func, "The dictionary scanned from XML file [%s] contains no blocks.\n");
      free_mem( dict, book, sig, calculator );

      return( ERR_DICT );
    }
    
  if ( !GPD_QUIET ) mp_info_msg( func, "The dictionary is now loaded.\n" );
  
  /****/
  /* Load the signal */
  if ( !GPD_QUIET ) mp_info_msg( func, "Loading the signal...\n" );
  sig = MP_Signal_c::init( sndFileName );
  if ( sig == NULL )
    {
      mp_error_msg( func, "Failed to initialize a signal from file [%s].\n",
                    sndFileName );
      free_mem( dict, book, sig, calculator );
      return( ERR_SIG );
    }

  /* Pre-emphasize the signal if needed */
  if (GPD_PREEMP != 0.0)
    {
      if ( GPD_VERBOSE ) mp_info_msg( func, "Pre-emphasizing the signal...\n" );
      sig->preemp( GPD_PREEMP );
      if ( GPD_VERBOSE ) mp_info_msg( func, "Pre-emphasis done.\n" );
    }
  if ( !GPD_QUIET ) mp_info_msg( func, "The signal is now loaded.\n" );

  /****/
  /* Make the book */
  if ( !GPD_QUIET ) mp_info_msg( func, "Try to create a new book.\n" );
  book = GP_Double_Index_Book_c::create(sig->numChans, sig->numSamples, sig->sampleRate, dict->numBlocks );
  if ( book == NULL )
    {
      mp_error_msg( func, "Failed to create a new book.\n" );
      free_mem( dict, book, sig, calculator );
      return( ERR_BOOK );
    }
  
  /****/
  /* Make the calculator */
  //calculator = GP_Gpd_Core_c::create( sig, book, dict );
  //string AlgoType = "MP";
  calculator = GPD_Core_c::create( sig, book, dict);
  if ( calculator == NULL )
    {
      mp_error_msg( func, "Failed to create a GPD core object.\n" );
      free_mem( dict, book, sig, calculator );
      return( ERR_CORE );
    }
  if ( GPD_USE_ITER ) calculator->set_iter_condition( GPD_NUM_ITER );
  if ( GPD_USE_SNR  ) calculator->set_snr_condition( GPD_SNR );
  
  /****/
  /* Report */
  if ( GPD_VERBOSE )
    {
      mp_info_msg( func, "The dictionary read from file [%s] contains [%u] blocks:\n",
                   dictFileName, dict->numBlocks );
      for ( i = 0; i < dict->numBlocks; i++ ) dict->block[i]->info( stderr );
      mp_info_msg( func, "End of dictionary.\n" );
      mp_info_msg( func, "The signal loaded from file [%s] has:\n", sndFileName );
      sig->info();
    }

  /****/
  /* Set the breakpoints and other parameters */
  if ( !GPD_QUIET ) calculator->set_report_hit( GPD_REPORT_HIT );
  calculator->set_save_hit( GPD_SAVE_HIT, bookFileName, resFileName, decayFileName );
  if ( GPD_USE_SNR ) calculator->set_snr_hit( GPD_SNR_HIT );
  if ( GPD_VERBOSE ) calculator->set_verbose();
  else calculator->reset_verbose();
   



  /******************/
  /* Initial report */
  if ( !GPD_QUIET )
    {
      mp_info_msg( func, "-------------------------\n" );
      mp_info_msg( func, "Starting Gradient Pursuit on signal [%s] with dictionary [%s].\n",
                   sndFileName, dictFileName );
      mp_info_msg( func, "-------------------------\n" );
      calculator->info_conditions();
    }

  /**************************************************/
  /* MAIN PURSUIT LOOP                              */
  /**************************************************/

  /* Report */
  if ( !GPD_QUIET )
    {
      mp_info_msg( func, "The initial signal energy is : %g\n", calculator->get_initial_energy() );
      mp_info_msg( func, "STARTING TO ITERATE\n" );
    }
  calculator->run();

  /* Report */
  if ( !GPD_QUIET ) calculator->info_state();


  /*********************/


  /**************************************************/
  /* FINAL SAVES AND CLEANUP                        */
  /**************************************************/

  /**************/
  /* End report */
  if ( !GPD_QUIET )
    {
      mp_info_msg( func, "-------------------------\n" );
      mp_info_msg( func, "Gradient PURSUIT RESULTS:\n" );
      mp_info_msg( func, "-------------------------\n" );
      calculator->info_result();
    }
 
  /***************************/
  /* Global save at the end: */
  calculator->save_result();

  /* If the book has to be sent to stdin: */
  if ( strcmp( bookFileName, "-" ) == 0 )
  
    {
      book->print( stdout, MP_TEXT );
      fflush( stdout );
      if ( GPD_VERBOSE ) mp_info_msg( func, "Sent the book to stdout in text mode.\n" );
    }

  /*******************/
  /* Clean the house */
   
  if ( !GPD_QUIET ) mp_info_msg( func, "Have a nice day !\n" );
  /* Release MPTK environnement */
  free_mem( dict, book, sig, calculator ); 
  MPTK_Env_c::get_env()->release_environment();
  
  return( 0 );
}
