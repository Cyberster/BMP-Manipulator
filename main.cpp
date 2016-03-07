/**
  * @title BMP Manipulator and Text Segmentation Tool
  * Author: Arpan Das, DCST-III, Roll-D108055005, No-64626, Ph-91-9163609044
  * Project: Image manipulation
  * Image format: 24 bit BMP image
  */

#include <iostream>		// standard input output stream
#include <stdio.h>		// for printf()
#include <conio.h>		// for getch()
#include <stdlib.h>		// for system("cls") in main()
#include <math.h>		// for pow()
#include <string>		// for c++ string
#include <vector>		// for vectors
#include <fstream>		// for ofstream in exportHeader()
#include <stdexcept>	// vector:: for out of bound exception checking
#include "version.h"	// for auto version control plugin of codeblocks
#include <algorithm>	// for erase, remove, begin, end of hexColorReader()
#include <ctype.h>		// for is xdigit() on #include <ctype.h>

using namespace std;

/** Holds a RGB pixel data
 */
class RGB {
    public:
        unsigned char r, g, b; ///< to store red, green, blue values of pixels

		/** No parameter constructor
		 */
        RGB () {
			r = g = b = 255;
        }

		/** Three parameter constructor
		 *
		 * @param R an unsigned char argument.
		 * @param G an unsigned char argument.
		 * @param B an unsigned char argument.
		 */
        RGB (unsigned char R, unsigned char G, unsigned char B) {
            r = R;
            g = G;
            b = B;
        }

		/** Calculates the grey scale value from RGB value
		 *
		 * @returns a char
		 */
        unsigned char getGrey () {
            //return round(0.2989 * r + 0.5870 * g + 0.1140 * b);
            return round(0.2989 * r + 0.5871 * g + 0.1140 * b);
        }


		/** Overloaded assignment operator to copy values
		 *
		 * @param pixel an RGB argument.
		 */
        void operator = (RGB pixel) {
            r = pixel.r;
            g = pixel.g;
            b = pixel.b;
        } // RGB pixel = vector<RGB>(value)
};

/** Holds a BMP image and manipulates
 */
class BMP {
    private:
        string filename;					///< file name
        string bfType = "";					///< byte 0-1 - BM
        unsigned long bfSize = 0; 			///< byte 2-5 - file size in bytes
        unsigned short bfReserved1 = 0;		///< byte 6-7 - reserved 1
        unsigned short bfReserved2 = 0;		///< byte 8-9 - reserved 2
        unsigned long bfOffBits = 0;		///< byte 10-13 - header size (54)/ starting of data

        unsigned long biSize = 0; 			///< byte 14-17 - header information size in bytes
        unsigned long biWidth = 0;			///< byte 18-21 - width
        unsigned long biHeight = 0;			///< byte 22-25 - height
        unsigned short biPlanes = 0;		///< byte 26-27 - no of planes
        unsigned short biBitCount = 0;		///< byte 28-29 - image bit depth (24)
        unsigned long biCompression = 0;	///< byte 30-33 - compression
        unsigned long biSizeImage = 0;		///< byte 34-37 - size of image without padding
        unsigned long biXPelsPerMeter = 0;	///< byte 38-41 - pixels per meter in X axis
        unsigned long biYPelsPerMeter = 0;	///< byte 42-45 - pixels per meter in Y axis
        unsigned long biClrUsed = 0;		///< byte 46-49 - number of colors used
        unsigned long biClrImportant = 0;	///< byte 50-53 - number of significant colors

        short padding = 0;					///< padding or junk bytes after each row
        vector< vector <RGB> > pixelMatrix;	///< 2D matrix of RGB type to store pixel data

    public:
        // constructors ==========================================
		/** Do nothing constructor
		 */
        BMP () {}

		/** One parameter constructor
		 *
		 * @param fn an string argument.
		 */
        BMP (string fn) {
            filename = fn;
            this->readHeader();
            pixelMatrix = this->getBMPmatrix();
        }

        // main methods ==========================================

