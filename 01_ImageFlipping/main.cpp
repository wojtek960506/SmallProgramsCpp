#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cctype>
#include <climits>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
using namespace std;

const uint16_t ENDIAN_LITTLE = 0x4949;
const uint16_t ENDIAN_BIG    = 0x4d4d;

#endif /* __PROGTEST__ */

//CLASSES

#define HEADERSIZE 8 //expected number of bytes in header
#define BYTE_SIZE 8 //number of bits in a byte

enum ENDIANITY {
  little_endian = 0,
  big_endian = 1,
  wrong = 2
};

enum CHANNEL {
  BLACK_WHITE = 1, //1 channel per pixel
  RGB = 3, //3 channels per pixel
  RGBA = 4 //4 channels per pixel
};//

class FileData {
private:
  //char * fileData;
  unsigned long long imageSize; //in bytes
  ENDIANITY endianity;
  uint16_t width,
       height;
  CHANNEL channelsPerPixel;
  unsigned int bitsPerChannel;
  char* header;
  char* imageBytes;
  unsigned char padding; //1byte will be enough to store this data

  bool readEndianity();
  bool readWidth();
  bool readHeight();
  bool readPixelFormat();
  bool readHeader();
public:
  FileData() { header = new char[HEADERSIZE]; padding = 0; }
  FileData(FileData& obj); //copy constuctor
  ~FileData() { delete [] header; }

  //getters
  unsigned long long getImageSize() const { return imageSize; }
  ENDIANITY getEndianity() const { return endianity; }
  uint16_t getWidth() const { return width; }
  uint16_t getHeight() const { return height; }
  CHANNEL getChannelsPerPixel() const { return channelsPerPixel; }
  unsigned int getBitsPerChannel() const { return bitsPerChannel; }
  char* getHeader() const { return header; }
  char* getImageBytes() const { return imageBytes; }
  unsigned char getPadding() const { return padding; }

  //methods
  bool readImageData(const char * fileName);
  void printPixels();
  void printHeader();
  void printFileData();
  //friend ostream& operator << (ostream& out, FileData f); 
};


class Image {
private:
  unsigned long long imageSize;
  ENDIANITY endianity;
  uint16_t width,
       height;
  uint32_t widthB; //width in bytes: I have to use 32-byte representation because there might be situation where
           //wight will be almost maximum number from uint16_t and then for 16 bits per channel it will
           //extend the range and loose data;
  CHANNEL channelsPerPixel;
  unsigned int bitsPerChannel;
  char* header;
  char* imageBytes; 
  char ** pixelArray;
  unsigned char padding;
public:
  Image(FileData & fD);
  ~Image() {}
  char* getHeader() { return header; }
  char** getPixelArray() { return pixelArray; }
  uint16_t getWidth() { return width; }
  uint16_t getHeight() { return height; }
  uint32_t getWidthB() { return widthB; }
  unsigned char getPadding() { return padding; }
  void flipVertical();
  void flipHorizontal();
  void printPixelArray();
  bool checkPadding();
};

bool saveImage (const char * dstFileName, Image & image);

bool flipImage ( const char  * srcFileName,
                 const char  * dstFileName,
                 bool          flipHorizontal,
                 bool          flipVertical )
{
  FileData fileData;
  if (!fileData.readImageData(srcFileName)) {
    return false;//
  }

  Image image(fileData); //we will use this variable to make flips

  if (!image.checkPadding()) {
    return false;
  }

  if (flipHorizontal)
    image.flipHorizontal();

  if (flipVertical) 
    image.flipVertical();

  if (!saveImage(dstFileName,image))
    return false;

  return true;
}

#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1,
                      const char * fileName2 )
{
  FileData fileData1;
	if (!fileData1.readImageData(fileName1))
		return false;

	FileData fileData2;
	if (!fileData2.readImageData(fileName2))
		return false;


  //fileData1.printFileData();
  //fileData1.printPixels();
  //fileData2.printFileData();
  //fileData2.printPixels();

	if ((fileData1.getImageSize() != fileData2.getImageSize()) ||
		(fileData1.getEndianity() != fileData2.getEndianity()) ||
		(fileData1.getWidth() != fileData2.getWidth()) ||
		(fileData1.getHeight() != fileData2.getHeight()) ||
		(fileData1.getChannelsPerPixel() != fileData2.getChannelsPerPixel()) ||
		(fileData1.getBitsPerChannel() != fileData2.getBitsPerChannel()))
		return false;

	char* imageBytes1 = fileData1.getImageBytes();
	char* imageBytes2 = fileData2.getImageBytes();

	for (unsigned int i = 0 ; i < fileData1.getImageSize() ; i++) {
		if (imageBytes1[i] != imageBytes2[i])
			return false;
	}

	return true;
}

