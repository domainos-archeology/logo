/*
 *    Convert .mac bitmaps into Apollo logo...
 *
 */

#include <stdio.h>
#include <stdlib.h>

int current_y = 0 ;
int current_x = 0 ;
int x_offset = 0 ;
int y_offset = 0 ;
int invert_on = 0 ;

int debug_on = 0 ;

void outbits( FILE *fp, int c );
int getopt( int argc, char **argv, char *optstring );
extern char *optarg;
extern int optind;

int main( int argc, char ** argv )
{
   FILE *infile = stdin ;
   FILE *outfile = stdout ;
   int this_opt ;
   int exit_status ;
   int i, j ;
   int c ;

   exit_status = 0 ;

   while( (this_opt = getopt( argc, argv, "i:o:x:y:vdu" )) != EOF ) {

      switch( this_opt ) {

         case 'i' :
            infile = fopen( optarg, "rb" );
            if( infile == NULL ) {
               fprintf( stderr, "%s: Unable to open file: %s\n", argv[0], optarg );
               exit_status = 3 ;
            }
            break ;

         case 'o' :
            outfile = fopen( optarg, "wb" );
            if( outfile == NULL ) {
               fprintf( stderr, "%s: Unable to open file: %s\n", argv[0], optarg );
               exit_status = 3 ;
            }
            break ;

         case 'x' :
            x_offset = atoi( optarg );
            break ;

         case 'y' :
            y_offset = atoi( optarg );
            break ;

         case 'd' :
            ++debug_on ;
            break ;

         case 'v' :
            ++invert_on ;
            break ;

         case 'u' :
            exit_status = 3 ;     /* Cause usage to print and exit */
            break ;

      } /* switch */

   } /* while arguments */

   if( exit_status != 0 ) {

      fputs( "Usage: mac2logo [-i infile] [-o outfile] [-x offset] [-y offset] [-v] [-d] [-u]\n"
             "\n"
             "where:   -i  input .mac file (default standard input)\n"
             "         -o  output apollo logo format (default standard output)\n"
             "         -x  shift logo window over .mac input (- left, + right)\n"
             "         -y  shift logo window over .mac input (- up, + down)\n"
             "         -v  invert image\n"
             "         -d  debug info\n"
             "         -u  usage message\n"
           , stderr );

      exit( exit_status );

   }

   if( debug_on ) {
      printf( "DEBUG: Seeking past header...\n" );
   }
   fseek( infile, 640, 0L );

   for( i = 0 ; i < 720 && !feof( infile ) ; ++i ) {

      if( debug_on ) {
         printf( "DEBUG: Reading line %d of input image...\n", i );
      }

      for( j = 0 ; j < 576 ;  ) {

         c = getc( infile );

         if( c & 0x80 ) {

            int cnt ;

            /* Repeat */
            cnt = 257 - c ;
            c = getc( infile );

            if( debug_on > 2 ) {
               printf( "DEBUG: Repeating %02X, %d times...\n", c, cnt );
            }

            while( cnt-- ) {
               outbits( outfile, c );
               j += 8 ;
            }

         } else {

            int cnt ;

            /* Count */
            cnt = c +2 ;

            if( debug_on > 2 ) {
               printf( "DEBUG: Reading next %d bytes...\n       ", cnt );
            }

            while( --cnt ) {
               int   tmp ;

               tmp = getc( infile );

               if( debug_on > 2 ) {
                  printf( "%02X ", tmp );
               }

               outbits( outfile, tmp );
               j += 8 ;

            }

            if( debug_on > 2 ) {
               putchar( '\n' );
            }

         }

      } /* for j */

      /* Start a new line */
      outbits( outfile, -1 );

   } /* for i */

   return 0 ;

}

void outbits( FILE *fp, int c )
{

   int i ;

   if( c == -1 ) {

      /* Fill out line if necessary */
      if( current_y >= y_offset && current_y < 160 + y_offset ) {
         while(  current_x <= 512 + x_offset ) {
            if( debug_on > 1 ) {
               fputs( "        ", stdout );
            }
            putc( 0, fp );
            current_x += 8 ;
         }
      }

      current_x = 3 ;   /* Offset for rounding byte-wide copies */
      ++current_y ;

      if( debug_on > 1 ) {
         putchar( '\n' );
      }

   } else {

      if( current_y >= y_offset && current_y < 160 + y_offset
          && current_x >= x_offset && current_x < 512 + x_offset
        ) {
         if( invert_on ) {
            c = ~c ;
         }

         if( debug_on > 1 ) {
            int tmp_c = c ;
            for( i = 0 ; i < 8 ; ++i ) {
               if( tmp_c & 0x80 ) {
                  putchar( '#' );
               } else {
                  putchar( ' ' );
               }
            }
         }

         putc( c, fp );

      }

      current_x += 8 ;    /* 8 more bits out */

   }

}
