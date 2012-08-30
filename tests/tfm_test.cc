
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <tfm.h>

int main(int argc, char *argv[])
   {
   if(argc==1) 
	  { cout << "Usage: test_tfm [.tfm file]" << endl;
	    exit(1);
	  }
   ifstream ff(argv[1], ios::binary | ios::in);
   if(!ff.is_open()) 
	  { cout << "test_tfm: cannot open " << argv[1] << endl;
	    exit(2);
	  }

	font_set_t mtfm;
   mtfm.read(ff);

	read 0;
   }