int main ( void )
{
  assert ( flipImage ( "./test_files/input_00.img", "./test_files/output_00.img", true, false )
           && identicalFiles ( "./test_files/output_00.img", "./test_files/ref_00.img" ) );
  assert ( flipImage ( "./test_files/input_01.img", "./test_files/output_01.img", false, true )
           && identicalFiles ( "./test_files/output_01.img", "./test_files/ref_01.img" ) );
  assert ( flipImage ( "./test_files/input_02.img", "./test_files/output_02.img", true, true )
           && identicalFiles ( "./test_files/output_02.img", "./test_files/ref_02.img" ) );
  assert ( flipImage ( "./test_files/input_03.img", "./test_files/output_03.img", false, false )
           && identicalFiles ( "./test_files/output_03.img", "./test_files/ref_03.img" ) );
  assert ( flipImage ( "./test_files/input_04.img", "./test_files/output_04.img", true, false )
           && identicalFiles ( "./test_files/output_04.img", "./test_files/ref_04.img" ) );
  assert ( flipImage ( "./test_files/input_05.img", "./test_files/output_05.img", true, true )
           && identicalFiles ( "./test_files/output_05.img", "./test_files/ref_05.img" ) );
  assert ( flipImage ( "./test_files/input_06.img", "./test_files/output_06.img", false, true )
           && identicalFiles ( "./test_files/output_06.img", "./test_files/ref_06.img" ) );
  assert ( flipImage ( "./test_files/input_07.img", "./test_files/output_07.img", true, false )
           && identicalFiles ( "./test_files/output_07.img", "./test_files/ref_07.img" ) );
  assert ( flipImage ( "./test_files/input_08.img", "./test_files/output_08.img", true, true )
           && identicalFiles ( "./test_files/output_08.img", "./test_files/ref_08.img" ) );
  assert ( ! flipImage ( "./test_files/input_09.img", "./test_files/output_09.img", true, false ) );

  // extra inputs (optional & bonus tests)
  assert ( flipImage ( "./test_files/extra_input_00.img", "./test_files/extra_out_00.img", true, false )
           && identicalFiles ( "./test_files/extra_out_00.img", "./test_files/extra_ref_00.img" ) );
  assert ( flipImage ( "./test_files/extra_input_01.img", "./test_files/extra_out_01.img", false, true )
           && identicalFiles ( "./test_files/extra_out_01.img", "./test_files/extra_ref_01.img" ) );
  assert ( flipImage ( "./test_files/extra_input_02.img", "./test_files/extra_out_02.img", true, false )
           && identicalFiles ( "./test_files/extra_out_02.img", "./test_files/extra_ref_02.img" ) );
  assert ( flipImage ( "./test_files/extra_input_03.img", "./test_files/extra_out_03.img", false, true )
           && identicalFiles ( "./test_files/extra_out_03.img", "./test_files/extra_ref_03.img" ) );
  assert ( flipImage ( "./test_files/extra_input_04.img", "./test_files/extra_out_04.img", true, false )
           && identicalFiles ( "./test_files/extra_out_04.img", "./test_files/extra_ref_04.img" ) );
  assert ( flipImage ( "./test_files/extra_input_05.img", "./test_files/extra_out_05.img", false, true )
           && identicalFiles ( "./test_files/extra_out_05.img", "./test_files/extra_ref_05.img" ) );
  assert ( flipImage ( "./test_files/extra_input_06.img", "./test_files/extra_out_06.img", true, false )
           && identicalFiles ( "./test_files/extra_out_06.img", "./test_files/extra_ref_06.img" ) );
  assert ( flipImage ( "./test_files/extra_input_07.img", "./test_files/extra_out_07.img", false, true )
           && identicalFiles ( "./test_files/extra_out_07.img", "./test_files/extra_ref_07.img" ) );
  assert ( flipImage ( "./test_files/extra_input_08.img", "./test_files/extra_out_08.img", true, false )
           && identicalFiles ( "./test_files/extra_out_08.img", "./test_files/extra_ref_08.img" ) );
  assert ( flipImage ( "./test_files/extra_input_09.img", "./test_files/extra_out_09.img", false, true )
           && identicalFiles ( "./test_files/extra_out_09.img", "./test_files/extra_ref_09.img" ) );
  assert ( flipImage ( "./test_files/extra_input_10.img", "./test_files/extra_out_10.img", true, false )
          && identicalFiles ( "./test_files/extra_out_10.img", "./test_files/extra_ref_10.img" ) );
  assert ( flipImage ( "./test_files/extra_input_11.img", "./test_files/extra_out_11.img", false, true )
           && identicalFiles ( "./test_files/extra_out_11.img", "./test_files/extra_ref_11.img" ) );

  cout << "ALL TESTS PASSED SUCCESSFULLY" << endl;

  return 0;
}
#endif /* __PROGTEST__ */

