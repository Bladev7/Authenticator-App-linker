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
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image.h>
#include <windows.h>


using namespace std;
using namespace ZXing;

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

string decode_Base32() //Base32 Decoder
{


	return "0";
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

bool GenerateQRCode(const string & URI, const string & fileName, const int size )
{

	vector<unsigned char> pixels(size * size); //(height * width)
	string pngName;
	string pngPath;


	MultiFormatWriter writer(BarcodeFormat::QRCode); //creating a class Member that generate QRCode
	BitMatrix QRMatrix = writer.encode(URI,size,size); //encode the data into QR Code Matrix
	SaveAsPBM(QRMatrix, fileName ,4); //function take the QR code Matrix and save it as file

	//converting pbm to png
	int height, width , channel; //variables to read the image and passed in the function
	unsigned char* img_check = stbi_load((fileName).c_str(), &width, &height, &channel, 1); //1 = grayscale in desired channel 
	
	//used c_str() function to convert string to const char for the function parameter
	
	
	if (img_check) 
	{
		pngName = fileName;
		pngName.erase(pngName.size() - 4); //deleting the .pbm in the filename when converting it to png

		// save as PNG
		if (!stbi_write_png((pngName + ".png").c_str(), width, height, channel, img_check, width * channel)) //calling function and excute while checking if it saved secessfully
		{
			cerr << "Failed to save PNG file" << endl;
			return false;
		}

		// free the image memory
		stbi_image_free(img_check);
		remove((fileName).c_str());
	}

	else if (!img_check)
	{
		cerr << "Failed to load pbm file " << endl; //using cerr to detect errors
		return false;
	}
	
	//user choice
	int user_choice;

	

	pngPath = pngName + ".png";


	cout << "QR Code :" << endl;
	cout << "1-open png file \n" <<"2-print in cmd"<<endl;
	cin >> user_choice;
	
	if (user_choice == 1)
	{
		if (GetFileAttributesA(pngPath.c_str()) == INVALID_FILE_ATTRIBUTES)
		{
			cerr << "PNG file not found" << endl;
			return false;
		}

		HINSTANCE result = ShellExecuteA(
			NULL,            // no parent window
			"open",          // operation
			pngPath.c_str(), // file to open and c_str() convert string to const char for Windows API to handle
			NULL,            // parameters
			NULL,            // default directory
			SW_SHOWNORMAL    // show normally
		);

		if ((int)result <= 32) // Microsoft error codes int>32 if int>32 success
		{
			cerr << "Failed to open PNG file" << endl;
			return false;
		}

	}

	else if (user_choice == 2)
	{
		int modules = QRMatrix.width();   // e.g., 33 or 41
		int targetWidth = 40;             // try between 25–40
		int scale = ceil(modules / (double)targetWidth);

		for (int y = 0; y < modules; y += scale)
		{
			for (int x = 0; x < modules; x += scale)
			{
				if (QRMatrix.get(x, y))
				{
					cout << u8"\u2588\u2588";  // black module
				}
				else
				{
					cout << "  ";   // white module
				}
			}
			cout << endl; // new line after each row
		}

	}

	else
	cout << "invalid input";

	return true;
}


int main()
{
	SetConsoleOutputCP(CP_UTF8);
	
	vector<unsigned char> secretKey = GenerateRandom();
	string Encoded_secret = conv_toBase32(secretKey);
	string URI = TOTP_URI("user@example.com" , "My2faApp", Encoded_secret);
	cout << "2-Authentication App" << endl;

	GenerateQRCode(URI ,"C:\\Users\\user\\Documents\\QRcode.pbm",200);

	system("pause");  
	return 0;
}