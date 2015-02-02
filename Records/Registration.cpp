
#include "Registration.hpp"

#include "../utils.hpp"
#include <botan/base64.h>
#include <json/json.h>

/*
    let "central" be:
        std::string name_;
        std::vector<std::pair<std::string,std::string>> subdomains_;
        uint8_t consensusHash_[SHA256_LEN];
        uint8_t nonce_[NONCE_LEN];
        std::string contact_;
        long timestamp_;
        pubKey

    for each nonce, generate:
        uint8_t scrypted_[SCRYPTED_LEN]; //scrypt output of {central}
        uint8_t signature_[SIGNATURE_LEN]; //digital sig of {central, scrypted_}
    registration valid iff SHA384{central, scrypted_, signature_} < THRESHOLD
    then save as {central, scrypted_, signature_} in JSON format

    computational operator won't know when to stop, since the sig matters
    must use deterministic sig alg!
*/

Registration::Registration(Botan::RSA_PrivateKey* key, uint8_t* consensusHash,
    const std::string& name, const std::string& contact):
    Record(key, consensusHash)
{
    setName(name);
    setContact(contact);
}



bool Registration::setName(const std::string& newName)
{
    if (newName.empty() || newName.length() > 32)
        return false;

    name_ = newName;
    valid_ = false;
    return true;
}



bool Registration::addSubdomain(const std::string& from, const std::string& to)
{
    if (subdomains_.size() >= 16 || from.size() > 32 || to.size() > 32)
        return false;

    subdomains_.push_back(std::make_pair(from, to));
    valid_ = false; //need new nonce now

    return true;
}



bool Registration::setContact(const std::string& contactInfo)
{
    if (!Utils::isPowerOfTwo(contactInfo.length()))
        return false;

    contact_ = contactInfo;
    valid_ = false; //need new nonce now
    return true;
}



bool Registration::makeValid(uint8_t nCPUs)
{
    //TODO: if issue with fields other than nonce, return false

    return mineParallel(nCPUs);
}



std::string Registration::asJSON() const
{
    Json::Value obj;

    //add all static fields
    obj["name"] = name_;
    obj["pgp"] = contact_;
    obj["t"] = std::to_string(timestamp_);
    obj["cHash"] = Botan::base64_encode(consensusHash_, SHA256_LEN);

    //add any subdomains
    for (auto sub : subdomains_)
        obj["subd"][sub.first] = sub.second;

    //extract and save public key
    auto ber = Botan::X509::BER_encode(*key_);
    uint8_t* berBin = new uint8_t[ber.size()];
    memcpy(berBin, ber, ber.size());
    obj["pubKey"] = Botan::base64_encode(berBin, ber.size());

    //if the domain is valid, add nonce_, scrypted_, and signature_
    if (isValid())
    {
        obj["n"] = Botan::base64_encode(nonce_, NONCE_LEN);
        obj["scrypt"] = Botan::base64_encode(scrypted_, SCRYPTED_LEN);
        obj["sig"] = Botan::base64_encode(signature_, SIGNATURE_LEN);
    }

    //output in compressed (non-human-friendly) format
    Json::FastWriter writer;
    return writer.write(obj);
}



std::ostream& operator<<(std::ostream& os, const Registration& dt)
{
    os << "Domain Registration: (currently " <<
        (dt.valid_ ? "VALID)" : "INVALID)") << std::endl;
    os << "   Name: " << dt.name_ << " -> " << dt.getOnion() << std::endl;
    os << "   Subdomains: ";

    if (dt.subdomains_.empty())
        os << "(none)";
    else
        for (auto subd : dt.subdomains_)
            os << std::endl << "      " << subd.first << " -> " << subd.second;
    os << std::endl;

    os << "   Contact: 0x" << dt.contact_ << std::endl;
    os << "   Time: " << dt.timestamp_ << std::endl;
    os << "   Validation:" << std::endl;

    os << "      Day Consensus: " <<
        Botan::base64_encode(dt.consensusHash_, dt.SHA256_LEN) << std::endl;

    os << "      Nonce: ";
    if (dt.isValid())
        os << Botan::base64_encode(dt.nonce_, dt.NONCE_LEN) << std::endl;
    else
        os << "<regeneration required>" << std::endl;

    os << "      Proof of Work: ";
    if (dt.isValid())
        os << Botan::base64_encode(dt.scrypted_, dt.SCRYPTED_LEN) << std::endl;
    else
        os << "<regeneration required>" << std::endl;

    os << "      Signature: ";
    if (dt.isValid())
        os << Botan::base64_encode(dt.signature_, dt.SIGNATURE_LEN / 4) <<
            " ..." << std::endl;
    else
        os << "<regeneration required>" << std::endl;

    auto pem = Botan::X509::PEM_encode(*dt.key_);
    pem.pop_back(); //delete trailing /n
    Utils::stringReplace(pem, "\n", "\n\t");
    os << "      RSA Public Key: \n\t" << pem;

    return os;
}


//********************* PRIVATE METHODS ****************************************


UInt32Data Registration::getCentral(uint8_t* nonce) const
{
    std::string str;
    str += name_;
    for (auto subd : subdomains_)
        str += subd.first + subd.second;
    str += contact_;
    str += std::to_string(timestamp_);

    int index = 0;
    auto pubKey = getPublicKey();
    const size_t centralLen = str.length() + SHA256_LEN + NONCE_LEN + pubKey.second;
    uint8_t* central = new uint8_t[centralLen];

    memcpy(central + index, str.c_str(), str.size()); //copy string into array
    index += str.size();

    memcpy(central + index, consensusHash_, SHA256_LEN);
    index += SHA256_LEN;

    memcpy(central + index, nonce, NONCE_LEN);
    index += NONCE_LEN;

    memcpy(central + index, pubKey.first, pubKey.second);

    //std::cout << Botan::base64_encode(central, centralLen) << std::endl;

    return std::make_pair(central, centralLen);
}
