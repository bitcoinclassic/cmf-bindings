#include "cmf.h"

#include <assert.h>
#include <string.h> // for memcpy

static bool cmf_unserialize(const unsigned char **data, const unsigned char * const endMessage, unsigned long *result)
{
    assert(data);
    assert(*data);
    assert(result);
    assert(endMessage);

    const unsigned char *ptr = *data;
    while (ptr < endMessage) {
        unsigned char byte = *ptr++;
        *result = (*result << 7) | (byte & 0x7F);
        if (byte & 0x80)
            *result += 1;
        else {
            *data = ptr;
            return true;
        }
    }
    return false;
}

static void cmf_serialize(unsigned char **data, unsigned long value)
{
    unsigned char *start = *data;
    unsigned char *pos = start;
    while (true) {
        *pos = (unsigned char) ((value & 0x7F) | (pos != start ? 0x80 : 0x00));
        if (value <= 0x7F)
            break;
        value = (value >> 7) - 1;
        ++pos;
    }
    *data = pos + 1;

    // reverse
    while (pos > start) {
        unsigned char tmp = *start; // swap
        *start = *pos;
        *pos = tmp;
        ++start;
        --pos;
    }
}

static void cmf_write(unsigned char **data, unsigned int tag, short type) {
    assert(type < 8);
    if (tag >= 31) { // use more than 1 byte
        unsigned char byte = type | 0xF8; // set the 'tag' to all 1s
        *data[0] = byte;
        *data += 1;
        cmf_serialize(data, tag);
    }
    else {
        assert(tag < 32);
        unsigned char byte = tag;
        byte = byte << 3;
        byte += type;
        *data[0] = byte;
        *data += 1;
    }
}

void cmfbuilder_add_int(unsigned char **ptr, unsigned int tag, int value)
{
    short type;
    if (value >= 0) {
        type = CMFMT_POSITIVE_NUMBER;
    } else {
        type = CMFMT_NEGATIVE_NUMBER;
        value *= -1;
    }
    cmf_write(ptr, tag, type);
    cmf_serialize(ptr, value);
}

void cmfbuilder_add_ulong(unsigned char **ptr, unsigned int tag, unsigned long value)
{
    cmf_write(ptr, tag, CMFMT_POSITIVE_NUMBER);
    cmf_serialize(ptr, value);
}

void cmfbuilder_add_bytes(unsigned char **ptr, unsigned int tag, const char *data, int length, enum cmf_message_format fmt)
{
    assert(fmt == CMFMT_STRING_UTF8 || fmt == CMFMT_BYTES);
    cmf_write(ptr, tag, fmt);
    cmf_serialize(ptr, length);
    memcpy(*ptr, data, length);
    *ptr += length;
}

void cmfbuilder_add_bool(unsigned char **ptr, unsigned int tag, bool value)
{
    cmf_write(ptr, tag, value ? CMFMT_BOOL_TRUE : CMFMT_BOOL_FALSE);
}


enum cmf_parser_result cmfparser_next(const unsigned char **ptr, const unsigned char * const endMessage, struct cmf_message_parser_token *token)
{
    if (*ptr >= endMessage)
        return CMF_DOCUMENT_END;

    unsigned char byte = *ptr[0];
    token->fmt = (enum cmf_message_format)(byte & 0x07);
    token->tag = byte >> 3;
    if (token->tag == 31) { // the tag is stored in the next byte(s)
        unsigned long tag = 0;
        *ptr += 1;
        bool ok = cmf_unserialize(ptr, endMessage, &tag);
        if (!ok || tag > 0xFFFFFFFF) {
            *ptr -= 1;
            return CMF_PARSER_ERROR;
        }
        *ptr -= 1;
        token->tag = (unsigned int) tag;
    }

    unsigned long value = 0;

    switch (token->fmt) {
    case CMFMT_POSITIVE_NUMBER:
    case CMFMT_NEGATIVE_NUMBER: {
        *ptr += 1;
        bool ok = cmf_unserialize(ptr, endMessage, &value);
        if (!ok) {
            *ptr -= 1;
            return CMF_PARSER_ERROR;
        }
        if (token->fmt == CMFMT_NEGATIVE_NUMBER)
            token->signed_num = (long) (value * -1);
        else
            token->big_num = value;
        break;
    }
    case CMFMT_BYTES:
    case CMFMT_STRING_UTF8: {
        *ptr += 1;
        bool ok = cmf_unserialize(ptr, endMessage, &value);
        if (!ok) {
            *ptr -= 1;
            return CMF_PARSER_ERROR;
        }
        token->begin = *ptr;
        token->end = token->begin + value;
        if (token->end > endMessage) // The actual value is not included in the message
            return CMF_PARSER_ERROR;
        *ptr += value;
        break;
    }
    case CMFMT_BOOL_TRUE:
    case CMFMT_BOOL_FALSE:
        *ptr += 1;
        break;
    default:
        return CMF_PARSER_ERROR;
    }

    return CMF_FOUND_TOKEN;
}