//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//

bool saveImage(const char * dstFileName, Image & image) {
  char * header = image.getHeader();
  char ** pixelArray = image.getPixelArray();
  uint16_t height = image.getHeight();
  uint32_t widthB = image.getWidthB();

  ofstream new_file; //ifstream is to read from the file
  new_file.open(dstFileName, ios::binary | ios::trunc);   //for ofstream by default it is ios:out ; ios::binary to write file in binary way
                                                          //ios::trunc - any contents that existed in the file before it is open and discarded

  //if the file does not exist or is not writable then it is not open
  if (new_file.is_open()) {
    new_file.write (header, HEADERSIZE);

    for (int i = 0 ; i < height ; i++) {
      new_file.write(pixelArray[i], widthB);

      if (!new_file.good()) {
        return false;
      }

    }

    new_file.close();
  } else 
    return false;


  return true;
}


Image::Image(FileData & fileData) {
    imageSize =     fileData.getImageSize();
    endianity =     fileData.getEndianity();
    width =       fileData.getWidth();
    height =      fileData.getHeight();
    channelsPerPixel =  fileData.getChannelsPerPixel();
    bitsPerChannel =  fileData.getBitsPerChannel();
    header =      new char [HEADERSIZE];
    header =      fileData.getHeader();
    imageBytes =    new char[imageSize];
    imageBytes =    fileData.getImageBytes();
    padding =     fileData.getPadding();

    //in widthB I store actual number of bytes in a row (it depends on bits per channel and channels per pixel)
      if (imageSize % height != 0)
        widthB = 0;
      widthB = imageSize / height;

      pixelArray = new char*[height];
      for (int i = 0 ; i < height ; i++)
        pixelArray[i] = new char[widthB];

      for (int i = 0 ; i < (int)height ; i++) {
        for (int j = 0 ; j < (int)widthB ; j++) {
          pixelArray[i][j] = imageBytes[(i*widthB+j)];
        }
      } 
  }

bool Image::checkPadding() {
  if (padding == 0)
    return true;

  for (int i = 0 ; i < height ; i++) {
    for (int j = 0 ; j < (int)padding ; j++) {
      //I check if bits which are padded are equal 0
      if (((pixelArray[i][widthB-1]>>(7-j))&0x1) != 0x0) {
        return false;
      }
    }
  }
  return true; //there was no return before so everything is fine
}

void Image::printPixelArray() {
  cout << "Image::PixelArray" << endl;
  for (int i = 0 ; i < (int)height ; i++) {
    for (int j = 0 ; j < (int)widthB ; j++) {
      cout << setw(4) << dec << (pixelArray[i][j] & 0xff) << ",";
    }
    cout <<endl;
  } 
}

//function used to flip image vertically (by horizontal axis) 
void Image::flipVertical() {
  char ** newPixelArray = new char*[height];
  //it is valid for all types of combinations "bits per channel and channels per pixel"
  for (int i = 0 ; i < height ; i++)
      newPixelArray[i] = pixelArray[height - 1 -i];
  
  //we update the pixel array of the file
  for (int i = 0 ; i < height ; i++)
      pixelArray[i] = NULL;
  delete [] pixelArray;
  pixelArray = new char*[height];
  for (int i = 0 ; i < height ; i++) {
    pixelArray[i] = new char[widthB];
    pixelArray[i] = newPixelArray[i];
  }
  
  //it is nice to delete the temporary array from memory
  for (int i = 0 ; i < height ; i++)
      newPixelArray[i] = NULL;
  delete [] newPixelArray;
}

