/******************************************************************************/
/*                                                                            */
/*                                  mask.h                                    */
/*                                                                            */
/*                        Matching Pursuit Library                            */
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

/********************************/
/*                              */
/* DEFINITION OF THE MASK CLASS */
/*                              */
/********************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */


#ifndef __mask_h_
#define __mask_h_


/***************************/
/** \brief A class implementing a masking mechanism to select/filter atoms from books.
 */
/***************************/
class MP_Mask_c {

  /********/
  /* DATA */
  /********/

public:
  
  /** \brief The dimension fo the filtered book */
  unsigned long int numAtoms;
  
  /** \brief An array of numAtoms booleans storing boolean values;
   *  MP_TRUE means keep the atom, MP_FALSE means throw it away.
   *   
   *  Note: we are not using the bool type because this array
   *  will be stored on disk, and we don't know what happens
   *  across platforms in that case. See mp_types.h to see which
   *  type is abstracted by MP_Bool_t.
   */
  MP_Bool_t* sieve;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
  
public:

  /** \brief A constructor initializing the boolean array.
   *
   * \param setNumAtoms The number of booleans in the sieve array.
   */
  MP_Mask_c( unsigned long int setNumAtoms );


  /** \brief Destructor. */
  virtual ~MP_Mask_c();


  /***************************/
  /* FACTORY METHOD          */
  /***************************/

 public:

  /** \brief Factory method. */
  static MP_Mask_c * init( unsigned long int setNumAtoms );


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /** \brief Set one element to MP_TRUE. */
  void set_true( unsigned long int i );

  /** \brief Set one element to MP_FALSE. */
  void set_false( unsigned long int i );

  /** \brief A method setting the whole array to MP_TRUE. */
  void reset_all_true( void );

  /** \brief A method setting the whole array to MP_FALSE. */
  void reset_all_false( void );

  /** \brief Check if numAtoms is the same in both masks. */
  int is_compatible_with( MP_Mask_c mask );

  /** \brief Check if numAtoms is the same in the mask and in a book. */
  int is_compatible_with( MP_Book_c book );

  /***************************/
  /* OPERATORS               */
  /***************************/

  /* \brief Assignment operator */
  MP_Mask_c& operator=(  const MP_Mask_c& from );

  /* \brief Operator AND. */
  MP_Mask_c operator&&( const MP_Mask_c& m2 );

  /* \brief Operator OR. */
  MP_Mask_c operator||( const MP_Mask_c& m2 );

  /* \brief Operator NOT. */
  MP_Mask_c operator!( void );

};


#endif /* __mask_h_ */