		/** Reads 54 byte bmp header
		 */
        void readHeader () {
            unsigned short byte;
            unsigned long i = 0; // counts byte read
            FILE *fp;
            fp = fopen(filename.c_str(), "rb");

            while (!feof(fp)) {
                i++; // i-th byte (1st byte = 1)
                byte = getc(fp);

                //printf("%d ", byte);

                /**** reading Header File : 14 bytes ****/
                if (i >= 1 && i <=2) {          /** bfType - 2 bytes **/
                    bfType += (char)byte;
                } else if (i >= 3 && i <=6) {   /** bfSize: - 4 bytes **/
                    if (bfType.compare("BM") != 0) {
                        cout << "Error: this is not a valid 24 bit BMP file.";
                        return; // if not bmp, exit
                    }
                    bfSize += byte  * pow(2, ((i-3)*8));
                } else if (i >= 7 && i <=8) {   /** bfReserved1: - 2 bytes **/
                    bfReserved1 += byte  * pow(2, ((i-7)*8));
                } else if (i >= 9 && i <=10) {  /** bfReserved2: - 2 bytes **/
                    bfReserved2 += byte  * pow(2, ((i-9)*8));
                } else if (i >= 11 && i <=14) { /** bfOffBits: - 4 bytes **/
                    bfOffBits += byte  * pow(2, ((i-11)*8));
                }

                /**** reading Header Information : 40 bytes ****/
                if (i >= 15 && i <= 18) { /** biSize: - 4 bytes **/
                    biSize += byte  * pow(2, ((i-15)*8));
                } else if (i >= 19 && i <= 22) { /** biWidth: - 4 bytes **/
                    biWidth += byte  * pow(2, ((i-19)*8));
                } else if (i >= 23 && i <= 26) { /** biHeight: - 4 bytes **/
                    biHeight += byte  * pow(2, ((i-23)*8));
                } else if (i >= 27 && i <= 28) { /** biPlanes: - 2 bytes **/
                    biPlanes += byte  * pow(2, ((i-27)*8));
                } else if (i >= 29 && i <= 30) { /** biBitCount: - 2 bytes **/
                    biBitCount += byte  * pow(2, ((i-29)*8));
                } else if (i >= 31 && i <= 34) { /** biCompression: - 4 bytes **/
                    biCompression += byte  * pow(2, ((i-31)*8));
                } else if (i >= 35 && i <= 38) { /** biSizeImage: - 4 bytes **/
                    biSizeImage += byte  * pow(2, ((i-35)*8));
                } else if (i >= 39 && i <= 42) { /** biXPelsPerMeter: - 4 bytes **/
                    biXPelsPerMeter += byte  * pow(2, ((i-39)*8));
                } else if (i >= 43 && i <= 46) { /** biYPelsPerMeter: - 4 bytes **/
                    biYPelsPerMeter += byte  * pow(2, ((i-43)*8));
                } else if (i >= 47 && i <= 50) { /** biClrUsed: - 4 bytes **/
                    biClrUsed += byte  * pow(2, ((i-47)*8));
                } else if (i >= 51 && i <= 54) { /** biClrImportant: - 4 bytes **/
                    biClrImportant += byte  * pow(2, ((i-51)*8));
                }

                /** calculating padding value **/
                padding = getPadding(biWidth);
            }
            fclose(fp);
        } // readHeader end
		/** Prints 54 byte bmp header to screen
		 */
        void printHeader () {
            cout << "Filename: " << filename << "\n==========================\n";
            cout << "bfType: " << bfType << endl;
            cout << "bfSize: " << bfSize << " bytes" << endl;
            cout << "bfReserved1: " << bfReserved1 << endl;
            cout << "bfReserved2: " << bfReserved2 << endl;
            cout << "bfOffBits: " << bfOffBits << endl;
            cout << "biSize: " << biSize << " bytes" << endl;
            cout << "biWidth: " << biWidth << " pixels" << endl;
            cout << "biHeight: " << biHeight << " pixels" << endl;
            cout << "biPlanes: " << biPlanes << endl;
            cout << "biBitCount: " << biBitCount << " bit" << endl;
            cout << "biCompression: " << biCompression << endl;
            cout << "biSizeImage: " << biSizeImage << " pixels" << endl; // somtimes its 0
            cout << "biXPelsPerMeter: " << biXPelsPerMeter << " pixels" << endl;
            cout << "biYPelsPerMeter: " << biYPelsPerMeter << " pixels" << endl;
            cout << "biClrUsed: " << biClrUsed << endl;
            cout << "biClrImportant: " << biClrImportant << endl;
            cout << "junk bytes per row: " << padding << endl;
        } // printHeader end
		/** Prints 54 byte bmp header to screen and file
		 */
        void exportHeader () {
            printHeader();
            string fn = filename.substr(0, filename.size()-4) + ".header";

            streambuf *coutbuf = cout.rdbuf(); //save old buf
            ofstream out(fn.c_str());
            cout.rdbuf(out.rdbuf()); //redirect cout to filename.header
            printHeader();
            cout.rdbuf(coutbuf); //reset to standard output again
        } // exportHeader end

		/** Rotates a bmp image by 90, 180 and 270 degrees clockwise
		 *
		 * @param degree an int argument.
		 * @param fn an string argument, default = blank.
		 * @returns a vector< vector<RGB> >
		 */
        vector< vector<RGB> > rotateBMP (int degree, string fn = string()) {
        	vector< vector< RGB > > data2D;

        	if (fn.empty() == true) {
				fn = filename;
				data2D = pixelMatrix;
        	} else {
				data2D = getBMPmatrix(fn);
        	}

            unsigned int height = data2D.size();
            unsigned int width = data2D[0].size();

            switch (degree) {
                case 90:
                    rotateMatrix(data2D, 90);
                    break;
                case 180:
                    rotateMatrix(data2D, 180);
                    break;
                case 270:
                    rotateMatrix(data2D, 270);
                    break;
                default:
                    return data2D; // return unrotated matrix
            }

			// rotate back pixelMatrix
			pixelMatrix = data2D;

            /** writing to new BMP **/
            writeAsBMP(fn, data2D);
            //exportHexColor(fn);

            return data2D;
        } // rotateBMP end

		/** Rotates a 2d matrix/array by 90, 180 and 270 degrees clockwise
		 *
		 * @param data2D an vector< vector< RGB > > reference argument.
		 * @param degree an int argument.
		 */
        void rotateMatrix (vector< vector< RGB > >& data2D, int degree) {
        	unsigned int height = data2D.size();
        	unsigned int width = data2D[0].size();
            vector< vector< RGB > > data2DR;

            switch (degree) {
                case 90: {
                    //height = (height+width)-(width=height); // swap height-width
                    swap(height, width);
                    data2DR.resize(height);
                    for (int i=0; i<height; i++) data2DR[i].resize(width);

                    // rotating the 2d matrix by 90 degrees
                    for (int i=0; i<height; i++) {
                        for (int j=0, k=width-1; j<width; j++, k--) {
                            data2DR[i][j] = data2D[k][i];
                        }
                    }
                    break;
                }
                case 180: {
                    data2DR.resize(height);
                    for (int i=0; i<height; i++) data2DR[i].resize(width);

                    // rotating the 2d matrix by 180 degrees
                    for (int i=0, l=height-1; i<height; i++, l--) {
                        for (int j=0, k=width-1; j<width; j++, k--) {
                            data2DR[i][j] = data2D[l][k];
                        }
                    }
                    break;
                }
                case 270: {
                    //height = (height+width)-(width=height); // swap height-width
                    swap(height, width);
                    data2DR.resize(height);
                    for (int i=0; i<height; i++) data2DR[i].resize(width);

                    // rotating the 2d matrix by 270 degrees
                    for (int i=0, l=height-1; i<height; i++, l--) {
                        for (int j=0, k=0; j<width; j++, k++) {
                            data2DR[i][j] = data2D[k][l];
                        }
                    }
                    break;
                }
            }

            data2D = data2DR;
        } // rotateMatrix end