void Image::flipHorizontal() {
  if (bitsPerChannel == 1) {
    
    unsigned int num_of_bits_with_data = widthB*BYTE_SIZE-padding; //it can be also represented as width * channelPerPixel

    char** newPixelArray = new char*[height];
    for (unsigned int i = 0 ; i < height ; i++) {
      newPixelArray[i] = new char[widthB];
      //inside this loop we have to handle really well decomposition and composition of bytes
      
      char * tmp = new char[num_of_bits_with_data]; //first I will flip bites and then compose them back to bytes
      for (unsigned int j = 0 ; j < widthB ; j++) {
        
        for (unsigned short int b = 0 ; b < BYTE_SIZE ; b++) {
          if (j == (widthB-1)) {
            if (padding != 0)
              if (b == (BYTE_SIZE-padding))
                break;
          }

          tmp[j*BYTE_SIZE+b] = ((pixelArray[i][j]>>b)&0x1);
        }
      }

      //now I have to go through array tmp and flip the order of bytes
      char* flip_tmp = new char[num_of_bits_with_data];

      //in width I have an information of number of pixels in a row
      for (unsigned int j = 0 ; j < width ; j++) {
        //we have to move pixels as a single element not to change an order of bits and channels inside a pixel
        for (unsigned short int pS = 0 ; pS < channelsPerPixel ; pS++) {
          flip_tmp[j*channelsPerPixel+pS] = tmp[(num_of_bits_with_data - ( (j+1) * channelsPerPixel ) + pS)];
        }
      }

      //now I have flipped bits in flip_tmp, so I should free the memory of tmp
      delete [] tmp;

      //loop to compose bytes
      for (unsigned int j = 0 ; j < widthB ; j++) {
        char single_byte = 0x0;
        for (unsigned short int b = 0 ; b < BYTE_SIZE ; b++) {
          if (j == (widthB-1)) {
            if (padding != 0)
              if (b == (BYTE_SIZE-padding))
                break;
          }
          single_byte = (single_byte) + ((flip_tmp[j*BYTE_SIZE+b]&0x1)<<b);
        }
        newPixelArray[i][j] = single_byte;
      }

      //we compose back the bytes so we don't need flip_tmp anymore
      delete [] flip_tmp;
    }

    //we update the pixel array of the file
    for (int i = 0 ; i < height ; i++)
      pixelArray[i] = NULL;
    delete [] pixelArray;
    pixelArray = new char*[height];
    for (int i = 0 ; i < height ; i++) {
      pixelArray[i] = new char[widthB];
      pixelArray[i] = newPixelArray[i];
    }

    //it is nice to delete the temporary array from memory
    for (int i = 0 ; i < height ; i++)
      newPixelArray[i] = NULL;
    delete [] newPixelArray;


  } else if ((bitsPerChannel == 8) || (bitsPerChannel == 16))  {
    //it is better to make this variable, because than maybe later I will connect it with 16 bytes
    unsigned int pixel_size = channelsPerPixel * bitsPerChannel / BYTE_SIZE ; //in bytes (there is 8 bits per channel so one byte) 

    char** newPixelArray = new char*[height];
    for (unsigned int i = 0 ; i < height ; i++) {
      newPixelArray[i] = new char[widthB];
      for (unsigned int j = 0 ; j < width ; j++) {
        //we have to move pixels as a single element not to change an order of bytes and channels inside a pixel
        for (unsigned int pS = 0 ; pS < pixel_size ; pS++) {
          newPixelArray[i][ (j*pixel_size) + pS ] = pixelArray[i][ (widthB - ( (j+1) * pixel_size ) + pS) ];
        }
      }
    }
    //we update the pixel array of the file
    for (int i = 0 ; i < height ; i++)
      pixelArray[i] = NULL;
    delete [] pixelArray;
    pixelArray = new char*[height];
    for (int i = 0 ; i < height ; i++) {
      pixelArray[i] = new char[widthB];
      pixelArray[i] = newPixelArray[i];
    }

    //it is nice to delete the temporary array from memory
    for (int i = 0 ; i < height ; i++)
      newPixelArray[i] = NULL;
    delete [] newPixelArray;
  } 

  //else {
  //  cout << "unrecognisable number of bits per channel\n";
  //}
}


