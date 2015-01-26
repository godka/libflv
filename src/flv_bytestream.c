#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "flv_bytestream.h"

uint64_t flv_dbl2int( double value )
{
    //return (union {double f; uint64_t i;}){value}.i;
}

/* Put functions  */

void flv_put_byte( flv_buffer *c, uint8_t b )
{
    flv_append_data( c, &b, 1 );
}

void flv_put_be32( flv_buffer *c, uint32_t val )
{
    flv_put_byte( c, val >> 24 );
    flv_put_byte( c, val >> 16 );
    flv_put_byte( c, val >> 8 );
    flv_put_byte( c, val );
}

void flv_put_be64( flv_buffer *c, uint64_t val )
{
    flv_put_be32( c, val >> 32 );
    flv_put_be32( c, val );
}

void flv_put_be16( flv_buffer *c, uint16_t val )
{
    flv_put_byte( c, val >> 8 );
    flv_put_byte( c, val );
}

void flv_put_be24( flv_buffer *c, uint32_t val )
{
    flv_put_be16( c, val >> 8 );
    flv_put_byte( c, val );
}

void flv_put_tag( flv_buffer *c, const char *tag )
{
    while( *tag )
        flv_put_byte( c, *tag++ );
}

void flv_put_amf_string( flv_buffer *c, const char *str )
{
    uint16_t len = strlen( str );
    flv_put_be16( c, len );
    flv_append_data( c, (uint8_t*)str, len );
}

void flv_put_amf_double( flv_buffer *c, double d )
{
    flv_put_byte( c, AMF_DATA_TYPE_NUMBER );
    flv_put_be64( c, flv_dbl2int( d ) );
}

/* flv writing functions */

flv_buffer *flv_create_writer( const char *filename )
{
    flv_buffer *c = (flv_buffer *)calloc( 1, sizeof(flv_buffer) );
    if( !c )
        return NULL;

    if( !strcmp( filename, "-" ) )
//        c->fp = stdout;
//    else
//        c->fp = fopen( filename, "wb" );
//    if( !c->fp )
    {
        free( c );
        return NULL;
    }

    return c;
}

int flv_append_data( flv_buffer *c, uint8_t *data, unsigned size )
{
    unsigned ns = c->d_cur + size;

    if( ns > c->d_max )
    {
        void *dp;
        unsigned dn = 16;
        while( ns > dn )
            dn <<= 1;

        dp = realloc( c->data, dn );
        if( !dp )
            return -1;

        c->data = (uint8_t*)dp;
        c->d_max = dn;
    }

    memcpy( c->data + c->d_cur, data, size );

    c->d_cur = ns;

    return 0;
}

void flv_rewrite_amf_be24( flv_buffer *c, unsigned length, unsigned start )
{
     *(c->data + start + 0) = length >> 16;
     *(c->data + start + 1) = length >> 8;
     *(c->data + start + 2) = length >> 0;
}

int flv_flush_data( flv_buffer *c )
{
    if( !c->d_cur )
        return 0;

//    if( fwrite( c->data, c->d_cur, 1, c->fp ) != 1 )
//        return -1;

    c->d_total += c->d_cur;

    c->d_cur = 0;

    return 0;
}
