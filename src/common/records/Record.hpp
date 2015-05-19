
#ifndef RECORD_HPP
#define RECORD_HPP

#include <botan-1.10/botan/botan.h>
#include <botan-1.10/botan/rsa.h>
#include <cstdint>
#include <string>

typedef std::pair<uint8_t*, size_t> UInt8Array;
typedef std::vector<std::pair<std::string, std::string>> NameList;

class Record
{
 public:
  static const uint64_t SCR_N = 1 << 20;  // RAM load = O(SCR_N)
  static const uint32_t SCR_P = 1 << 0;   // CPU time = O(SCR_N * SCR_P)

  static const uint8_t NONCE_LEN = 4;
  static const uint8_t SCRYPTED_LEN = 16;

  static const uint32_t RSA_LEN = 1024;
  static const uint32_t SIGNATURE_LEN = RSA_LEN / 8;
  // static const uint32_t SHA256_LEN = 256 / 8;
  static const uint32_t SHA384_LEN = 384 / 8;
  static const uint32_t SHA1_LEN = 160 / 8;

  enum WorkStatus
  {
    Success,
    NotFound,
    Aborted
  };

  Record(Botan::RSA_PublicKey*);
  Record(Botan::RSA_PrivateKey*, uint8_t*);
  Record(const Record&);

  void setNameList(const NameList&);
  NameList getNameList();

  void setContact(const std::string&);
  std::string getContact();

  bool setKey(Botan::RSA_PrivateKey*);
  UInt8Array getPublicKey() const;
  std::string getOnion() const;

  bool refresh();
  void makeValid(uint8_t);
  void computeValidity(bool*);  // updates valid_, with flag to abort work
  bool isValid() const;
  bool hasValidSignature() const;

  // virtual bool makeValid(uint8_t);
  std::string getType();
  virtual uint32_t getDifficulty() const;
  virtual std::string asJSON() const;
  friend std::ostream& operator<<(std::ostream&, const Record&);

 protected:
  WorkStatus makeValid(uint8_t, uint8_t, bool*);
  virtual UInt8Array computeCentral();
  void updateAppendSignature(UInt8Array& buffer);
  int updateAppendScrypt(UInt8Array& buffer);
  void updateValidity(const UInt8Array& buffer);

  std::string type_;
  NameList nameList_;
  std::string contact_;

  Botan::RSA_PrivateKey* privateKey_;
  Botan::RSA_PublicKey* publicKey_;

  uint8_t consensusHash_[SHA384_LEN];
  uint8_t nonce_[NONCE_LEN];
  uint8_t scrypted_[SCRYPTED_LEN];
  uint8_t signature_[SIGNATURE_LEN];
  long timestamp_;
  bool valid_, validSig_;
};

#endif
