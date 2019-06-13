#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <sys/stat.h> 
#include <stdlib.h>

#define debug 0

class MyTail
{
private:
	std::list<std::string> mLastNLine;
	const int mNoOfLines;
	std::ifstream mIn;

public:
	explicit MyTail(int pNoOfLines):mNoOfLines(pNoOfLines) {}
	
	const int getNoOfLines() {return mNoOfLines; }
	
	void getLastNLines();

	void printLastNLines();
	
	void tailF(const char* filename);
	
};

void MyTail::getLastNLines() 
{
	if (debug) std::cout << "In: getLastNLines()" << std::endl;

	mIn.seekg(-1,std::ios::end);
	int pos=mIn.tellg();
	int count = 1;
	
	//Get file pointer to point to bottom up mNoOfLines.
	for(int i=0;i<pos;i++)
	{
		if (mIn.get() == '\n')
			if (count++ > mNoOfLines)
				break;
		mIn.seekg(-2,std::ios::cur);
	}
	
	//Start copying bottom mNoOfLines string into list to avoid I/O calls to print lines
	std::string line;
	while(getline(mIn,line)) {
		int data_Size = mLastNLine.size();
		if(data_Size >= mNoOfLines) {
			mLastNLine.pop_front();
		}
		mLastNLine.push_back(line);
	}
	
	if (debug) std::cout << "Out: getLastNLines()" << std::endl;
}

void MyTail::printLastNLines()
{    
	 for (std::list<std::string>::iterator i = mLastNLine.begin();  i != mLastNLine.end(); ++i)
		std::cout << *i << std::endl;
}

void MyTail::tailF(const char* filename)
{
	if (debug) std::cout << "In: TailF()" << std::endl;
	
	int date = 0;
	while (true) {
		struct stat st;
		int ierr = stat (filename, &st);
		int newdate = st.st_mtime;
		if (newdate != date){
			system("@cls||clear");
			std::cout << "Print last " << getNoOfLines() << " Lines: \n";
			mIn.open(filename);
			date = newdate;
			getLastNLines();
			mIn.close();
			printLastNLines();
		}
	}
	
	if (debug) std::cout << "Out: TailF()" << std::endl;		
}

int main(int argc, char **argv)
{
	if(argc==1) {
        std::cout << "No Extra Command Line Argument Passed Other Than Program Name\n"; 
		return 0;
	}
		
	if(argc>=2) {
		MyTail t1(10);
		t1.tailF(argv[1]);
	}
	return 0;
}
