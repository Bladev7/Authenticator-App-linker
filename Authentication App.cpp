#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <bitset>
#include <random>
#include <vector>
#include <ZXing/BitMatrix.h>
#include <ZXing/BitMatrixIO.h>
#include <ZXing/MultiFormatWriter.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image.h>



using namespace std;

string conv_toBase32(vector<unsigned char> key ) //Base32 Encoder 
{
	uint32_t buffer = 0;   // to collect bits
	int bitsLeft = 0;      // how many bits are in buffer
	string result = "";
	const char BASE32_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

	for (int i = 0; i < 16; i++) // loop byte by byte in each iteration
	{
		
		buffer = (buffer << 8) | key[i]; //shift the bits by 8 places to make a place for a new byte
		bitsLeft += 8; //1 byte counter
		while (bitsLeft >= 5)
		{
			int index = (buffer >> (bitsLeft-5) ) & 0x1F; //takes the 5 chunks of bits from left to right one by one and the 0x1F(00001111) takes last 5 bits
			result += BASE32_ALPHABET[index];
			bitsLeft -= 5; //decrease the bits counter by 5
		}

	
	}

	if (bitsLeft < 5) //checks after loop ends if there is bits left
		{
			int shiftAmount = 5 - bitsLeft; //to see how much bits left to make it 5 digits
			int index = (buffer << shiftAmount) & 0x1F; //we make shifting to make it 5 bits and 0x1f(00001111) takes last 5 bits
			result += BASE32_ALPHABET[index];

		}


	return result;
}

vector <unsigned char> GenerateRandom()
{
	
	vector<unsigned char> secret;

	random_device rd; //random device seed
	mt19937 gen ( rd() ); // generator for numbers
	uniform_int_distribution<int> randombyte(0, 255); //define the numbers we want (from , to)

	for (int i = 0; i < 16; i++)
	{
		secret.push_back(static_cast<unsigned char>(randombyte(gen) ) ); // converting integers we get from the generator of random numbers into unsigned char 

	}
	
	return secret;
}

string TOTP_URI(const string Account ,const string Key ,const string AppName)
{
	string URI = "otpauth://totp/" + Account + "?secret=" + Key + "&issuer=" + AppName;
	return URI;
}

void GenerateQRCode(const string & URI, const string & fileName, const int size )
{
	vector<unsigned char> pixels(size * size); //(height * width)

	ZXing::MultiFormatWriter writer(ZXing::BarcodeFormat::QRCode); //creating a class Member that generate QRCode
	ZXing::BitMatrix QRMatrix = writer.encode(URI,size,size); //encode the data into QR Code Matrix
	ZXing::SaveAsPBM(QRMatrix, fileName ,4); //function take the QR code Matrix and save it as file
}

int main()
{
	vector<unsigned char> secretKey = GenerateRandom();
	string Encoded_secret = conv_toBase32(secretKey);
	string URI = TOTP_URI("user@example.com" , "My2faApp", Encoded_secret);
	GenerateQRCode(URI ,"C:\\Users\\user\\Documents\\2fa_qrcode3.pbm",200);
	
	cout << "2-Authentication App" << endl;
	
	

	return 0;
}