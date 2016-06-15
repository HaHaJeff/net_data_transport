#!/usr/bin/python3
#
# Author: MSGF_FUCK (327767852@qq.com)
#
# Description: HMAC-SHA256 implementation in Python 3
#
# Date: 2013-06-10
#
# License: Attribution-NonCommercial-ShareAlike 3.0 Unported
#          (CC BY-NC-SA 3.0)
#================================================================

import binascii
from functools import reduce
from math import log,ceil

def intToList2(number,length):
    """Convert a number into a byte list
       with specified length"""
    return [(number >> i) & 0xff
            for i in reversed(range(0,length*8,8))]

def intToList(number):
    """Converts an integer of any length into an integer list"""
    L1 = log(number,256)
    L2 = ceil(L1)
    if L1 == L2:
        L2 += 1
    return [(number&(0xff<<8*i))>>8*i for i in reversed(range(L2))] 

def listToInt(lst):
    """Convert a byte list into a number"""
    return reduce(lambda x,y:(x<<8)+y,lst)

def bitList32ToList4(lst):
    """Convert a 32-bit list into a 4-byte list"""
    def bitListToInt(lst):
        return reduce(lambda x,y:(x<<1)+y,lst)    
    
    lst2 = []
    for i in range(0,len(lst),8):
        lst2.append(bitListToInt(lst[i:i+8]))
    return list([0]*(4-len(lst2)))+lst2

def list4ToBitList32(lst):
    """Convert a 4-byte list into a 32-bit list"""
    def intToBitList2(number,length):
        """Convert an integer into a bit list
        with specified length"""
        return [(number>>n) & 1
                for n in reversed(range(length))]    
    
    lst2 = []
    for e in lst:
        lst2 += intToBitList2(e,8)
    return list([0]*(32-len(lst2)))+lst2

def add32(p,q,r=None,s=None,t=None):
    """Add up to five 32-bit numbers"""
    mask32 = (1<<32)-1
    p2,q2 = listToInt(p), listToInt(q)
    if t is None:
        if s is None:
            if r is None:
                return intToList2((p2+q2)&mask32,4)
            else:
                r2 = listToInt(r)
                return intToList2((p2+q2+r2)&mask32,4)
        else:
            r2,s2 = listToInt(r),listToInt(s)
            return intToList2((p2+q2+r2+s2)&mask32,4)
    else:
        r2,s2,t2 = listToInt(r),listToInt(s),listToInt(t)
        return intToList2((p2+q2+r2+s2+t2)&mask32,4)

def xor(x,y,z=None):
    """Evaluate the XOR on two or three operands"""
    if z is None:
        return list(i^j for i,j in zip(x,y))
    else:
        return list(i^j^k for i,j,k in zip(x,y,z))
   