		/** Exports hex colors to text file
		 *
		 * When used alone, do not mention last two arguments
		 * bmp2txt() is overloaded version of this method.
		 * Used methods: getHeightWidthByFN(), getPadding(), getBMPmatrix().
		 * @param inFileName an string argument, default is blank.
		 * @param binarize an int argument, default is false.
		 * @param showOnScreen an int argument, default is false.
		 */
        void exportHexColor (string inFileName = string(), bool binarize = false, bool showOnScreen = false) {
        	if (inFileName.empty() == true)	inFileName = filename;

            unsigned int * heightWidth = getHeightWidthByFN(inFileName);
            unsigned int height = heightWidth[0];
            unsigned int width = heightWidth[1];
            unsigned int newPadding = getPadding(width);

            string outFileName = inFileName.substr(0, inFileName.size()-3);
            if (binarize == true) outFileName = outFileName.append("txt"); // may use dat
            else outFileName = outFileName.append("hex"); // may use dat

            FILE *fpw = fopen(outFileName.c_str(), "wb"); // for writing txt

            vector< vector< RGB > > data2D = getBMPmatrix(inFileName);

            /** sorting left to right, bottom to top **/
            for (int i=0; i<height; i++) {
                for (int j=0; j<width; j++) {
					if (showOnScreen == true) printf("%s", data2D[i][j].getGrey() < 220 ? "#" : " ");
					if (binarize == true) fprintf(fpw, "%s", data2D[i][j].getGrey() < 220 ? "#" : " ");
					else fprintf(fpw, "%02x%02x%02x ", data2D[i][j].r, data2D[i][j].g, data2D[i][j].b);
                }
                fprintf(fpw, "\r\n");
                if (showOnScreen == true) printf("\n"); // comment out for no scrolling animation
            }
            fclose(fpw);
        } // exportHexColor end

		/** binarize a bmp image
		 *
		 * It is overloaded version of this method.
		 * Used methods: exportHexColor().
		 * @param fn an string argument, default is blank.
		 * @param showOnScreen an int argument, default is false.
		 */
        void bmp2txt (string fn = string(), bool showOnScreen = false) {
			exportHexColor(fn, true, showOnScreen);
        } // bmp2txt end

		/** reads hex color files and store them into 2D array
		 *
		 * reads color intensity .txt files and store them into vector
		 * Used methods: getHeightWidthByFN(), exportHexColor().
		 * @param fn an string argument.
		 * @returns a vector< vector<RGB> >
		 */
        vector< vector<RGB> > hexColorReader (string fn) {
        	FILE * fpr = fopen(fn.c_str(), "rb");
            //unsigned int height, width;
            vector< vector<RGB> > data2D;
            vector<RGB>data1D;

			// loading the file into array
			fseek(fpr, 0, SEEK_END);
			long fsize = ftell(fpr);
			fseek(fpr, 0, SEEK_SET);
			//char * data = new char[fsize];
			vector<char> data(fsize);
            //fread(data, fsize, 1, fpr);
            fread(&data[0], 1, data.size(), fpr);
			fclose(fpr);

			int height = 0, width = 0;
			for (int i=0; i<data.size(); i++) if (data[i] == '\n') height++;
			// erasing white space,\r and \n from the vector
			data.erase(remove(data.begin(), data.end(), ' '), data.end());
			data.erase(remove(data.begin(), data.end(), '\r'), data.end());
			data.erase(remove(data.begin(), data.end(), '\n'), data.end());
			width = data.size()/height/6;

            data2D.resize(height);
            for (int i=0; i<height; i++) data2D[i].resize(width);

            for (int i=0; i<data.size(); i+=6) {
				// pack 6 hex character into a string, then convert it to int number
				//http://www.cplusplus.com/reference/string/string/append/
				RGB pixel;
				string temp = "";
				int num;
				for (int j=0; j<6; j++) temp += data[i+j];
				if (isxdigit(temp[0])) num = strtol(temp.c_str(), NULL, 16);
												// ffeedd
				pixel.r = num >> 16; 			// 0000ff
				pixel.g = (num >> 8) & 0xFF; 	// 00ffdd => 00ee00
				pixel.b = num & 0xFF; 			// 0000dd

				// int m = (i+6)/(width*6);
				// int n = (i%(width*6))/6;
				//printf("[%02d][%02d] ", m, n); // bug: dont know why not working
				//if ((i+6)%(width*6) == 0) cout << endl;
				// data2D[m][n] = pixel;
				data1D.push_back(pixel);
            }

            // storing data from 1d to 2d vector
            for (int i=0; i<height; i++) {
                for (int j=0; j<width; j++) {
                    data2D[i][j] = data1D[i*width+j];
                }
            }

            return data2D;
        } // hexColorReader end

		/** Reads hex color files and write them into BMP files
		 *
		 * reads color intensity .txt files and store them into vector
		 * those .txt file has white space as pixel and '\n' as line delimiter
		 * Used methods: hexColorReader(), writeAsBMP().
		 * @param inFileName an string argument.
		 * @param outFileName an string argument.
		 */
        void hex2bmp (string inFileName, string outFileName) {
        	vector< vector<RGB> > data2D = hexColorReader(inFileName);
        	writeAsBMP(outFileName, data2D);
        } // hex2bmp end

		/** Reads color from BMP files and store them into 2D array
		 *
		 * Used methods: getHeightWidthByFN(), getPadding().
		 * @param fn an string argument, default is blank.
		 * @returns a vector< vector<RGB> >
		 */
        vector< vector< RGB > > getBMPmatrix (string fn = string()) {
        	if (fn.empty() == true)	fn = filename;

            unsigned int * heightWidth = getHeightWidthByFN(fn);
            unsigned int height = heightWidth[0];
            unsigned int width = heightWidth[1];
            unsigned int newPadding = getPadding(width);

            FILE * fpr = fopen(fn.c_str(), "rb");  // for reading bmp
            fseek(fpr, 54, SEEK_SET); // skip first 54 bytes

            vector< vector< RGB > > data2D;
            // preparing the 2d vector size
            data2D.resize(height);
            for (int i=0; i<height; i++) data2D[i].resize(width);
			/** fetching and sorting left to right, bottom to top **/
            for (int i=height-1; i>=0; i--) {
                for (int j=0; j<width; j++) {
                    RGB pixel;
                    pixel.b = getc(fpr);
                    pixel.g = getc(fpr);
                    pixel.r = getc(fpr);

                    data2D[i][j] = pixel;
                }
                //fseek(fpr, newPadding, SEEK_CUR);
                for (int j=0; j<newPadding; j++) getc(fpr);
                //fread(new int[newPadding], newPadding, 1, fpr);
            }
            fclose(fpr);
			return data2D;
        } // getBMPmatrix end

