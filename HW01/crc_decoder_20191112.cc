#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h> //exit()
#include <bitset>
#include <math.h>

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

int modulo2(string codeword, string generator) {
	string part = codeword.substr(0, generator.size()); //Get the size of the generator from the data and start the operation
	string tmp_divisor = "";
	for (int j = 0; j < generator.size(); j++) tmp_divisor += '0';
	for (int i = generator.size(); i <= codeword.size(); i++) {
		if (part[0] == '0') { //if Leftmost bit is 0, XOR with 0
			part = XOR(part, tmp_divisor);
			part += codeword[i]; //bring 1bit from divident
		}
		else { //if Leftmost bit is 1
			part = XOR(part, generator);
			part += codeword[i]; //bring 1bit from divident
		}
	}
	part = part.erase(part.length() - 1, 1);

	bitset<3> rem = bitset<3>(part);
	if (rem != 0) {
		return 1;//return if the remainder is not 0
	}
	else return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 6) {
		cout << "usage: ./crc_encoder input_file output_file generator dataword_size";
		exit(1);
	}
	if (*argv[5] != '4' && *argv[5] != '8') {
		cout << "dataword size must be 4 or 8";
		exit(1);
	}
	string input_file = argv[1]; //argument 1 : input_file
	string output_file = argv[2]; //argument 2 : output_file
	string result_file = argv[3]; //argument 3 : result_file
	string generator = argv[4]; //argument 4 : generator
	int dataword_size = (int)*argv[5]-'0'; //argument 5 : dataword_size

	char* buffer; //Buffers that store file contents
	int file_size; //file size

	ifstream fin; //read file
	fin.open(input_file.c_str(), ios::in | ios::binary);
	if (fin.fail()) { //Exception handling
		cerr << "input file open error." << endl;
		exit(1);
	}
	ofstream fout; //write file
	fout.open(output_file.c_str(), ios::out | ios::binary);
	if (fout.fail()) { //Exception handling
		cerr << "output file open error." << endl;
		exit(1);
	}

	ofstream fresult; //write result_file
	fresult.open(result_file.c_str(), ios::out | ios::binary);
	if (fresult.fail()) { //Exception handling
		cerr << "result file open error." << endl;
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
		bitset<1>byte(buffer[i]);
		data += byte.to_string();
	}

	int padding = 0;
	for (int i = 0; i < 8; i++) { //get padding bit
		if (data[i] == '1')
			padding += pow(2, 7 - i);
	}
	data.erase(0, 8 + padding); //delete data that notice number of padding bita, and padding bits

	int codeword_size = dataword_size + generator.length() - 1;
	int first = 0;
	string codeword = "";
	int codeword_num = 0;
	int error_num = 0; 
	string output_data = ""; //final output data
	while (true) {
		codeword = data.substr(first, codeword_size); //data divided by one codeword
		if (codeword.empty())
			break;
		codeword_num++;
		output_data += codeword.substr(0, dataword_size); //restored data
		if (modulo2(codeword, generator) == 1) //check error
			error_num++;
		first += codeword_size;
	}
	

	unsigned long ul;
	unsigned char uc;
	string char_data = "";
	int first2 = 0;
	while (true) { //write restored data in bytes to output_file
		char_data = output_data.substr(first2, 8);
		if (char_data.empty())
			break;
		bitset<8>byte(char_data);
		ul = byte.to_ulong();
		uc = static_cast<unsigned char>(ul);
		fout << uc;
		first2 += 8;
	}

	fresult << codeword_num << " " << error_num; //write number of codeword and number of error to result.txt
	fin.close(); 
	fout.close();
	delete[] buffer;
}
