#ifndef _CMF_H
#define _CMF_H

#include <stdbool.h>

enum cmf_message_format {
    CMFMT_POSITIVE_NUMBER = 0,
    CMFMT_NEGATIVE_NUMBER = 1,
    CMFMT_STRING_UTF8 = 2,
    CMFMT_BYTES = 3,
    CMFMT_BOOL_TRUE = 4,
    CMFMT_BOOL_FALSE = 5
    // TODO double
};

enum cmf_parser_result {
    CMF_FOUND_TOKEN,
    CMF_PARSER_ERROR,
    CMF_DOCUMENT_END
};

/* builder API */

/*
 * The cmfbuilder_add_* range of methods append data to an existing buffer.
 * The amount of data added is relative to the actual value passed because a variable-width byte encoding is being used.
 * The 'tag' takes at most 5 bytes. It takes zero bytes if the value is < 30.
 * The format (int/string/etc) is encoded and takes 1 byte.
 * The actual byte count used for the value is format dependent.
 *    numbers take up to 9 bytes for a 64-bit value. (notice that negative numbers are multiplied by -1 before being encoded, so -1 is just 1 byte)
 *    byte-arrays are just copied. Additional byte-count is the length. Also var-encoded.
 *    booleans are free. No bytes taken.
 *
 * Please make sure enough bytes are available in the buffer as no effort is being made to avoid appending after the buffer.
 */

void cmfbuilder_add_int(unsigned char **ptr, unsigned int tag, int value);
void cmfbuilder_add_ulong(unsigned char **ptr, unsigned int tag, unsigned long value);
/*
 * Add-bytes allows the caller to specify the format specifically because the compact message format
 * supports both byte-arrays as well as utf-8 encoded strings.
 * Add the one you want to encoded. Notice that if you pass anything other than CMFMT_STRING_UTF8 or CMFMT_BYTES your
 * stream will be corrupted
 */
void cmfbuilder_add_bytes(unsigned char **ptr, unsigned int tag, const char *data, int length, enum cmf_message_format fmt);
void cmfbuilder_add_bool(unsigned char **ptr, unsigned int tag, bool value);

/* parser API */


/*
 * The cmf parser method is essentially a SOX parser that allows really fast parsing and zero data-copy.
 *
 * The cmfparser_next() method can be called repeatedly until a certain token you wish to find has been located.
 * The actual token values are stored in the cmf_message_parser_token struct which can be reused for all calls.
 */
struct cmf_message_parser_token {
    int tag;
    enum cmf_message_format fmt;
    union {
        unsigned long big_num; /* Used when fmt is CMFMT_POSITIVE_NUMBER */
        long signed_num;       /* Used when fmt is CMFMT_NEGATIVE_NUMBER */
    };
    const unsigned char *begin, *end; /* used for byte arrays and strings */
};

enum cmf_parser_result cmfparser_next(const unsigned char **ptr, const unsigned char * const endMessage, struct cmf_message_parser_token *token);


#endif
