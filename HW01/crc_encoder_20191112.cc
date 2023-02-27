#include <iostream>
#include <fstream>
#include <stdlib.h>//exit()
#include <bitset>
#include <string>

using namespace std;

string XOR(string a, string b) {
	string c = "";
	for (int i = 0; i < a.length(); i++) {
		if (a[i] == b[i]) //if two bits are same
			c += '0';
		else //if two bits are different
			c += '1';
	}
	if (c[0] == '0') c = c.erase(0, 1);
	return c;
}

string modulo2(string dataword, string generator) {
	string original_dataword = dataword; //dataword before add '0'
	dataword += string(generator.size() - 1, '0');//add '0' to dataword
	string part = dataword.substr(0, generator.size()); //Get the size of the generator from the data and start the operation
	string tmp_divisor = "";
	for (int j = 0; j < generator.size(); j++) tmp_divisor += '0';
	for (int i = generator.size(); i <= dataword.size(); i++) {
		if (part[0] == '0') { //if Leftmost bit is 0, XOR with 0
			part = XOR(part, tmp_divisor);
			part += dataword[i]; //bring 1bit from divident
		}
		else { //if Leftmost bit is 1
			part = XOR(part, generator);
			part += dataword[i]; //bring 1bit from divident
		}
	}
	part = part.erase(part.length() - 1, 1);
	string tmp = original_dataword + part;
	return original_dataword + part;
}

int main(int argc, char *argv[]) {
	if (argc != 5) {
		cout << "usage: ./crc_encoder input_file output_file generator dataword_size";
		exit(1);
	}
	if (*argv[4] != '4' && *argv[4] != '8') {
		cout << "dataword size must be 4 or 8";
		exit(1);
	}
	string input_file = argv[1];//argument 1 : input_file
	string output_file = argv[2];//argument 2 : output_file
	string generator = argv[3];//argument 3 : generator
	int dataword_size = (int)*argv[4] - '0';//argument 4 : dataword_size

	char* buffer; //Buffers that store file contents
	int file_size; //file size
	
	ifstream fin; //read file
	fin.open(input_file.c_str(), ios::in | ios::binary);
	if (fin.fail()) {//Exception handling
		cerr << "input file open error." << endl;
		exit(1);
	}
	ofstream fout; //file write
	fout.open(output_file.c_str(), ios::out | ios::binary);
	if (fout.fail()) { //Exception handling
		cerr << "output file open error." << endl;
		exit(1);
	}

	fin.seekg(0, ios::end); //Move fin to the end of the file
	file_size = fin.tellg(); //Save the size of the file in file_size
	fin.seekg(0, ios::beg); //Move fin back to the beginning of the file

	buffer = new char[file_size + 1]; //dynamic allocation
	fill_n(buffer, file_size + 1, 0); //buffer initialization
	fin.read(buffer, file_size);

	string data = "";
	//Change the buffer content to binary and save it to data in string form
	for (int i = 0; i < file_size; i++) {
		bitset<8>byte(buffer[i]);
		data += byte.to_string();
	}

	int bitcount = 0; //number of bit
	int first = 0;
	string dataword = "";
	string codeword = "";
	string output_data = "";
	while (true) {
		dataword = data.substr(first, dataword_size);
		if (dataword.empty())
			break;
		codeword = modulo2(dataword, generator);
		output_data += codeword;
		bitcount += codeword.size();
		first += dataword_size;
	}

	string padding = ""; //padding bit
	padding = bitset<8>(8 - bitcount % 8).to_string(); //get padding bit
	output_data.insert(0, string(8 - bitcount % 8, '0')); //add '0' to make multiple of eight
	bitcount += 8 - bitcount % 8;
	output_data.insert(0, padding); //add padding bit to the very front
	bitcount += 8;


	unsigned long ul;
	unsigned char uc;
	for (int i = 0; i < bitcount; i++) { //write encoded data by 1 bit
		bitset<1>bit(output_data[i]);
		ul = bit.to_ulong();
		uc = static_cast<unsigned char>(ul);
		fout << uc;
	}
	
	fin.close(); 
	fout.close(); 
	delete[] buffer;
}
