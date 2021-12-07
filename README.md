# SPC

A tweakable block cipher with 128-bit key and block, using the
Lai-Massey structure, like
[FOX64](https://crypto.junod.info/sac04a_talk.pdf) but over 64-bit words
instead of 32-bit.
SPC can be used in CTR mode, can be turned into an authenticated cipher
with SIV or [SCT](https://eprint.iacr.org/2015/1049), for example.


```
Encryption round    Decryption round

  L       R           L       R
  |       |           |       |
  |---⊕---|          unσ      |
  |   |   |           |       |
  |   H   |           |---⊕---|
  |   |   |           |   |   |
  ⊕---+---⊕           |   H   |
  |       |           |   |   |
  σ       |           ⊕---+---⊕
  |       |           |       |
```

Given `L=L1||L2`, with 32-bit `L1` and `L2`, the orthomorphism `σ`
returns  `L2||L1⊕L2`.  The inverse `unσ` undoes this.
The last encryption round also does `σ`.

`H` is the hash function SHA4, which takes as input:

* The 128-bit key
* The 8-bit round counter
* The 64-bit `L⊕R`
* The 56-bit tweak

SHA4 is based on a variant of SipHash, where:

* `v0 = 0x50726f736563636f`
* `v1 = 0x43686f636f6c6174`
* `v2 = 0x01f32d1f4361f48e`
* `v3 = counter || tweak`

The 64-bit output of SHA4 is the result of applying SipHash to the
block `L⊕R`, with 1+2 rounds.

SPC has 4 rounds, which is [necessary and
sufficient](https://eprint.iacr.org/2009/266) for a Lai-Massey scheme to
achieve strong pseudorandomness (if the internal hash is ideal).
SPC's hash does 3 SipHash rounds, for which [distinguishers
exist](https://eprint.iacr.org/2021/189).  But 4 Lai-Massey rounds
involves 12 SipHash rounds, which is expected to prevent any attack on
the full cipher *when the key is secret* (key recovery, plaintext
recovery, distinguishers, with significantly less than 2<sup>128</sup>
operations).

No security claim in the related-key, known-key, chosen-key
models, or against "algorithm faults".

## Cryptanalysis

* Possible 2<sup>64</sup> [distinguisher](https://twitter.com/CryptoOrrDun/status/1467897556894208006)