		/** Generates both line and word segment from BMP files
		 *
		 * This method calls its sub method segmentGenerator() and
		 * loop through received string array up to two level
		 * Used methods: segmentGenerator().
		 * @param threshold an int argument, default is 225.
		 */
        void generateSegments (int threshold = 220) {
            cout << "\n================ Generating Segments ================" << endl;
            vector<string> lineNames = segmentGenerator(true, threshold);

            // iterating through each line
            for (int i=0; i<lineNames.size(); i++) {
                cout << "Processing: " << lineNames[i] << endl;
                vector<string> wordNames = segmentGenerator(false, threshold, lineNames[i]);

                for (int j=0; j<wordNames.size(); j++) {
                    cout << "Processing: " << wordNames[j] << endl;
                    //exportHexColor(wordNames[j]);
                    //bmp2txt(wordNames[j], true);
                }
            }
        } // generateSegments end

		/** Generates only line or word segment from BMP files depends on condition
		 *
		 * If fetchHorizontal is true then it fetch BMP image left to right else top to bottom
		 * i.e. fetchHorizontal = true means line segment else word segment.
		 * Used methods: writeAsBMP().
		 * @param fetchHorizontal an bool argument.
		 * @param threshold an int argument.
		 * @param fn an string argument, default is blank.
		 * @returns a vector<string>
		 */
        vector<string> segmentGenerator(bool fetchHorizontal, int threshold, string fn = string()) {
        	vector< vector< RGB > > data2D, data2DB;

			if (fn.empty() == true) {
				fn = filename;
				data2D = pixelMatrix;
			} else {
				data2D = getBMPmatrix(fn);
			}

            vector<string> fileSegmentNames;
			unsigned int height = data2D.size();
            unsigned int width = data2D[0].size();

            if (fetchHorizontal == false) {
				rotateMatrix(data2D, 90);
				height = (height+width) - (width=height);
            }

			// backup: later read position from data2D, read data from data2DB
			data2DB = data2D;

			// increase contrast
			increaseContrast(data2D, 99, threshold);
			//data2D = binarizeBMP(data2D, 50);

			// removing noise by median blur
			int kernelSize = fetchHorizontal == true ? 7 : 5;
			data2D = medianBlur(data2D, false, kernelSize);

			// generate histogram
			vector<unsigned long int> histogram = generateHistogram(data2D);
			int hMax = *max_element(histogram.begin(), histogram.end());
			int hMin = *min_element(histogram.begin(), histogram.end());

			// calculate permitted gap for histogram
			int permittedGap = hMin;
			if (fetchHorizontal == true) permittedGap += width*0.005;
			//else permittedGap += width*0.002;
			//cout << "hMax: " << hMax << "hMin: " << hMin << endl;

            int segCount = 0, iCount = 0; // segment count, row count just count rows
            while (true) { // pending: gojamil
                bool isWhite = true;
                for (int k=iCount; k<height; k++) {
					if (histogram[k] > permittedGap) isWhite = false;
					//cout << k << ": (" << histogram[k] << " > " << hMin << "), isWhite: " << isWhite << endl;
                }
                //cout << "\n\nultimately isWhite: " << isWhite << endl;
                if (isWhite == true) break; // main while breaker

				// preparing the new segment's file name
                char segmentNo[4];
                sprintf(segmentNo, "%d", ++segCount);
                string outFileName;
                //outFileName = (fetchHorizontal == true) ? "./lines/" : "./words/";
                outFileName += fn.substr(0, fn.size()-4);
                outFileName += (fetchHorizontal == true) ? "_line" : "_word";
                outFileName += segmentNo;
                outFileName += ".bmp";
                fileSegmentNames.push_back(outFileName);

                vector< vector<RGB> > line2D; // 2d vector to store each segment/line

				bool isReading = false;
				//int darkCount;

                for (int i=iCount; i<height; i++) {
					//if (histogram[i] <= permittedGap && isReading == false && darkCount <= 20) {
					if (histogram[i] <= permittedGap && isReading == false) {
						//darkCount = 0;
						continue; // skip blank rows
					}
					isReading = true;
					//cout << i << ": " << histogram[i] << endl;

					// start reading
					vector<RGB> newRow; // to be added in line2D
					for (int j=0; j<width; j++) {
						RGB pixel = data2DB[i][j];
						newRow.push_back(pixel);
						//printf("%02x%02x%02x ", pixel.r, pixel.g, pixel.b);
						//printf("%s", pixel.getGrey() < threshold ? "#" : " ");
					}
					//cout << endl;
					line2D.push_back(newRow);
					//cout << endl;

					if (histogram[i] <= permittedGap) { // line end checker
						isReading = false;
						iCount = i;
						break;
					}
                } // end outer for

                //cout << "line2d height: " << line2D.size() << ", line2d width: " << line2D[0].size() << endl;

				if (fetchHorizontal == false) rotateMatrix(line2D, 270); // rotate back words

                /** writing to new BMP **/
                writeAsBMP(outFileName, line2D);

                //if (fetchHorizontal == false) rotateBMP(270, outFileName); // rotate back words
            } // end while

            return fileSegmentNames;
        } // segmentGenerator end

