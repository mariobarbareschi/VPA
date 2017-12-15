#include "vpa_n.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// VPA Library - C++ Interface
::vpa_n::VPA& ::vpa_n::VPA::operator=(VPA fp) {
  this->value = fp.value;
  this->name = fp.name;
  this->prec = fp.prec;
  return *this;
}

::vpa_n::VPA & ::vpa_n::VPA::operator=(float fp) {
  this->value = (long double) fp;
  this->prec = FLOAT;
  return *this;
}

::vpa_n::VPA & ::vpa_n::VPA::operator=(double fp) {
  this->value = (long double) fp;
  this->prec = DOUBLE;
  return *this;
}

::vpa_n::VPA & ::vpa_n::VPA::operator=(long double fp) {
  this->value = (long double) fp;
  this->prec = LONG_DOUBLE;
  return *this;
}


::vpa_n::VPA::operator float() const {
  return (float)this->value;
}

::vpa_n::VPA::operator double() const {
    return (double)this->value;
}

::vpa_n::VPA::operator long double() const {
    return (long double)this->value;
}

///////////////////////////////////////////////////////////////////////////////
::std::ostream &operator<<(::std::ostream &out, const ::vpa_n::VPA &fp) {
  if (fp.getName() != "") {
    out << fp.getName() << " - ";
  }
  out << "[";
  switch(fp.getPrec()){
	case vpa_n::VPAPrecision::FLOAT:
	out << "float";
	break;
	case vpa_n::VPAPrecision::DOUBLE:
	out << "double";
	break;
	default:
	out << "long double";
	break;
  }
  out << "] ---> " << (double)fp<<"\n";

  return out;
}

// Arithmetic operators
::vpa_n::VPA & ::vpa_n::VPA::
operator+=(const ::vpa_n::VPA &fp) {
  VPA rhs = fp;
  this->adaptPrec(rhs);
  this->value += rhs.value;
  return *this;
}

::vpa_n::VPA & ::vpa_n::VPA::
operator-=(const ::vpa_n::VPA &fp) {
  VPA rhs = fp;
  *this += (-rhs);
  return *this;
}

::vpa_n::VPA & ::vpa_n::VPA::
operator*=(const ::vpa_n::VPA &fp) {
  VPA rhs = fp;
  this->adaptPrec(rhs);
  this->value *= rhs.value;
  return *this;
}

::vpa_n::VPA & ::vpa_n::VPA::
operator/=(const ::vpa_n::VPA &fp) {
  VPA rhs = fp;
  this->adaptPrec(rhs);
  this->value /= rhs.value;
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
void ::vpa_n::VPA::changePrec(::vpa_n::VPAPrecision new_prec) {
  switch(new_prec){
	case FLOAT:
	this->value = (float)this->value;
	break;
	case DOUBLE:
	this->value = (double)this->value;
	break;
	default:
	this->value = (long double)this->value;
	break;
  }
  this->prec = new_prec;
}

void ::vpa_n::VPA::adaptPrec(::vpa_n::VPA &rhs) {
	switch(prec){
		case FLOAT:
			if(rhs.getPrec() != FLOAT){
				if(UPCASTING)
					this->changePrec(rhs.getPrec());
				else
					rhs.changePrec(FLOAT);
			}
		break;
		case DOUBLE:
			if(rhs.getPrec() != DOUBLE){
					if(rhs.getPrec() == LONG_DOUBLE){
						if(UPCASTING)
							this->changePrec(rhs.getPrec());
						else
							rhs.changePrec(DOUBLE);
					} else { //RHS is float
						if(UPCASTING)
							rhs.changePrec(DOUBLE);
						else
							this->changePrec(FLOAT);
					}
				}
		break;
		default: // long double
			if(rhs.getPrec() != LONG_DOUBLE){
				if(UPCASTING)
					rhs.changePrec(LONG_DOUBLE);
				else
					this->changePrec(rhs.getPrec());
			}
		break;
	}
}
