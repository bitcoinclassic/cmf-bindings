#include "cmf.h"

#ifdef NDEBUG
# error run the test in debug mode otherwise you wont get any results
#endif
#include <assert.h>

#include <stdio.h>
#include <string.h>

void basic_test1()
{
    unsigned char buf[100];
    unsigned char *ptr = buf;
    struct cmf_message_parser_token token;
    enum cmf_parser_result found;

    cmfbuilder_add_int(&ptr, 15, 6512);
    assert(ptr > buf);
    // printf( "size: %d\n", ptr - buf);
    assert(ptr - buf == 3);
    assert(buf[0] == 120);
    assert((unsigned char) buf[1] == 177);
    assert(buf[2] == 112);

    const unsigned char *parsePtr = buf;
    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_FOUND_TOKEN);
    assert(token.tag == 15);
    assert(token.fmt == CMFMT_POSITIVE_NUMBER);
    assert(token.big_num == 6512);

    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_DOCUMENT_END);
}

void basic_test2()
{
    unsigned char buf[100];
    unsigned char *ptr = buf;
    struct cmf_message_parser_token token;
    enum cmf_parser_result found;

    cmfbuilder_add_int(&ptr, 129, 6512);
    assert(ptr > buf);
    // printf( "size: %d\n", ptr - buf);
    assert(ptr - buf == 5);
    assert((unsigned char) buf[0] == 248);
    assert((unsigned char) buf[1] == 128);
    assert(buf[2] == 1);
    assert((unsigned char) buf[3] == 177);
    assert(buf[4] == 112);

    const unsigned char *parsePtr = buf;
    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_FOUND_TOKEN);
    assert(token.tag == 129);
    assert(token.fmt == CMFMT_POSITIVE_NUMBER);
    assert(token.big_num == 6512);

    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_DOCUMENT_END);
}

void test_types()
{
    unsigned char buf[100];
    unsigned char *ptr = buf;
    struct cmf_message_parser_token token;
    enum cmf_parser_result found;

    const char *foo = "Föo";
    assert(strlen(foo) == 4); // someone changed encoding of this source file
    cmfbuilder_add_bytes(&ptr, 1, foo, 4, CMFMT_STRING_UTF8);
    const char *hihi = "hihi";
    cmfbuilder_add_bytes(&ptr, 200, hihi, 4, CMFMT_BYTES);
    cmfbuilder_add_bool(&ptr, 3, true);
    cmfbuilder_add_bool(&ptr, 40, false);
    assert(ptr > buf);
    // printf( "size: %d\n", ptr - buf);
    assert(ptr - buf == 17);
    // string '1'
    assert((unsigned char) buf[0] == 10);
    assert((unsigned char) buf[1] == 4); // serialized string length
    assert((unsigned char) buf[2] == 70);
    assert((unsigned char) buf[3] == 195);
    assert((unsigned char) buf[4] == 182);
    assert((unsigned char) buf[5] == 111);

    // blob '200'
    assert((unsigned char) buf[6] == 251);
    assert((unsigned char) buf[7] == 128);
    assert((unsigned char) buf[8] == 72);
    assert((unsigned char) buf[9] == 4); // length of bytearray
    assert((unsigned char) buf[10] == 104); // 'h'
    assert((unsigned char) buf[11] == 105); // 'i'
    assert((unsigned char) buf[12] == 104); // 'h'
    assert((unsigned char) buf[13] == 105); // 'i'

    // bool-true '3'
    assert((unsigned char) buf[14] == 28);

    // bool-false '40'
    assert((unsigned char) buf[15] == 253);
    assert((unsigned char) buf[16] == 40);

    const unsigned char *parsePtr = buf;
    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_FOUND_TOKEN);
    assert(token.tag == 1);
    assert(token.fmt == CMFMT_STRING_UTF8);
    assert(token.begin > buf);
    assert(token.begin < ptr);
    assert(token.end > buf);
    assert(token.end < ptr);
    assert(token.end - token.begin == 4);
    assert(memcmp(foo, token.begin, 4) == 0); // be careful, no trailing zero!

    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_FOUND_TOKEN);
    assert(token.tag == 200);
    assert(token.fmt == CMFMT_BYTES);
    assert(token.begin > buf);
    assert(token.begin < ptr);
    assert(token.end > buf);
    assert(token.end < ptr);
    assert(token.end - token.begin == 4);
    assert(memcmp(hihi, token.begin, 4) == 0); // be careful, no trailing zero!

    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_FOUND_TOKEN);
    assert(token.tag == 3);
    assert(token.fmt == CMFMT_BOOL_TRUE);
    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_FOUND_TOKEN);
    assert(token.tag == 40);
    assert(token.fmt == CMFMT_BOOL_FALSE);

    found = cmfparser_next(&parsePtr, ptr, &token);
    assert(found == CMF_DOCUMENT_END);
}

int main(int argc, char *argv[])
{
    basic_test1();
    basic_test2();
    test_types();

    return 0;
}

