#! /usr/bin/env python
#-*- coding: utf-8 -*-


# pycrpto import # 不建议使用 参见https://github.com/dlitz/pycrypto/issues?q=is%3Aopen+is%3Aissue

from Crypto.Cipher import DES, DES3
from Crypto import Random
from Crypto.Util import Counter
import array


KEY = "test"*6

def decrypt_3des(key, text):
    decryptor = Cipher(alg='des_ede3_ecb', key=key, op=0, iv='\0' * 16)
    s = decryptor.update(text)
    return s + decryptor.final()

def encrypt_3des(key, text):
    encryptor = Cipher(alg='des_ede3_ecb', key=key, op=1, iv='\0' * 16)
    s = encryptor.update(text)
    return s + encryptor.final()

if __name__ == "__main__":
    text = "1234abcd"
    # M2Crypto
    encrypt_text = encrypt_3des(KEY, text)
    b64_encrypt_text = base64.b64encode(encrypt_text)
    print "Encrypted: %r" % encrypt_text
    print "Decrypted: %r" % decrypt_3des(KEY, encrypt_text)
    print "b64: %r" % b64_encrypt_text