		/** Generate histogram of a BMP image (P5 PGM)
		 *
		 * @param data2D an vector< vector<RGB> > argument.
		 * @param fn an string argument, default is blank.
		 * @returns a vector<unsigned long int>
		 */
		vector<unsigned long int> generateHistogram (vector< vector<RGB> > data2D, string fn = string()) {
			int height = data2D.size();
			int width = data2D[0].size();
			vector<unsigned long int> histogram;
			//vector< vector<int> > greyMatrix;
			//greyMatrix.resize(height);
			//for (int i=0; i<height; i++) greyMatrix[i].resize(width);

            for (int i=0; i<height; i++) {
				unsigned long int lineValue = 0;
				for (int j=0; j<width; j++) {
					lineValue +=  data2D[i][j].getGrey();
					//printf("%s", data2D[i][j].getGrey() > 127 ? "#" : " ");
					//if (fn.empty() == false) greyMatrix[i][j] = data2D[i][j].getGrey();
				}
				//cout << endl;
				lineValue = width-lineValue/255;
				//cout << i << ": " << lineValue << endl;
                histogram.push_back(lineValue);
            }

            //cout << "max: " << *max_element(histogram.begin(), histogram.end()) << endl;
            //cout << "min: " << *min_element(histogram.begin(), histogram.end()) << endl;

            // now generate the p5-pgm file from here
//			P5
//			#Created by IrfanView
//			2029 491
//			255
			if (fn.empty() == false) { // when file name provided
				string pgmName = fn.substr(0, fn.size()-4) + ".pgm";
				FILE * fpw = fopen(pgmName.c_str(), "wb");
				fprintf(fpw, "P5\n#%s\n%d %d\n255\n", "Created by Arpan", width, height);

				for (int i=0; i<height; i++) {
//					for (int j=0; j<width; j++) {
//                        //fprintf(fpw, "%c", data2D[i][j].getGrey());
//                        fputc(data2D[i][j].getGrey(), fpw);
//					}
					// generating the horizontal bar graph, x=intensity, y=width
					for (int j=0; j<histogram[i]; j++) fprintf(fpw, "%c", 0); //fputc('1', fpw);
					for (int j=0; j<width-histogram[i]; j++) fprintf(fpw, "%c", 255); //fputc('0', fpw);
				}

                fclose(fpw);
			}


            return histogram;
		} // generateHistogram end

		/** Increase contrast of a RGB pixel matrix
		 *
		 * @param data2D an vector< vector<RGB> >& argument.
		 * @param factor an int argument.
		 * @param threshold an int argument.
		 */
        void increaseContrast (vector< vector<RGB> >& data2D, int factor, int threshold) {
			unsigned int height = data2D.size();
			unsigned int width = data2D[0].size();

			for (int i=0; i<height; i++) {
				for (int j=0; j<width; j++) {
					RGB& pixel = data2D[i][j];
                    if (pixel.getGrey() < threshold) {
                        pixel.r = pixel.r-(pixel.r*factor/100) < 0 ? 0 : pixel.r-(pixel.r*factor/100);
                        pixel.g = pixel.g-(pixel.g*factor/100) < 0 ? 0 : pixel.g-(pixel.g*factor/100);
                        //pixel.b = pixel.b-(pixel.b*factor/100) < 0 ? 0 : pixel.b-(pixel.b*factor/100);
                    }
				}
			}
        } // increaseContrast end

		/** Apply median blur on a pixel matrix, it removes salt/paper noise
		 *
		 * @param data2D an vector< vector<RGB> > argument, it should be odd.
		 * @param isColor an int bool.
		 * @param kernelSize an int argument, default is 7.
		 * @returns a vector< vector<RGB> >
		 */
		vector< vector<RGB> > medianBlur (vector< vector<RGB> > data2D, bool isColor, int kernelSize = 7) {
			int height = data2D.size();
			int width = data2D[0].size();

//			if (height < kernelSize && width < kernelSize) return data2D;
//			if (kernelSize == 0) { // auto generate kernel size
//				kernelSize = (width*height*0.002975)/100;
//				if (kernelSize % 2 == 0) kernelSize++;
//				if (kernelSize == 1) kernelSize += 2;
//			}
			//cout << "kernelSize: " << kernelSize << endl;

			vector< vector<RGB> > data2DR;
			data2DR.resize(height);
			for (int i=0; i<height; i++) {
				data2DR[i].resize(width);
				for (int j=0; j<width; j++) data2DR[i][j] = RGB();
			}

			// calculating scanning range
			int hStart = kernelSize/2;
			int hEnd = height - kernelSize/2 + 1;
			int wStart = kernelSize/2;
			int wEnd = width - kernelSize/2 + 1;

			//cout << "\nhStart: " << hStart << ", hEnd: " << hEnd;
			//cout << "\nwStart: " << wStart << ", wEnd: " << wEnd << endl;

            for (int i=hStart; i<hEnd; i++) {
				for (int j=wStart; j<wEnd; j++) {
					// add neighbours, get mean
					vector<unsigned short int> heighboursR, heighboursG, heighboursB, heighboursGrey;
					for (int k=i-kernelSize/2; k<i+kernelSize/2; k++) {
						for (int l=j-kernelSize/2; l<j+kernelSize/2; l++) {
							//if (k==i && l==j) continue; // scan except current pixed
							if (isColor == true) {
								heighboursR.push_back(data2D[k][l].r);
								heighboursG.push_back(data2D[k][l].g);
								heighboursB.push_back(data2D[k][l].b);
							} else {
								heighboursGrey.push_back(data2D[k][l].getGrey());
							}
						}
					}

					RGB pixel;
					if (isColor == true) {
						// sorting neighbours
						sort(heighboursR.begin(), heighboursR.end());
						sort(heighboursG.begin(), heighboursG.end());
						sort(heighboursB.begin(), heighboursB.end());
						int medianR = heighboursR[(kernelSize*2)/2];
						int medianG = heighboursG[(kernelSize*2)/2];
						int medianB = heighboursB[(kernelSize*2)/2];
						pixel.r = medianR;
						pixel.g = medianG;
						pixel.b = medianB;
					} else {
						sort(heighboursGrey.begin(), heighboursGrey.end());
						int medianGrey = heighboursGrey[(kernelSize*2)/2];
						pixel.r = pixel.g = pixel.b = medianGrey;
					}
					data2DR[i][j] = pixel;
				}
            }

            return data2DR;
		} // medianBlur end

//		/** Converts hex character (0-F) to actual integer value
//		 *
//		 * @param x an char argument.
//		 * @returns a int
//		 */
//        int hexChar2int(char x) {
//			return (x >= 'a') ? (x - 'a' + 10) : (x - '0');
//        }

        // helper methods ==========================================

		/** Writes/generates new BMP file from 2D array of pixel colors
		 *
		 * @param outFileName an string argument.
		 * @param data2D an vector< vector<RGB> > argument.
		 * @returns a int
		 */
        //void writeAsBMP (FILE*& bmfp, vector< vector< RGB > > data2D) { // if take file pointer
        void writeAsBMP (string outFileName, vector< vector<RGB> > data2D) {
        	FILE * fpw = fopen(outFileName.c_str(), "wb");

        	int height = data2D.size();
        	int width = data2D[0].size();

            fseek(fpw, 0, SEEK_SET);

            /** preparing header **/
            //if (type == 1) cout << ">>>>>> " << height << " <<<<<<<<<" << endl;
            int newPadding = getPadding(width);
            int newBiSizeImage = (width * 3 + newPadding) * height;
            int newBiSize = newBiSizeImage + 54;

            /** generating new header and write to file **/
            // bfType 2 byte
            fprintf(fpw, "%s", "BM"); // bfType.c_str()

            // bfSize 4 byte
            int * sizeBytes = dec2rbyte(newBiSize);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizeBytes[i]);

