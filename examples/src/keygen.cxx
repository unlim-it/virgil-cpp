/**
 * Copyright (C) 2014 Virgil Security Inc.
 *
 * Lead Maintainer: Virgil Security Inc. <support@virgilsecurity.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     (1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *     (2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *
 *     (3) Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>

#include <virgil/VirgilByteArray.h>
using virgil::VirgilByteArray;
#include <virgil/service/data/VirgilKeyPair.h>
using virgil::service::data::VirgilKeyPair;

int main(int argc, char **argv) {
    try {
        std::cout << "Generate keys with with password: 'password'" << std::endl;
        VirgilKeyPair newKeyPair(virgil::str2bytes("password"));

        std::cout << "Store public key: new_public.key ..." << std::endl;
        std::ofstream publicKeyStream("new_public.key", std::ios::out | std::ios::binary);
        if (!publicKeyStream.good()) {
            throw std::runtime_error("can not write file: new_public.key");
        }
        VirgilByteArray publicKey = newKeyPair.publicKey();
        std::copy(publicKey.begin(), publicKey.end(), std::ostreambuf_iterator<char>(publicKeyStream));

        std::cout << "Store private key: new_private.key ..." << std::endl;
        std::ofstream privateKeyStream("new_private.key", std::ios::out | std::ios::binary);
        if (!privateKeyStream.good()) {
            throw std::runtime_error("can not write file: new_private.key");
        }
        VirgilByteArray privateKey = newKeyPair.privateKey();
        std::copy(privateKey.begin(), privateKey.end(), std::ostreambuf_iterator<char>(privateKeyStream));
    } catch (std::exception& exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
    }
    return 0;
}
