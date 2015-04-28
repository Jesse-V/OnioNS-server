
#include "CreateR.hpp"
#include "../../common/CommonProtocols.hpp"


CreateR::CreateR(Botan::RSA_PrivateKey* key,
   const std::string& primaryName, const std::string& contact):
   Record(key, CommonProtocols::get().computeConsensusHash())
{
   NameList nameList;
   nameList.push_back(std::make_pair(primaryName, getOnion()));
   setNameList(nameList);

   setContact(contact);
}