FileData::FileData(FileData& obj) {
  this->imageSize =     obj.getImageSize();
  this->endianity =     obj.getEndianity();
  this->width =       obj.getWidth();
  this->height =      obj.getHeight();
  this->channelsPerPixel= obj.getChannelsPerPixel();
  this->bitsPerChannel =  obj.getBitsPerChannel();
  this->header =      new char [HEADERSIZE];
  this->header =      obj.getHeader();
  this->imageBytes =    new char[this->imageSize];
  this->imageBytes =    obj.getImageBytes();
  this->padding =     obj.getPadding();//
}


bool FileData::readImageData(const char * fileName) {
  ifstream image; //ifstream is to read from the file
  image.open(fileName, ios::binary); //for ifstream by default it is ios:in ; ios::binary to write file in binary way 

  //if the file does not exist or is not readable then it is not open
  if (image.is_open()) {
    image.read(header, HEADERSIZE); //we read header bytes
    if ( image.eof() ) { //an error while reading occured
      image.close();
      return false; //probably there was not enough bytes in a file to even read a header 
    }
    if (!readHeader()) {
      image.close();
      return false;
    }

    if (bitsPerChannel == 1) {
      unsigned char redundant_bits = (width*channelsPerPixel)%BYTE_SIZE;
      if (redundant_bits != 0)
        padding = BYTE_SIZE - redundant_bits; //number of bits with which we have to fill the last byte
    }
    //if not 1 bit per channel than two other possible numbers are divided by size of a byte (in bits)
    imageSize = (width * channelsPerPixel + padding) * height * bitsPerChannel / BYTE_SIZE; 

    imageBytes = new char[imageSize];
    image.read(imageBytes, imageSize);
    if ( image.eof() ) {
      image.close();
      return false;
    } 

    //to check if we have written the whole data
    char * a = new char[1];
    image.read(a, 1);

    if ( !image.eof() ) {
      image.close();
      return false;
    }

    image.close();
    return true;
  }
  else {
    return false; //it was not possible to open the file so we couldn't get the data
  }
}


//it is written for 16 and 8 bits
void FileData::printPixels() {
  
  cout << "-------------------------------------------------------------" << endl;
  cout << "-----------------------PRINTING PIXELS-----------------------" << endl;
  cout << "-------------------------------------------------------------" << endl;

  if (bitsPerChannel == 16) {
    for (unsigned long long i = 0 ; i < (unsigned int)imageSize/2 ;i++) {
      if (i%(width*channelsPerPixel) == 0)
        cout << endl;
      uint16_t byte1 = imageBytes[2*i] & 0x00ff;
      uint16_t byte2 = imageBytes[2*i+1] & 0x00ff;
      uint16_t byte = (byte2 << 8) + byte1;

      cout << setw(4) << hex << byte << ",";
    }
  }
  else if (bitsPerChannel == 8) {
    for (unsigned long long i = 0 ; i < (unsigned int)imageSize ;i++) {
      if (i%(width*channelsPerPixel) == 0)
        cout << endl;
      cout  << setw(2) << hex << (imageBytes[i] & 0xff) << ",";
    } 
  } else {
    cout << "printing for 1 bit per channel handled now" << endl;
    cout << "padding: " << (int)padding << endl;
    for (unsigned long long i = 0 ; i < (unsigned int)imageSize ;i++) {
      if ((i%((width*channelsPerPixel+padding)/8)) == 0)
        cout << endl;
      //for (int j = 0 ; j < 8 ; j++)
      //    cout << setw(1)<< ((imageBytes[i]>>(7-j))&0x1) << ",";
      cout  << setw(2) << hex << (imageBytes[i] & 0xff) << ",";
    }
  }

  cout << "\n\n-------------------------------------------------------------" << endl;
  cout << "-------------------------------------------------------------" << endl;
  cout << "-------------------------------------------------------------" << endl;
}

void FileData::printHeader() {
    cout << "-----------------------------------------" << endl;
    cout << "FILE DATA PRINT HEADER" << endl;
    for (int i = 0 ; i < 8 ; i++) {
      cout << (header[i]&0xff) << endl;
    }
    cout << "-----------------------------------------" << endl;
  }

