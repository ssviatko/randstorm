# randstorm

A small C program that generates pseudo-random data by repeatedly hashing with SHA-512, seeded from a passphrase, and reports how long the generation took.

## How it works

1. The passphrase is hashed with SHA-512 to produce a 64-byte seed.
2. For each requested megabyte, a 1 MB buffer is filled in 64-byte blocks. Each block is the SHA-512 hash of the previous block (`hash = SHA512(hash)`).
3. The elapsed time for the seeding and generation steps is measured with `CLOCK_MONOTONIC` and printed with microsecond resolution.

The same passphrase always produces the same stream.

## Current status

The generated data is only kept in memory. The 1 MB buffer is overwritten on every pass, and nothing is written to a file or to stdout. The program currently works as a throughput benchmark of the generator. Add output code after the generation loop if you need the data itself.

## Building

Requires `gcc` and a POSIX system (uses `getopt` and `clock_gettime`).

```
make        # builds ./randstorm
make clean  # removes object files and the binary
```

## Usage

```
./randstorm -n <megabytes to generate> -s <passphrase, max 64 characters>
```

Both options are required.

| Option | Description |
|--------|-------------|
| `-n`   | Number of megabytes to generate. Must be an integer. |
| `-s`   | Passphrase used to seed the generator. Up to 64 characters. |

Example:

```
./randstorm -n 100 -s "correct horse battery staple"
```

The program prints the megabyte count, the passphrase, the seed hash in hex, and the time used:

```
megabytes = 100
passphrase = 'correct horse battery staple'
seeded hash from passphrase = <128 hex characters>
time used: S.UUUUUU seconds (N us)
```

The program exits with status 1 and a usage message if an option is missing, the integer is invalid, or the passphrase is longer than 64 characters.

## Files

| File | Purpose |
|------|---------|
| `main.c` | Argument parsing, generation loop and timing |
| `sha2.c`, `sha2.h` | SHA-224/256/384/512 implementation |
| `Makefile` | Build rules |

## Security note

This is not a vetted cryptographically secure random number generator. Iterated hashing from a passphrase is deterministic, and its output is only as unpredictable as the passphrase. Do not use it to generate keys or other secrets.
