﻿/***********************************************************************************************
created: 		2022-05-22

author:			chensong

purpose:		TLSv1.3 协议的学习

原因是WebRTC中有DTSTransport -->
************************************************************************************************/
#ifndef _C_DTLSV1X_H_
#define _C_DTLSV1X_H_
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



namespace chen {

	enum class DtlsState
	{
		NEW = 1,
		CONNECTING,
		CONNECTED,
		FAILED,
		CLOSED
	};

	enum class Role
	{
		NONE = 0,
		AUTO = 1,
		CLIENT,
		SERVER
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


	class Listener
	{
	public:
		//virtual ~Listener() = default;

	public:
		// DTLS is in the process of negotiating a secure connection. Incoming
		// media can flow through.
		// NOTE: The caller MUST NOT call any method during this callback.
		virtual void ProcessDtlsData(const uint8_t* data, size_t len) = 0;
	};
	class dtlsv1x : public Listener
	{
	public:
		dtlsv1x(Listener * listener);

		//~dtlsv1x();
	public:

		static void ReadCertificateAndPrivateKeyFromFiles();


		// Create a global SSL_CTX.
		static void CreateSslCtx();

		// Generate certificate fingerprints.
		static void GenerateFingerprints();


	public:

		void Run(Role localRole);

		virtual void ProcessDtlsData(const uint8_t* data, size_t len);


		void Reset();

		bool CheckStatus(int returnCode);

		bool ProcessHandshake();
		bool CheckRemoteFingerprint();
	public:
		void show() const;

		/* Callbacks fired by OpenSSL events. */
	public:
		void OnSslInfo(int where, int ret);

		/* Pure virtual methods inherited from Timer::Listener. */
	private:
		bool IsRunning() const;
		

		void SendPendingOutgoingDtlsData();
	private:
		Listener * listener;
		SSL* ssl{ nullptr };

		BIO* sslBioFromNetwork{ nullptr }; // The BIO from which ssl reads.
		BIO* sslBioToNetwork{ nullptr };   // The BIO in which ssl writes.

		// // Others.
		DtlsState state{ DtlsState::NEW };
		Role localRole{ Role::NONE };
		Fingerprint remoteFingerprint;
		bool handshakeDone{ false };
		bool handshakeDoneNow{ false };
		std::string remoteCert;
	};
}

#endif // _C_DTLSV1X_H_