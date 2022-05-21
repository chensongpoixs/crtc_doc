/***********************************************************************************************
created: 		2022-05-22

author:			chensong

purpose:		TLSv1.3 协议的学习

原因是WebRTC中有DTSTransport --> 
************************************************************************************************/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <iostream>
#include <mutex>
#include <openssl/asn1.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include <vector>
#include <map>
#include <string>
#include "clog.h"
///////////////////////////////////////////////////////////////////////////////////////
enum class CryptoSuite
{
	NONE = 0,
	AES_CM_128_HMAC_SHA1_80 = 1,
	AES_CM_128_HMAC_SHA1_32,
	AEAD_AES_256_GCM,
	AEAD_AES_128_GCM
};
struct SrtpCryptoSuiteMapEntry
{
	CryptoSuite cryptoSuite;
	const char* name;
};


enum class FingerprintAlgorithm
{
	NONE = 0,
	SHA1 = 1,
	SHA224,
	SHA256,
	SHA384,
	SHA512
};


struct Fingerprint
{
	FingerprintAlgorithm algorithm{ FingerprintAlgorithm::NONE };
	std::string value;
};

////////////////////////////////////////////////////////////////
std::vector<SrtpCryptoSuiteMapEntry>  srtpCryptoSuites =
{
	{ CryptoSuite::AEAD_AES_256_GCM, "SRTP_AEAD_AES_256_GCM" },
	{ CryptoSuite::AEAD_AES_128_GCM, "SRTP_AEAD_AES_128_GCM" },
	{ CryptoSuite::AES_CM_128_HMAC_SHA1_80, "SRTP_AES128_CM_SHA1_80" },
	{ CryptoSuite::AES_CM_128_HMAC_SHA1_32, "SRTP_AES128_CM_SHA1_32" }
};



std::map<std::string, FingerprintAlgorithm> string2FingerprintAlgorithm =
{
	{ "sha-1",  FingerprintAlgorithm::SHA1   },
	{ "sha-224",FingerprintAlgorithm::SHA224 },
	{ "sha-256",FingerprintAlgorithm::SHA256 },
	{ "sha-384",FingerprintAlgorithm::SHA384 },
	{ "sha-512",FingerprintAlgorithm::SHA512 }
};

///////////////////////////////////////////////////
/* Static. */

static std::once_flag globalInitOnce;


 
const char * dtlsCertificateFile = "./certs/fullchain.pem";
const char * dtlsPrivateKeyFile = "./certs/privkey.pem";


// clang-format off
static  int DtlsMtu{ 1350 };
/* Class variables. */

thread_local X509*  certificate{ nullptr };
thread_local EVP_PKEY*  privateKey{ nullptr };
thread_local SSL_CTX*  sslCtx{ nullptr };

thread_local std::vector<Fingerprint>  localFingerprints;


// 读取公钥和私钥
void ReadCertificateAndPrivateKeyFromFiles()
{
	FILE* file{ nullptr };

	file = fopen(dtlsCertificateFile, "r");

	if (!file)
	{
		ERROR_EX_LOG("error reading DTLS certificate file: %s", std::strerror(errno));

		return;
	}

	certificate = PEM_read_X509(file, nullptr, nullptr, nullptr);

	if (!certificate)
	{
		ERROR_EX_LOG("PEM_read_X509() failed");

		return;
	}

	fclose(file);

	file = fopen(dtlsPrivateKeyFile, "r");

	if (!file)
	{
		ERROR_EX_LOG("error reading DTLS private key file: %s", std::strerror(errno));

		return;
	}

	privateKey = PEM_read_PrivateKey(file, nullptr, nullptr, nullptr);

	if (!privateKey)
	{
		ERROR_EX_LOG("PEM_read_PrivateKey() failed");

		return;
	}

	fclose(file);
}

//DTLS状态回调
inline static void onSslInfo(const SSL* ssl, int where, int ret)
{
	DEBUG_EX_LOG("[where = %d][ret = %d]", where, ret);
	//static_cast<RTC::DtlsTransport*>(SSL_get_ex_data(ssl, 0))->OnSslInfo(where, ret);
}

/* Static methods for OpenSSL callbacks. */

inline static int onSslCertificateVerify(int /*preverifyOk*/, X509_STORE_CTX* /*ctx*/)
{
	 
	DEBUG_EX_LOG("OpenSSL callbacks");
	// Always valid since DTLS certificates are self-signed.
	return 1;
}

