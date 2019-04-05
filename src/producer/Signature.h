/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once

#include <ndn-cxx/security/v2/key-chain.hpp>
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/name.hpp>

#include "ParametersProducer.h"

class Signature 
{
private:
    ndn::security::v2::KeyChain key_chain_;
    ndn::security::Identity identity_;
    ndn::security::Key key_;
    SignatureType sign_type_;
    ndn::security::SigningInfo signature_info_;

    bool has_key_;


public:
    Signature(const char* prefix);
    ~Signature();

    void sign(ndn::Data& data);

private:
    void signWithHmac(ndn::Data& data);
};