            // bfReserved 4 byte
            for (int i=0; i<4; i++) fprintf(fpw, "%c", 0);

            // bfOffBits 4 byte
            int * sizebfOffBits = dec2rbyte(54);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebfOffBits[i]);

            // biSize 4 byte
            int * sizebiSize = dec2rbyte(40);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiSize[i]);

            // biWidth 4 byte
            int * sizebiWidth = dec2rbyte(width);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiWidth[i]);

            // biHeight 4 byte
            int * sizebiHeight = dec2rbyte(height);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiHeight[i]);

            // biPlanes 2 byte
            int * sizebiPlanes = dec2rbyte(biPlanes);
            for (int i=0; i<2; i++) fprintf(fpw, "%c", sizebiPlanes[i]);

            // biBitCount 2 byte
            int * sizebiBitCount = dec2rbyte(biBitCount);
            for (int i=0; i<2; i++) fprintf(fpw, "%c", sizebiBitCount[i]);

            // biCompression 4 byte
            int * sizebiCompression = dec2rbyte(biCompression);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiCompression[i]);

            // biSizeImage 4 byte
            int * sizebiSizeImage = dec2rbyte(newBiSizeImage);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiSizeImage[i]);

            // biXPelsPerMeter 4 byte
            int * sizebiXPelsPerMeter = dec2rbyte(biXPelsPerMeter);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiXPelsPerMeter[i]);

            // biYPelsPerMeter 4 byte
            int * sizebiYPelsPerMeter = dec2rbyte(biYPelsPerMeter);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiYPelsPerMeter[i]);

            // biClrUsed 4 byte
            int * sizebiClrUsed = dec2rbyte(biClrUsed);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiClrUsed[i]);

            // biClrImportant 4 byte
            int * sizebiClrImportant = dec2rbyte(biClrImportant);
            for (int i=0; i<4; i++) fprintf(fpw, "%c", sizebiClrImportant[i]);

            /** writing pixel data **/
            // sorting back the array left to right, bottom to top
            fseek(fpw, 54, SEEK_SET);

            /** sorting left to right, bottom to top **/
            for (int i=height-1; i>=0; i--) {
                for (int j=0; j<width; j++) {
                    RGB pixel = data2D[i][j];
                    fprintf(fpw, "%c%c%c", pixel.b, pixel.g, pixel.r); // swap r with b
                }
                fwrite(new int[newPadding], 1, newPadding, fpw);
            }
            fclose(fpw);
        } // writeAsBMP end

		/** Converts integer to 4 different file-writable byte array in reverse order
		 *
		 * If fetchHorizontal is true then it fetch BMP image left to right else top to bottom
		 *	i/p- 24, o/p- 18 00 00 00
		 * @param x an int argument.
		 * @returns a int *
		 */
        int * dec2rbyte (int x) {
            int quotient, remainder, no;
            bool bin[32] = {};
            int * bytes = new int [4];

            quotient = x;

            int i=31;
            while (quotient != 0) {
                remainder = quotient % 2;
                bin[i--] = remainder;
                quotient /= 2;
            }

            for (int i=3; i>=0; i--) {
                no = 0;
                for (int j=0; j<8; j++) {
                    no |= bin[i*8+j] << abs(7-j);
                }
                bytes[abs(3-i)] = no;
            }

            return bytes;
        } // dec2rbyte end

		/** Calculate the padding or junk byte values of BMP from width
		 *
		 * @param width an int argument.
		 * @returns a int
		 */
        int getPadding (int width) {
            int padding;
            if ((width * 3)%4 == 0) padding = (width * 3) % 4;
            else padding = 4 - ((width * 3) % 4);
            return padding;
        } // getPadding end

		/** Calculate the padding or junk byte values of BMP from width
		 *
		 * @param width an int argument.
		 * @returns a int
		 */
        unsigned int* getHeightWidthByFN (string fn) {
            unsigned int * heightWidth = new unsigned int [2];
            FILE * fp = fopen(fn.c_str(), "rb");

            fseek(fp, 18, SEEK_SET);
            fread(&heightWidth[1], 4, 1, fp); // Read the width in binary format (signed integer)
            fread(&heightWidth[0], 4, 1, fp);
            fclose(fp);

            return heightWidth;
        }

		/** Resized a BMP
		 *
		 * @param data2D a vector< vector<RGB> > argument.
		 * @param target_width an int argument.
		 * @param target_height an int argument.
		 * @returns a vector< vector<RGB> >
		 */
		vector< vector<RGB> > resizeBMP (vector< vector<RGB> > data2D, int target_width, int target_height) {
			int height = data2D.size();
			int width = data2D[0].size();

			double scaleHeight = (double)target_height/(double)height;
			double scaleWidth = (double)target_width/(double)width;

			vector< vector<RGB> > resized_data2D;
			resized_data2D.resize(target_height);
			for (int i=0; i<target_height; i++) {
				resized_data2D[i].resize(target_width);
				for (int j=0; j<target_width; j++) resized_data2D[i][j] = RGB();
			}

			for (int i=0; i<target_height; i++) {
				for (int j=0; j<target_width; j++) {
					resized_data2D[i][j] = data2D[i/(double)scaleHeight][j/(double)scaleWidth];
				}
			}

			return resized_data2D;
		}

		/** Binarize a BMP
		 *
		 * @param data2D a vector< vector<RGB> > argument.
		 * @param threshold an int argument.
		 * @returns a vector< vector<RGB> >
		 */
		vector< vector<RGB> > binarizeBMP (vector< vector<RGB> > data2D, int threshold, bool invert=false) {
			int height = data2D.size();
			int width = data2D[0].size();
			int low, high;

			if (invert == false) {
				low = 0;
				high = 255;
			} else {
				low = 255;
				high = 0;
			}

			vector< vector<RGB> > resized_data2D;
			resized_data2D.resize(height);
			for (int i=0; i<height; i++) {
				resized_data2D[i].resize(width);
				for (int j=0; j<width; j++) resized_data2D[i][j] = RGB();
			}

			for (int i=0; i<height; i++) {
				for (int j=0; j<width; j++) {
					resized_data2D[i][j].r = data2D[i][j].getGrey() < threshold ? low : high;
					resized_data2D[i][j].g = data2D[i][j].getGrey() < threshold ? low : high;
					resized_data2D[i][j].b = data2D[i][j].getGrey() < threshold ? low : high;
				}
			}

			return resized_data2D;
		}

		/** Get white boundary limits i.e. white border from a BMP image, helper method of getBiggestContour
		 *
		 * @param data2D a vector< vector<RGB> > argument.
		 * @returns an int
		 */
		 int getWhiteBoundery (vector< vector<RGB> > data2D) {
		 	int height = data2D.size();
			int width = data2D[0].size();

			for (int i=0; i<height; i++) {
				bool isRowWhite = false;
				int sumOfRow = 0;

				for (int j=0; j<width; j++) {
                    if (data2D[i][j].getGrey() > 240) sumOfRow++;
				}

				if (sumOfRow == width) isRowWhite = true;

				if (isRowWhite == false) {
					return i;
					break;
				}
			}
		 }

		/** Get the biggest contour from a BMP i.e. cropping out unnecessary white borders
		 *
		 * @param data2D a vector< vector<RGB> > argument.
		 * @param threshold an int argument.
		 * @returns a vector< vector<RGB> >
		 */
		vector< vector<RGB> > getBiggestContour (vector< vector<RGB> > data2D) {
			int height = data2D.size();
			int width = data2D[0].size();
			int tl, tr, br, bl; // top-left, top-right, bottom-right, bottom-left
			int top, right, bottom, left;
			int newHeight, newWidth;

			top 	= getWhiteBoundery(data2D);
			data2D 	= rotateBMP(270);
			right 	= getWhiteBoundery(data2D);
			data2D 	= rotateBMP(270);
			bottom 	= getWhiteBoundery(data2D);
			data2D 	= rotateBMP(270);
			left 	= getWhiteBoundery(data2D);
			data2D 	= rotateBMP(270);

			newHeight = height - top - bottom;
			newWidth = width - left - right;

			vector< vector<RGB> > cropped_data2D;
			cropped_data2D.resize(newHeight);
			for (int i=0; i<newHeight; i++) {
				cropped_data2D[i].resize(newWidth);
				for (int j=0; j<newWidth; j++) cropped_data2D[i][j] = RGB();
			}

			for (int i=top, k=0; i<height-bottom; i++, k++) {
				for (int j=left, l=0; j<width-right; j++, l++) {
					cropped_data2D[k][l] = data2D[i][j];
				}
			}

			return cropped_data2D;
		}

		/** OCR on single digits
		 *
		 * @param data2D a vector< vector<RGB> > argument.
		 * @returns int
		 */
		int detectDigit (vector< vector<RGB> > data2D) {
            int height = data2D.size();
			int width = data2D[0].size();
			int num_and_sum[10];

			// get the biggest contour i.e. cropping out white portions
			data2D = getBiggestContour(data2D); // <========================================================================< here is the bug

			// invert the needle image
			data2D = binarizeBMP(data2D, 127, true);

            for (int i=0; i<10; i++) { // iterating throught // data/0.bmp to data/9.bmp
                string filename = "data/" + to_string(i) + ".bmp";
				BMP number(filename.c_str());
				vector< vector<RGB> > number_data2D = number.getBMPmatrix();

				// invert the haystack images
				number_data2D = binarizeBMP(number_data2D, 127, true);

				// calculating average size i.e. height and width
				int avgHeight = (number_data2D.size() + height) / 2;
				int avgWidth = (number_data2D[0].size() + width) / 2;

                // resize both haystack and needle image by haystack and needle
				data2D = resizeBMP(data2D, avgWidth, avgHeight);
				number_data2D = resizeBMP(number_data2D, avgWidth, avgHeight);

				// getting the image difference by XOR-ing
				vector< vector<RGB> > resized_data2D;
				resized_data2D.resize(avgHeight);
				for (int j=0; j<avgHeight; j++) {
					resized_data2D[j].resize(avgWidth);
					for (int k=0; k<avgWidth; k++) resized_data2D[j][k] = RGB();
				}

				for (int j=0; j<avgHeight; j++) {
					for (int k=0; k<avgWidth; k++) {
						resized_data2D[j][k].r = data2D[j][k].g ^ number_data2D[j][k].r;
						resized_data2D[j][k].g = data2D[j][k].g ^ number_data2D[j][k].g;
						resized_data2D[j][k].b = data2D[j][k].b ^ number_data2D[j][k].b;
					}
				}

				// sum up the difference
				unsigned long int sum = 0;
				for (int j=0; j<avgHeight; j++) {
					for (int k=0; k<avgWidth; k++) {
						sum += resized_data2D[j][k].getGrey() == 0 ? 0 : 1;
					}
				}

				// adding number and sum to array num_and_sum
				num_and_sum[i] = sum;

				//cout << endl << endl;
				//cout << i << ": " << sum << endl;

				string outfn = "data/temp/" + to_string(i) + ".bmp";
				writeAsBMP(outfn, resized_data2D);
            }

            // get the index of lowest sum i.e. the result
            int index = 0;
            for (int i=0; i<10; i++) {
				if (num_and_sum[i] < num_and_sum[index]) {
					index = i;
				}
            }

            return index;
		}

		/** OCR on numbers i.e. multiple digits
		 *
		 * @param data2D a vector< vector<RGB> > argument.
		 * @returns int
		 */
		int detectNumber (vector< vector<RGB> > data2D) {
			segmentGenerator(false, 220, filename);

			int number;
			FILE * fp;
			int i = 0;

			do {
				string fn = filename.substr(0, filename.size()-4) + "_word" + to_string(++i) + ".bmp";
				fp = fopen(fn.c_str(), "rb"); // to check file's existence
				//if (fp == NULL) cout << fn << " not found!" << endl;

				int digit = detectDigit(getBMPmatrix(fn));

				cout << digit << endl;

			} while (fp != NULL);

			fclose(fp);

			return 0;
		}

        ~BMP () {} // destructor
};

