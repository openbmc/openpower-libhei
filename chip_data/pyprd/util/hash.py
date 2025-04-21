#!/usr/bin/env python3

import argparse


def hash_string(num_bytes: int, string: str) -> int:
    """
    Converts a string into an integer hash value. This is primarily used to
    convert register and isolation node names from the Chip and RAS Data into
    integer values to save space in the data.
    """

    # This hash is a simple "n*s[0] + (n-1)*s[1] + ... + s[n-1]" algorithm,
    # where s[i] is a chunk from the input string the length of i_bytes.

    # Currently only supporting 1:8 byte hashes
    assert 1 <= num_bytes and num_bytes <= 8

    # Start hashing each chunk
    sumA = 0
    sumB = 0

    # Iterate one chunk at a time
    for i in range(0, len(string), num_bytes):
        # Combine each chunk into a single integer value. If we reach the end
        # of the string, pad with null characters.
        chunk = 0
        for j in range(0, num_bytes):
            chunk <<= 8
            chunk |= ord(string[i + j]) if (i + j < len(string)) else ord("\0")

        # Apply the simple hash
        sumA += chunk
        sumB += sumA

    # Mask off everything except the target number of bytes.
    mask = 0xFFFFFFFFFFFFFFFF
    sumB &= mask >> ((8 - num_bytes) * 8)

    return sumB


def hash_string_format(num_bytes: int, string: str) -> str:
    """
    Returns a formatted hex string of the given string's hash value.
    """
    return "{0:0{1}x}".format(hash_string(num_bytes, string), num_bytes * 2)


if __name__ == "__main__":

    cli = argparse.ArgumentParser()
    cli.add_argument("num_bytes", help="Number of bytes to hash to")
    cli.add_argument("string_name", help="String to hash")
    args = cli.parse_args()
    print(hex(hash_string(int(args.num_bytes), args.string_name)))