void FileData::printFileData() {
  cout << "\n---------------------" << endl;
  cout << "------FILE DATA------" <<endl;
  cout << "---------------------" << endl;
  cout << "endianity: " << endianity << endl;
  cout << "width: " << width << endl;
  cout << "height: " << height << endl;
  cout << "channelsPerPixel: " << channelsPerPixel << endl;
  cout << "bitsPerChannel: " << bitsPerChannel << endl;
  cout << "imageSize: " << imageSize << endl;
  cout << "---------------------" << endl;
  cout << "------FILE DATA------" <<endl;
  cout << "---------------------\n" << endl;
}

//we proceed the first 8 fixed bytes of the file. In case of any failure function return false.
bool FileData::readHeader(){
  //header size must be equal 8 bytes
  if (header == NULL)
    return false;
  if ( (sizeof(header)/sizeof(header[0])) != HEADERSIZE )
    return false; 

  //we have to determine if it is little endian, big endian or failure
  if(!readEndianity()) {
    return false;
  }
  
  //since this moment we have to distinguish if we are using little_endian or big_endian

  //width and heigth have to be non-zero values
  if(!readWidth()) {
    return false;
  }
  if(!readHeight()) {
    return false;
  }
  if(!readPixelFormat()) {
    return false;
  }
  
  return true;
}

bool FileData::readEndianity() {
  uint16_t endian_tmp1 = header[0] & 0xff;
  uint16_t endian_tmp2 = header[1] & 0xff;
  
  if (endian_tmp1 != endian_tmp2)
    return false;

  switch (endian_tmp1) {
    case 0x49:
      endianity = ENDIANITY::little_endian;
      break;
    case 0x4d:
      endianity = ENDIANITY::big_endian;
      break;
    default:
      return false; //there is only big or little endian acceptable
  }
  return true;
}

//AT THIS TIME I ASSUME THERE IS ONLY LITTLE ENDING
bool FileData::readWidth() {
  uint16_t width_tmp1 = header[2] & 0xff;
  uint16_t width_tmp2 = header[3] & 0xff;
  

  if (endianity == ENDIANITY::little_endian) //little endian
    width = (width_tmp2 << 8) + width_tmp1;
  else //other option is only big endian
    width = (width_tmp1 << 8) + width_tmp2;

  if (width == 0)
    return false;
  return true;
}

//AT THIS TIME I ASSUME THERE IS ONLY LITTLE ENDING
bool FileData::readHeight() {
  uint16_t height_tmp1 = header[4] & 0xff;
  uint16_t height_tmp2 = header[5] & 0xff;

  if (endianity == ENDIANITY::little_endian) //little endian
    height = (height_tmp2 << 8) + height_tmp1;
  else //other option is only big endian
    height = (height_tmp1 << 8) + height_tmp2;
  
  if (height == 0)
    return false;
  return true;
}

bool FileData::readPixelFormat() {
  uint16_t pixel_format_tmp1 = header[6] & 0xff;
  uint16_t pixel_format_tmp2 = header[7] & 0xff;
  uint16_t pixel_format;

  if (endianity == ENDIANITY::little_endian) {
    //only first 5 bits of pixel format can have values other than 0
    if (pixel_format_tmp2 != 0)
      return false;
    if ((pixel_format_tmp1 >> 5) != 0)
      return false;
    pixel_format = (pixel_format_tmp2 << 8) + pixel_format_tmp1;
  } else { //only other option is big endian
    //only first 5 bits of pixel format can have values other than 0
    if (pixel_format_tmp1 != 0)
      return false;
    if ((pixel_format_tmp2 >> 5) != 0)
      return false;
    pixel_format = (pixel_format_tmp1 << 8) + pixel_format_tmp2;
  }

  //number of channels per pixel is hidden in first 2 bits of pixel_format
  uint16_t channel_type = pixel_format & 0x3; //((hex)0x3 === (bin)11)
  switch (channel_type) {
    case 0:
      channelsPerPixel = CHANNEL::BLACK_WHITE;
      break;
    case 2:
      channelsPerPixel = CHANNEL::RGB;
      break;
    case 3:
      channelsPerPixel = CHANNEL::RGBA;
      break;
    default:
      return false;
  }

  uint16_t channel_value = (pixel_format >> 2) & 0x7; //((hex)0x7 === (bin)111)
  switch (channel_value) {
    case 0:
      bitsPerChannel = 1;
      break;
    case 3:
      bitsPerChannel = 8;
      break;
    case 4:
      bitsPerChannel = 16;
      break;
    default:
      return false;
  }
  return true;
}
