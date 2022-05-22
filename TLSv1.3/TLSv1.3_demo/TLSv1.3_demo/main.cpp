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
#include "dtlsv1.x.h"
/* Static. */

static std::once_flag globalInitOnce;

int main(int argc, char *argv[])
{
	std::call_once(globalInitOnce, [] {
		DEBUG_EX_LOG( "openssl version: %s" ,  OpenSSL_version(OPENSSL_VERSION));
		// Initialize some crypto stuff.
		RAND_poll();
	});
	
	/////////////
	// global ssl
	////////////
	//chen::dtlsv1x::ReadCertificateAndPrivateKeyFromFiles();
	chen::dtlsv1x::GenerateCertificateAndPrivateKey();

	// Create a global SSL_CTX.
	chen::dtlsv1x::CreateSslCtx();

	// Generate certificate fingerprints.
	chen::dtlsv1x::GenerateFingerprints();

	 


	chen::dtlsv1x client("client");
	chen::dtlsv1x server("server");
	client.SetListener(&server);
	server.SetListener(&client);

	client.startup();
	server.startup();
	//server 准备handhasb
	server.Run(chen::Role::SERVER);

	client.Run(chen::Role::CLIENT);

	while (true)
	{
		DEBUG_EX_LOG("main sleep 1 seconds ...");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return EXIT_SUCCESS;
}