void  CreateSslCtx()
{
	 

	std::string dtlsSrtpCryptoSuites;
	int ret;

	/* Set the global DTLS context. */

	// Both DTLS 1.0 and 1.2 (requires OpenSSL >= 1.1.0).
	 sslCtx = SSL_CTX_new(DTLS_method());

	if (!sslCtx)
	{
		ERROR_EX_LOG("SSL_CTX_new() failed");

		goto error;;
	}

	ret = SSL_CTX_use_certificate(sslCtx, certificate);

	if (ret == 0)
	{
		ERROR_EX_LOG("SSL_CTX_use_certificate() failed");

		goto error;;
	}

	ret = SSL_CTX_use_PrivateKey(sslCtx, privateKey);

	if (ret == 0)
	{
		ERROR_EX_LOG("SSL_CTX_use_PrivateKey() failed");

		goto error;;
	}

	ret = SSL_CTX_check_private_key(sslCtx);

	if (ret == 0)
	{
		ERROR_EX_LOG("SSL_CTX_check_private_key() failed");

		goto error;;
	}

	// Set options.
	SSL_CTX_set_options(
		 sslCtx,
		SSL_OP_CIPHER_SERVER_PREFERENCE | SSL_OP_NO_TICKET | SSL_OP_SINGLE_ECDH_USE |
		SSL_OP_NO_QUERY_MTU);

	// Don't use sessions cache.
	SSL_CTX_set_session_cache_mode(sslCtx, SSL_SESS_CACHE_OFF);

	// Read always as much into the buffer as possible.
	// NOTE: This is the default for DTLS, but a bug in non latest OpenSSL
	// versions makes this call required.
	SSL_CTX_set_read_ahead(sslCtx, 1);

	SSL_CTX_set_verify_depth(sslCtx, 4);

	// Require certificate from peer.
	SSL_CTX_set_verify(
		sslCtx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, onSslCertificateVerify);

	// Set SSL info callback.
	SSL_CTX_set_info_callback(sslCtx, onSslInfo);

	// Set ciphers.
	ret = SSL_CTX_set_cipher_list(
		sslCtx, "DEFAULT:!NULL:!aNULL:!SHA256:!SHA384:!aECDH:!AESGCM+AES256:!aPSK");

	if (ret == 0)
	{
		ERROR_EX_LOG("SSL_CTX_set_cipher_list() failed");

		goto error;;
	}

	// Enable ECDH ciphers.
	// DOC: http://en.wikibooks.org/wiki/OpenSSL/Diffie-Hellman_parameters
	// NOTE: https://code.google.com/p/chromium/issues/detail?id=406458
	// NOTE: https://bugs.ruby-lang.org/issues/12324

	// For OpenSSL >= 1.0.2.
	SSL_CTX_set_ecdh_auto(DtlsTransport::sslCtx, 1);

	// Set the "use_srtp" DTLS extension.
	for (auto it =  srtpCryptoSuites.begin(); it != srtpCryptoSuites.end(); ++it)
	{
		if (it != srtpCryptoSuites.begin())
		{
			dtlsSrtpCryptoSuites += ":";
		}

		SrtpCryptoSuiteMapEntry* cryptoSuiteEntry = std::addressof(*it);
		dtlsSrtpCryptoSuites += cryptoSuiteEntry->name;
	}

	DEBUG_EX_LOG(  "setting SRTP cryptoSuites for DTLS: %s", dtlsSrtpCryptoSuites.c_str());

	// NOTE: This function returns 0 on success.
	ret = SSL_CTX_set_tlsext_use_srtp(sslCtx, dtlsSrtpCryptoSuites.c_str());

	if (ret != 0)
	{
		ERROR_EX_LOG(
			"SSL_CTX_set_tlsext_use_srtp() failed when entering '%s'", dtlsSrtpCryptoSuites.c_str());
		ERROR_EX_LOG("SSL_CTX_set_tlsext_use_srtp() failed");

		goto error;
	}

	return;

error:

	if ( sslCtx)
	{
		SSL_CTX_free( sslCtx);
		 sslCtx = nullptr;
	}

	ERROR_EX_LOG("SSL context creation failed");
}

