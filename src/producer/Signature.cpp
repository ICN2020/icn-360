/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/signing-info.hpp>
#include <ndn-cxx/security/tpm/tpm.hpp>
#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/encoding/buffer.hpp>
#include <ndn-cxx/security/transform/hmac-filter.hpp>
#include <ndn-cxx/security/transform/buffer-source.hpp>
#include <ndn-cxx/security/transform/stream-sink.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include "Signature.h"

static const uint8_t HMAC_KEY[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

Signature::Signature(const char* prefix)
    : sign_type_(ParametersProducer::signatureType())
{
    has_key_ = sign_type_.hasType(SignatureType::RSA) || sign_type_.hasType(SignatureType::ECDSA);
    if(has_key_){
        identity_ = key_chain_.createIdentity(prefix);
        if(sign_type_.hasType(SignatureType::RSA)){
            key_ = key_chain_.createKey(identity_, ndn::RsaKeyParams(sign_type_.keySize()));
        }else if(sign_type_.hasType(SignatureType::ECDSA)){
            key_ = key_chain_.createKey(identity_, ndn::EcKeyParams(sign_type_.keySize()));
        }
        signature_info_ = ndn::security::signingByKey(key_);
    }else if(sign_type_ == SignatureType::SHA_256){
        signature_info_ = ndn::security::signingWithSha256();
    }else if(sign_type_ == SignatureType::HMAC_SHA_256){
        signature_info_ = ndn::security::signingWithSha256();
        auto sig_info = signature_info_.getSignatureInfo();
        sig_info.setSignatureType(ndn::tlv::SignatureHmacWithSha256);
        signature_info_.setSignatureInfo(sig_info);
    }
}


Signature::~Signature()
{
    if(has_key_){
        key_chain_.deleteKey(identity_, key_);
        key_chain_.deleteIdentity(identity_);
    }
}



void
Signature::sign(ndn::Data& data)
{
    // Digest
    if(sign_type_ == SignatureType::NO_SIGN){
        return;
    }else if(sign_type_ == SignatureType::HMAC_SHA_256){
        signWithHmac(data);
    }else{
        key_chain_.sign(data, signature_info_);
    }
}

void
Signature::signWithHmac(ndn::Data& data)
{
    data.setSignature(ndn::Signature(signature_info_.getSignatureInfo()));
    ndn::OBufferStream os;
    
    ndn::EncodingBuffer encoder;
    data.wireEncode(encoder, true);
    
    ndn::security::transform::bufferSource(encoder.buf(), encoder.size()) >>
        ndn::security::transform::hmacFilter(ndn::DigestAlgorithm::SHA256, HMAC_KEY, sizeof(HMAC_KEY)) >>
        ndn::security::transform::streamSink(os);
    
    data.wireEncode(encoder, ndn::Block(ndn::tlv::SignatureValue, os.buf()));
}