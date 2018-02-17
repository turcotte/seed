/*                               -*- Mode: C -*- 
 * seq.h --- sequence related functions
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Wed Jun  8 14:00:30 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Thu Jul 21 16:23:51 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef SEQ_H
#define SEQ_H

extern int bio_read_fasta( char *name, char ***seqs_out, char ***descs_out, int ( *isvalid )( char c ) );

/*
 * Cornish-Bowden A. (1985)
 * Nucleic Acids Res. 10;13(9):3021-30. 
 * Nomenclature for incompletely specified bases in nucleic acid sequences:
 * recommendations 1984.
 * PMID: 2582368
 */

#define SYM_NUC_A  1 /* A       Adenine */
#define SYM_NUC_C  2 /* C       Cytosine */
#define SYM_NUC_M  3 /* A|C     aMino group at common position */
#define SYM_NUC_G  4 /* G       Guanine */
#define SYM_NUC_R  5 /* A|G     puRines */
#define SYM_NUC_S  6 /* C|G     Strong hydrogen bonding */
#define SYM_NUC_V  7 /* A|C|G   not T */
#define SYM_NUC_T  8 /* T       Thymine */
#define SYM_NUC_U  8 /* U       Uracil */
#define SYM_NUC_W  9 /* A|T     Weak hydrogen bonding */
#define SYM_NUC_Y 10 /* C|T     pYrimidines */
#define SYM_NUC_H 11 /* A|C|T   not G */
#define SYM_NUC_K 12 /* G|T     Keto group at common position */
#define SYM_NUC_D 13 /* A|G|T   not C */
#define SYM_NUC_B 14 /* C|G|T   not A */
#define SYM_NUC_N 15 /* A|C|G|T aNy */

#define SYM_GAP 0
#define SYM_TER 16

#define ALPHABET_SIZE 16
#define NUM_SYMBOLS 17

#define isgap( c ) ( c == SYM_GAP )
#define isany( c ) ( c == SYM_NUC_N )
#define ister( c ) ( c == SYM_TER )

extern int isnuc( char c );

extern int bio_is_char_class( symbol_t a );

extern alphabet_t bio_nuc_alphabet;

extern char bio_nuc_tochar( symbol_t s );

extern int bio_nuc_cmp( symbol_t c, symbol_t d );

extern int bio_nuc_isbp( symbol_t a, symbol_t b, int gu_allowed );

extern symbol_t bio_nuc_complement( symbol_t a );

extern dstring_t *bio_nuc_revcomp( dstring_t *forward );

#endif