static FingerprintAlgorithm GetFingerprintAlgorithm(const std::string& fingerprint)
{
	auto it = string2FingerprintAlgorithm.find(fingerprint);

	if (it != string2FingerprintAlgorithm.end())
	{
		return it->second;
	}
	//else
	//{
		return  FingerprintAlgorithm::NONE;
	//}
}
void GenerateFingerprints()
{
	 

	for (auto& kv :  string2FingerprintAlgorithm)
	{
		const std::string& algorithmString = kv.first;
		FingerprintAlgorithm algorithm = kv.second;
		uint8_t binaryFingerprint[EVP_MAX_MD_SIZE];
		unsigned int size{ 0 };
		char hexFingerprint[(EVP_MAX_MD_SIZE * 3) + 1];
		const EVP_MD* hashFunction = NULL;;
		int ret;

		switch (algorithm)
		{
		case FingerprintAlgorithm::SHA1:
			hashFunction = EVP_sha1();
			break;

		case FingerprintAlgorithm::SHA224:
			hashFunction = EVP_sha224();
			break;

		case FingerprintAlgorithm::SHA256:
			hashFunction = EVP_sha256();
			break;

		case FingerprintAlgorithm::SHA384:
			hashFunction = EVP_sha384();
			break;

		case FingerprintAlgorithm::SHA512:
			hashFunction = EVP_sha512();
			break;

		default:
			ERROR_EX_LOG("unknown algorithm");
		}

		ret = X509_digest(certificate, hashFunction, binaryFingerprint, &size);

		//(void *)hashFunction;
		if (ret == 0)
		{
			ERROR_EX_LOG("X509_digest() failed");
			ERROR_EX_LOG("Fingerprints generation failed");
		}

		// Convert to hexadecimal format in uppercase with colons.
		for (unsigned int i{ 0 }; i < size; ++i)
		{
			std::sprintf(hexFingerprint + (i * 3), "%.2X:", binaryFingerprint[i]);
		}
		hexFingerprint[(size * 3) - 1] = '\0';

		DEBUG_EX_LOG( "%-7s fingerprint: %s", algorithmString.c_str(), hexFingerprint);

		// Store it in the vector.
		Fingerprint fingerprint;

		fingerprint.algorithm = GetFingerprintAlgorithm(algorithmString);
		fingerprint.value = hexFingerprint;

		localFingerprints.push_back(fingerprint);
	}
}


inline static unsigned int onSslDtlsTimer(SSL* /*ssl*/, unsigned int timerUs)
{
	if (timerUs == 0)
	{
		return 100000;
	}
	else if (timerUs >= 4000000)
	{
		return 4000000;
	}
	//else
		return 2 * timerUs;
}

int main(int argc, char *argv[])
{
	std::call_once(globalInitOnce, [] {
		std::cout << "openssl version: " << OpenSSL_version(OPENSSL_VERSION);
		// Initialize some crypto stuff.
		RAND_poll();
	});
	
	/////////////
	// global ssl
	////////////
	ReadCertificateAndPrivateKeyFromFiles();


	// Create a global SSL_CTX.
	CreateSslCtx();

	// Generate certificate fingerprints.
	GenerateFingerprints();

	///////////////////////////////////

	SSL* ssl{ nullptr };

	BIO* sslBioFromNetwork{ nullptr }; // The BIO from which ssl reads.
	BIO* sslBioToNetwork{ nullptr };   // The BIO in which ssl writes.
	////////////////////

	/* Set SSL. */

	ssl = SSL_new(sslCtx);

	if (!ssl)
	{
		ERROR_EX_LOG("SSL_new() failed");

		return 0;
	}

	// Set this as custom data.
	//SSL_set_ex_data(ssl, 0, static_cast<void*>(this));


	sslBioFromNetwork = BIO_new(BIO_s_mem());

	if (! sslBioFromNetwork)
	{
		ERROR_EX_LOG("BIO_new() failed");

		SSL_free( ssl);

		return 0;
	}


	sslBioToNetwork = BIO_new(BIO_s_mem());

	if (! sslBioToNetwork)
	{
		ERROR_EX_LOG("BIO_new() failed");

		BIO_free( sslBioFromNetwork);
		SSL_free( ssl);

		return 0;
	}

	SSL_set_bio( ssl,  sslBioFromNetwork,  sslBioToNetwork);



	// Set the MTU so that we don't send packets that are too large with no fragmentation.
	SSL_set_mtu( ssl, DtlsMtu);
	DTLS_set_link_mtu( ssl, DtlsMtu);

	// Set callback handler for setting DTLS timer interval.

	//DTLS_set_timer_cb( ssl, onSslDtlsTimer);
	// NOTE: At this point SSL_set_bio() was not called so we must free BIOs as
		// well.
	///*if (this->sslBioFromNetwork)
	//	BIO_free(this->sslBioFromNetwork);

	//if (this->sslBioToNetwork)
	//	BIO_free(this->sslBioToNetwork);

	//if (this->ssl)
	//	SSL_free(this->ssl);*/


	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// server --> 
	SSL_set_accept_state(ssl);
	SSL_do_handshake(ssl);


	// <------------- client
	SSL_set_connect_state( ssl);
	SSL_do_handshake( ssl);
	if (BIO_eof(sslBioToNetwork))
	{
		return 0 ;
	}

	int64_t read;
	char* data{ nullptr };

	read = BIO_get_mem_data( sslBioToNetwork, &data); // NOLINT

	if (read <= 0)
	{
		return 0;
	}

	DEBUG_EX_LOG("%u bytes of DTLS data ready to sent to the peer", read);

	// Notify the listener. send server ===> 
	/*this->listener->OnDtlsTransportSendData(
		this, reinterpret_cast<uint8_t*>(data), static_cast<size_t>(read));*/

	// Clear the BIO buffer.
	// NOTE: the (void) avoids the -Wunused-value warning.
	(void)BIO_reset( sslBioToNetwork);
	////////////////////////



	return EXIT_SUCCESS;
}