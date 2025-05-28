#!/bin/python3
#
# This file supports basic ASN.1 decode.


from enum import Enum

class ASN1Types(Enum):
    INTEGER = 0x02
    BITFIELD = 0x03
    OCTETSTRING = 0x04
    OID = 0x06
    SEQUENCE = 0x30
    CONTEXT = 0xa0
    CHOICE = 0xa1

# Manage output to console.
def consoleprint(quiet, msg):
    '''
        Manage output to console.
    '''
    if quiet is False:
        print(msg)

# Handle the sequence type.
def handle_sequence(data, quiet=False):
    # Field length indicates the size of the sequence block, but is only a container.
    # Length of the data for this type is 2 bytes.
    # No data is captured.
    flen = data[1]

    consoleprint(quiet, "  SEQUENCE ")
    consoleprint(quiet, "    LENGTH: %d" % flen)

    return 2, None

# Handle the integer type.
def handle_integer(data, quiet=False):
    # Field length indicates the size of the integer block.
    # Length of the data is (2 + field length).
    flen = data[1]
    value = data[2:2+flen]

    consoleprint(quiet, "  INTEGER")
    consoleprint(quiet, "    SIZE:   %d" % flen)
    consoleprint(quiet, "    VALUE:  %s" % ' '.join(('%02x' % x for x in value)))

    return 2 + flen, value

# Handle the bitfield type.
def handle_bitfield(data, quiet=False):
    # Field length indicates the size of the bitfield block.
    # This is followed by an 'unused-bits count'.
    # This is followed by a data-format byte (in practice, always 04 = uncompressed-format).
    # length of the data is (2 + field length).
    flen = data[1]
    unusedbits = data[2]

    # Format is the uncompressed (04) or compressed (02/03) format.
    # 02/03 are not supported, so this is always expectecd to be 04.
    format = data[3]

    value = data[4:2+flen]

    consoleprint(quiet, "  BITFIELD")
    consoleprint(quiet, "    SIZE:   %d" % flen)
    consoleprint(quiet, "    UNUSED: %02x" % unusedbits)
    consoleprint(quiet, "    FORMAT: %02x" % format)
    consoleprint(quiet, "    VALUE: %s" % ' '.join(('%02x' % x for x in value)))

    return 2 + flen, value


# Handle the octetstring type.
def handle_octetstring(data, quiet=False):
    # Field length indicates the size of the octetstring block.
    # length of the data is (2 + field length).
    flen = data[1]
    value = data[2:2+flen]

    consoleprint(quiet, "  OCTETSTRING")
    consoleprint(quiet, "    SIZE:   %d" % flen)
    consoleprint(quiet, "    VALUE:  %s" % ' '.join(('%02x' % x for x in value)))

    return 2 + flen, value


# Handle the OID type.
def handle_oid(data, quiet=False):
    # OID is formatted by the first two values being combined into one byte,
    # then each value after is one byte unless the value itself is greater
    # than 128, in which case it is a two-byte value with the first byte containing 0x80.
    # Returns an array decoded of integer values.
    flen = data[1]
    value = data[2:2+flen]

    consoleprint(quiet, "  OID")
    consoleprint(quiet, "    LENGTH: %d" % flen)
    consoleprint(quiet, "    VALUE:  %s" % ' '.join(('%d' % x for x in value)))

    oid = []

    # First two values are combined into the first byte by multiplying the first value
    # by 40 and adding the second value.
    oid.append(int(value[0] / 40))
    oid.append(int(value[0] % 40))

    temp = []
    result = 0
    # Loop through the data and extract values.
    for i in range(1, len(value)):
        v = value[i]

        # Store the value(s) in an array, stripping off the uppermost bit.
        temp.append(v & 0x7F)

        # When we enconter a value < 128, the base-128 encoding stops and we
        # take the accumulated values for decode.
        if v < 128:
            # By reversing the array, we can use an incrementing offset value for
            # shifting the byte and accumulate the result.  The values are shifted
            # by 128 (left-shift 7) for each byte (8 bits) beyond the first byte.
            for index, t in enumerate(reversed(temp), start=0):
                # First byte does not get shifted.
                # Subsequent bytes are shifted by 7 * byte position.
                offset = 0 if index == 0 else (index * 8) - 1
                result |= t << offset

            # Save the result as a value in the OID array and reset.
            oid.append(result)
            temp.clear()
            result = 0

    consoleprint(quiet, "    OID:    %s" % ' '.join(('%d' % x for x in oid)))

    return 2 + flen, oid


# Context (container).
def handle_context(data, quiet=False):
    # Field length indicates the size of the sequence block, but is only a container.
    # Length of the data for this type is 2 bytes.
    # No data is captured.
    flen = data[1]

    consoleprint(quiet, "  CONTEXT")
    consoleprint(quiet, "    LENGTH: %d" % flen)

    return 2, None


# Choice (container).
def handle_choice(data, quiet=False):
    # Field length indicates the size of the sequence block, but is only a container.
    # Length of the data for this type is 2 bytes.
    # No data is captured.
    flen = data[1]

    consoleprint(quiet, "  CHOICE")
    consoleprint(quiet, "    LENGTH: %d" % flen)

    return 2, None


# Default handler.
def handle_default(data, quiet=False):
    flen = data[1]
    value = data[2:2+flen]

    consoleprint(quiet, "  UNKNOWN TYPE (%02x)" % data[0])
    consoleprint(quiet, "    SIZE:   %d" % flen)
    consoleprint(quiet, "    VALUE:  %s" % ' '.join(('%02x' % x for x in value)))

    return 2 + flen, value


# Registered data handlers.
datatype_handlers = {
                        ASN1Types.INTEGER.value: handle_integer,
                        ASN1Types.BITFIELD.value: handle_bitfield,
                        ASN1Types.OCTETSTRING.value: handle_octetstring,
                        ASN1Types.OID.value: handle_oid,
                        ASN1Types.SEQUENCE.value: handle_sequence,
                        ASN1Types.CONTEXT.value: handle_context,
                        ASN1Types.CHOICE.value: handle_choice
                    }

# Get the next object from the data stream.
def getobject(data, quiet=False):
    # Decode the ASN.1 data.
    datatype = data[0]

    if datatype in datatype_handlers:
        size, value = datatype_handlers[datatype](data, quiet)
    else:
        size, value = handle_default(data, quiet)

    return datatype, size, value
