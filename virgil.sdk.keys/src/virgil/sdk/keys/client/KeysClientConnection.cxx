/**
 * Copyright (C) 2015 Virgil Security Inc.
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

#include <virgil/sdk/keys/client/KeysClientConnection.h>

#include <stdexcept>

#include <virgil/sdk/keys/http/Request.h>
#include <virgil/sdk/keys/http/Response.h>
#include <virgil/sdk/keys/error/KeysError.h>
#include <virgil/sdk/keys/util/JsonKey.h>

#include <virgil/crypto/VirgilByteArray.h>
#include <virgil/crypto/VirgilSigner.h>
#include <virgil/crypto/foundation/VirgilBase64.h>

#include <json.hpp>

using virgil::sdk::keys::client::KeysClientConnection;
using virgil::sdk::keys::http::Connection;
using virgil::sdk::keys::http::Request;
using virgil::sdk::keys::http::Response;
using virgil::sdk::keys::error::KeysError;
using virgil::sdk::keys::util::JsonKey;

using virgil::crypto::VirgilByteArray;
using virgil::crypto::VirgilSigner;
using virgil::crypto::foundation::VirgilBase64;

using json = nlohmann::json;

static const char * kHttpHeaderField_AppToken = "X-VIRGIL-APP-TOKEN";
static const char * kHttpHeaderField_RequestSign = "X-VIRGIL-REQUEST-SIGN";
static const char * kHttpHeaderField_RequestSignPkId = "X-VIRGIL-REQUEST-SIGN-PK-ID";

KeysClientConnection::KeysClientConnection(const std::string& appToken, const std::string& baseAddress)
        : appToken_(appToken), baseAddress_(baseAddress) {
    if (!baseAddress_.empty() && baseAddress_.back() == '/') {
        baseAddress_.pop_back();
    }
}

std::string KeysClientConnection::appToken() const {
    return appToken_;
}

std::string KeysClientConnection::baseAddress() const {
    return baseAddress_;
}

Response KeysClientConnection::send(const Request& request) {
    // Add application token to the header
    auto header = request.header();
    header[kHttpHeaderField_AppToken] = appToken();
    return Connection::send(Request(request).baseAddress(baseAddress_).header(header).contentType("application/json"));
}

Response KeysClientConnection::send(const Request& request, const Credentials& credentials) {
    if (credentials.isValid()) {
        throw std::logic_error("KeysClientConnection: Credentials is not valid.");
    }
    if (request.body().empty()) {
        throw std::logic_error("KeysClientConnection: Request body is empty, so can not be signed.");
    }
    VirgilByteArray sign = VirgilSigner().sign(virgil::crypto::str2bytes(request.body()),
            credentials.privateKey(), virgil::crypto::str2bytes(credentials.privateKeyPassword()));
    auto header = request.header();
    header[kHttpHeaderField_RequestSign] = VirgilBase64::encode(sign);
    if (!credentials.publicKeyId().empty()) {
        header[kHttpHeaderField_RequestSignPkId] = credentials.publicKeyId();
    }
    return send(Request(request).header(header));
}

void KeysClientConnection::checkResponseError(const Response& response, KeysError::Action action) {
    if (response.fail()) {
        json error = json::parse(response.body());
        json errorCode = error[JsonKey::error][JsonKey::errorCode];
        throw KeysError(action, response.statusCode(),
                errorCode.is_number() ? errorCode.get<unsigned int>() : KeysError::undefinedErrorCode);
    }
}