def sha256(m):
    """Return the SHA-256 digest of input"""
    def padding(m):
        """Pad message according to SHA-256 rules"""
        def bitListToList(lst):
            """Convert a bit list into a byte list"""
            lst2 = [0]*((8-len(lst)%8)%8)+lst
            return [reduce(lambda x,y:(x<<1)+y,lst2[i*8:i*8+8])
                    for i in range(len(lst2)//8)]
    
        def intToBitList(number):
            """Convert an integer into a bit list"""
            return list(map(int,list(bin(number)[2:])))
    
        if type(m) is int:
            m1 = intToBitList(m)
            L = len(m1)
            k = (447-L)%512
            return bitListToList(m1+[1]+list([0]*k))+intToList2(L,8)
        else:
            m1 = m
            if type(m) is str:
                m1 = list(map(ord,m))
            if not(type(m) is list):
                raise TypeError
            L = len(m1)*8
            k = (447-L)%512
            return m1+bitListToList([1]+list([0]*k))+intToList2(L,8)    
    
    def compress(m):
        """Evaluates SHA-256 compression function to input"""
        def Ch(x,y,z):
            return list([(i&j)^((i^0xff)&k) for i,j,k in zip(x,y,z)])
        
        def Maj(x,y,z):
            return list([(i&j)^(i&k)^(j&k) for i,j,k in zip(x,y,z)])              
        
        def rotRight(p,n):
            """Rotate 32-bit word right by n bits"""
            p2 = list4ToBitList32(p)
            return bitList32ToList4(p2[-n:]+p2[:-n])
        
        def shiftRight(p,n):
            """Shift 32-bit right by n bits"""
            p2 = list4ToBitList32(p)
            return bitList32ToList4(list(bytes(n))+p2[:-n])
        
        def Sigma0(p):
            """SHA-256 function"""
            return xor(rotRight(p,2),rotRight(p,13),rotRight(p,22))
        
        def Sigma1(p):
            """SHA-256 function"""
            return xor(rotRight(p,6),rotRight(p,11),rotRight(p,25))
        
        def sigma0(p):
            """SHA-256 function"""
            return xor(rotRight(p,7),rotRight(p,18),shiftRight(p,3))
        
        def sigma1(p):
            """SHA-256 function"""
            return xor(rotRight(p,17),rotRight(p,19),shiftRight(p,10))            
        
        nonlocal H
        [a,b,c,d,e,f,g,h] = H
        K = [0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2]    
        W = [None]*64
        for t in range(16):
            W[t] = m[t*4:t*4+4]
        for t in range(16,64):
            W[t] = add32(sigma1(W[t-2]),W[t-7],sigma0(W[t-15]),W[t-16])
        for t in range(64):
            T1 = add32(h,Sigma1(e),Ch(e,f,g),intToList2(K[t],4),W[t])
            T2 = add32(Sigma0(a),Maj(a,b,c))
            h = g; g = f; f = e; e = add32(d,T1)
            d = c; c = b; b = a; a = add32(T1,T2)
        H = [add32(x,y) for x,y in zip([a,b,c,d,e,f,g,h],H)]
        
    H0 = [0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
         0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19]
    H = list(map(lambda x:intToList2(x,4),H0))
    mp = padding(m)
    for i in range(0,len(mp),64):
        compress(mp[i:i+64])
    return listToInt([s2 for s1 in H for s2 in s1])

def hmac_sha256(k,m):
    """Return the HMAC-SHA-256 of the input
       HMAC(k,m)=SHA-256((k⊕opad)∥SHA-256((k⊕ipad)∥m))"""
    opad = list([0x5c]*64); ipad = list([0x36]*64)
    if type(k) is int:
        k1 = intToList(k)
        L = len(k1)
        if L > 64:
            K = intToList2(sha256(k),32)+list([0]*32)
        else:
            K = k1+list([0]*(64-L))  
    else:
        k1 = list(map(ord,k))      
        L = len(k1)
        if L > 64:
            K = intToList(sha256(k1))
        else:
            K = k1+list([0]*(64-L))
    if type(m) is int:
        M = intToList(m)
    else:
        M = list(map(ord,m))
    arg1 = xor(K,opad)
    arg2 = xor(K,ipad)
    return sha256(arg1+intToList(sha256(arg2+M)))

if __name__ == '__main__':

    # Wikipedia's test case #1
    assert hmac_sha256("","") == 0xb613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad
    
    # Wikipedia's test case #2
    assert hmac_sha256("key", "The quick brown fox jumps over the lazy dog") == \
           0xf7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
    
    # RFC 4231 - Identifiers and Test Vectors for HMAC-SHA-224, HMAC-SHA-256,
    # HMAC-SHA-384, and HMAC-SHA-512
    
    # RFC 4231 Test case 1
    Key1 = 0x0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b
    Data1 = 0x4869205468657265
    HMAC1 = 0xb0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7    
    assert hmac_sha256(Key1,Data1) == HMAC1      

    # RFC 4231 Test case 2
    Key2 = 0x4a656665
    Data2 = 0x7768617420646f2079612077616e7420666f72206e6f7468696e673f
    HMAC2 = 0x5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843
    assert hmac_sha256(Key2,Data2) == HMAC2
    
    # RFC 4231 Test case 3    
    Key3 = 0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
    Data3 = 0xdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd
    HMAC3 = 0x773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe
    assert hmac_sha256(Key3,Data3) == HMAC3

    # RFC 4231 Test case 4
    Key4 = 0x0102030405060708090a0b0c0d0e0f10111213141516171819
    Data4 = 0xcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd
    HMAC4 = 0x82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b
    assert hmac_sha256(Key4,Data4) == HMAC4

    # RFC 4231 Test case 5
    Key5 = 0x0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c
    Data5 = 0x546573742057697468205472756e636174696f6e 
    HMAC5 = 0xa3b6167473100ee06e0c796c2955552b
    assert hmac_sha256(Key5,Data5)>>128 == HMAC5

    # RFC 4231 Test case 6
    Key6 = 0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
    Data6 = 0x54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374
    HMAC6 = 0x60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54
    assert hmac_sha256(Key6,Data6) == HMAC6

    # RFC 4231 Test case 7    
    Key7 = 0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
    Data7 = 0x5468697320697320612074657374207573696e672061206c6172676572207468616e20626c6f636b2d73697a65206b657920616e642061206c6172676572207468616e20626c6f636b2d73697a6520646174612e20546865206b6579206e6565647320746f20626520686173686564206265666f7265206265696e6720757365642062792074686520484d414320616c676f726974686d2e
    HMAC7 = 0x9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2
    assert hmac_sha256(Key7,Data7) == HMAC7
    
    file_in = open("in.txt")

    file_out = open("out.txt","a")

    str = file_in.read();
    
#    print (str)
    encode_str = binascii.b2a_hex(str.encode("utf8")) 
    
   # print (encode_str)
    encode_int = int(encode_str, 16)
   # print (encode_int) 
    file_out.write(hex(hmac_sha256(Key7,encode_int)))

#    print file_in.readline()
#    print (file_in.name)   

#    print file_in.readline()
#    print("Ok!")
