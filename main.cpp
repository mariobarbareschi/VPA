#include <iostream>

#include "vpa_n.h"

bool vpa_n::VPA::UPCASTING = false;

int main(){
	vpa_n::VPA a((long double)1.1);
	vpa_n::VPA b((double)1.2);
	vpa_n::VPA c((float)1.3);
	
	vpa_n::VPA x = a+b;
	vpa_n::VPA y = a+b*c;

	std::cout << a.getPrec() << std::endl;
	std::cout << b.getPrec() << std::endl;
	std::cout << c.getPrec() << std::endl;
	std::cout << x.getPrec() << std::endl;
	std::cout << y.getPrec() << std::endl;
	return 0;

}