/************************************ main ***********************************/

int main (int * argc, char * argv[]) {
    char ch;
    do { // do until gets a valid file name
        system("cls"); // clrscr()
        cout << "BMP to TXT Converter:" << endl;
        cout << "=====================" << endl;
        cout << "Major: " << AutoVersion::MAJOR << ", Minor: " << AutoVersion::MINOR << endl;
        cout << "Build: " << AutoVersion::BUILD << ", Revesion: " << AutoVersion::REVISION << endl;
        cout << "Build no: " << AutoVersion::BUILDS_COUNT << endl;
        cout << "=====================" << endl;

		string filename;
		FILE * fp;
		do {
			cout << "Please enter path of a 24 bit BMP file: ";
			cin >> filename;
			fp = fopen(filename.c_str(), "r");

			if (fp == NULL) cout << "Invalid file name!" << endl;
			else fclose(fp);
		} while(fp == NULL);

        BMP img(filename.c_str());

        // main menu
		char sch = 'a';
		do { // do until choice is valid
			switch (sch) {
				case 'a': { // Print header
					img.printHeader(); // print on screen
					break;
				}
				case 'b': { // Export header
					img.exportHeader(); // print on screen and save as filename.header
					cout << "Header exported successfully..." << endl;
					break;
				}
				case 'c': { // Export color intensity
					img.exportHexColor();
					cout << "HEX color exported successfully..." << endl;
					break;
				}
				case 'd': { // Convert BMP to TEXT (bianarize)
					img.bmp2txt();
					cout << "BMP to TXT converted successfully..." << endl;
					break;
				}
				case 'e': { // Convert HEX to BMP
					string inFileName, outFileName;
					cout << "\n\tEnter input HEX file name: ";
					cin >> inFileName;
					cout << "\n\tEnter output BMP file name: ";
					cin >> outFileName;
					img.hex2bmp(inFileName, outFileName);
					cout << "HEX to BMO converted successfully..." << endl;
					break;
				}
				case 'f': { // Rotate BMP (90, 180, 270 degrees clockwise)
					unsigned int degree = 0;
					cout << "\n\tEnter degree (90/180/270): ";
					cin >> degree;
					img.rotateBMP(degree); // 90, 180, 270 degrees
					cout << "Rotated successfully..." << endl;
					break;
				}
				case 'g': { // Segmentize BMP
					img.generateSegments();
					cout << "Segmented successfully..." << endl;
					break;
				}
				case 'h': { // mdian blur testing
					int kernelSize;
					bool isColor;
					cout << "\n\tIs it a color image? (0=no, 1=yes) ";
					cin >> isColor;
					cout << "\n\tEnter kernel size (odd number): ";
					cin >> kernelSize;
					vector< vector<RGB> > data2D = img.getBMPmatrix();
					data2D = img.medianBlur(data2D, isColor, kernelSize);
					img.writeAsBMP("t/blurred.bmp", data2D);
					cout << "Median blur applied successfully..." << endl;
					break;
				}
				case 'i': { // histogram testing
					vector< vector<RGB> > data2D = img.getBMPmatrix();
					img.increaseContrast(data2D, 99, 220);
					data2D = img.medianBlur(data2D, false, 7);
					img.generateHistogram(data2D, filename);
					cout << "Histogram generated successfully..." << endl;
					break;
				}
				case 'j': { // resize bmp
					vector< vector<RGB> > data2D = img.getBMPmatrix();
					int newHeight, newWidth;
					cout << "\nPlease enter new width and height respectively: ";
					cin >> newHeight >> newWidth;
					data2D = img.resizeBMP(data2D, newHeight, newWidth);
					img.writeAsBMP("t/resized.bmp", data2D);
					break;
				}
				case 'k': { // binarize bmp
					vector< vector<RGB> > data2D = img.getBMPmatrix();
					int threshhold;
					bool invert;
					cout << "\nPlease enter threshhold for binarization: ";
					cin >> threshhold;
					cout << "\nDo you want to invert? ";
					cin >> invert;
					data2D = img.binarizeBMP(data2D, threshhold, invert);
					img.writeAsBMP("t/binarized.bmp", data2D);
					break;
				}
				case 'l': { // get biggest contour from bmp
					vector< vector<RGB> > data2D = img.getBMPmatrix();
					data2D = img.getBiggestContour(data2D);
					img.writeAsBMP("t/cropped_out.bmp", data2D);
					break;
				}
				case 'm': { // OCR single digit from bmp
					vector< vector<RGB> > data2D = img.getBMPmatrix();
					int digit = img.detectDigit(data2D);
					cout << "\nThe digit is: " << digit << endl;
					break;
				}
				case 'n': { // OCR number from bmp
					vector< vector<RGB> > data2D = img.getBMPmatrix();
					int number = img.detectNumber(data2D);
					cout << "\nThe number is: " << number << endl;
					break;
				}
			}

			//system("cls"); // clrscr()
			cout << "\n\ta. Print header" << endl;
			cout << "\tb. Export header" << endl;
			cout << "\tc. Export color intensity" << endl;
			cout << "\td. Convert BMP to TEXT (bianarize)" << endl;
			cout << "\te. Convert HEX to BMP" << endl;
			cout << "\tf. Rotate BMP (90, 180, 270 degrees clockwise)" << endl;
			cout << "\tg. Segmentize BMP" << endl;
			cout << "\th. Median blur" << endl;
			cout << "\ti. Generate histogram" << endl;
			cout << "\tj. Resize image" << endl;
			cout << "\tk. Binarize image" << endl;
			cout << "\tl. Crop out image" << endl;
			cout << "\tm. Detect single digit (OCR)" << endl;
			cout << "\to. Detect number (OCR)" << endl;
			cout << "\to. Exit" << endl;
			cout << "\tPlease enter your choice...";
			sch = getch();
		} while (sch >= 'a' && sch <= 'n');

        cout << "\nDo you want to process another BMP? Y or N";
        ch = getche();
    } while (ch == 'Y' || ch == 'y');

	return 0;
}
