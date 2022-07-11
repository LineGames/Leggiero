cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_Crypto STATIC)
le_tgt_setCppOptions(LE_M_Crypto)


target_include_directories(LE_M_Crypto PUBLIC ..)


find_library(nlib-z z)

target_link_libraries(LE_M_Crypto
    PRIVATE
        ${nlib-z}
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA
)


target_sources(LE_M_Crypto
    PUBLIC
        CryptoModuleInterface.h
        Encrypt/AESUtil.h Encrypt/RSAUtil.h
        Hash/HashUtility.h Hash/HashContext.h Hash/CRC32Util.h Hash/MD5Util.h Hash/SHAUtil.h
        Sign/HMAC_SHA_Util.h
        ProtectedKey/IProtectedKey.h ProtectedKey/NotSecureKey.h
        OpenSSLCrypto/OpenSSLUtility.h
        
    PRIVATE
        CryptoModuleInterface.cpp
        Encrypt/AESUtil_OpenSSL.cpp Encrypt/RSAUtil_OpenSSL.cpp
        Hash/CRC32Util_zlib.cpp Hash/MD5Util_OpenSSL.cpp Hash/SHAUtil_OpenSSL.cpp
        Sign/HMAC_SHA_Util_OpenSSL.cpp
        ProtectedKey/NotSecureKey.cpp
        OpenSSLCrypto/OpenSSLUtility.cpp
)
